/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "swf.h"
#include "urlutils.h"
#include "compat.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#ifdef ENABLE_CURL
#include <curl/curl.h>
#endif

using namespace lightspark;
extern TLSDATA SystemState* sys;

URLInfo::URLInfo(const tiny_string& u)
{
	url = u;

	std::string str = std::string(url.raw_buf());

	valid = false;

	//Check for :// marking that there is a protocol in this url
	size_t colonPos = str.find("://");
	if(colonPos == std::string::npos)
		invalidReason = MISSING_PROTOCOL;

	protocol = str.substr(0, colonPos);

	size_t hostnamePos = colonPos+3;
	size_t portPos = std::string::npos;
	size_t pathPos = std::string::npos;
	size_t queryPos = std::string::npos;
	size_t fragmentPos = std::string::npos;
	size_t cursor = hostnamePos;

	if(protocol == "file")
	{
		pathPos = cursor;
		if(pathPos == str.length())
			invalidReason = MISSING_PATH;
	}
	else
	{
		//URLEncode spaces by default. This is just a safety check.
		//Full encoding should be performed BEFORE passing the url to URLInfo
		url = encode(u, ENCODE_SPACES);

		pathPos = str.find("/", cursor);
		cursor = pathPos;

		portPos = str.rfind(":", cursor);
		if(portPos < hostnamePos)
			portPos = std::string::npos;

		queryPos = str.find("?", cursor);
		if(queryPos != std::string::npos)
			cursor = queryPos;
		fragmentPos = str.find("#", cursor);

		if(portPos == hostnamePos || pathPos == hostnamePos || queryPos == hostnamePos)
			invalidReason = MISSING_HOSTNAME;
	}

	//Parse the host string
	hostname = str.substr(hostnamePos, std::min(std::min(pathPos, portPos), queryPos)-hostnamePos);

	port = 0;
	//Check if the port after ':' is not empty
	if(portPos != std::string::npos && portPos != std::min(std::min(str.length()-1, pathPos-1), queryPos-1))
	{
		portPos++;
		std::istringstream i(str.substr(portPos, std::min(pathPos, str.length())-portPos));
		if(!(i >> port))
			invalidReason = INVALID_PORT;
	}

	//Parse the path string
	if(pathPos != std::string::npos)
	{
		std::string pathStr = str.substr(pathPos, std::min(str.length(), queryPos)-pathPos);
		path = tiny_string(pathStr);
		path = normalizePath(path);
		pathStr = std::string(path.raw_buf());

		size_t lastSlash = pathStr.rfind("/");
		if(lastSlash != std::string::npos)
		{
			pathDirectory = pathStr.substr(0, lastSlash+1);
			pathFile = pathStr.substr(lastSlash+1);
		}
		//We only get here when parsing a file://abc URL
		else
		{
			pathDirectory = "";
			pathFile = path;
		}
	}
	else
	{
		path = "/";
		pathDirectory = path;
		pathFile = "";
	}

	//Copy the query string
	if(queryPos != std::string::npos && queryPos < str.length()-1)
		query = str.substr(queryPos+1);
	else
		query = "";

	//Copy the query string
	if(fragmentPos != std::string::npos && fragmentPos < str.length()-1)
		fragment = str.substr(fragmentPos+1);
	else
		fragment = "";

	//Create a new normalized and encoded string representation
	parsedURL = getProtocol();
	parsedURL += "://";
	parsedURL += getHostname();
	if(getPort() > 0)
	{
		parsedURL += ":";
		parsedURL += tiny_string((int) getPort());
	}
	parsedURL += getPath();
	if(query != "")
	{
		parsedURL += "?";
		parsedURL += getQuery();
	}
	if(fragment != "")
	{
		parsedURL += "#";
		parsedURL += getFragment();
	}

	valid = true;
}

tiny_string URLInfo::normalizePath(const tiny_string& u)
{
	std::string pathStr(u.raw_buf());

	//Remove double slashes
	size_t doubleSlash = pathStr.find("//");
	while(doubleSlash != std::string::npos)
	{
		pathStr.replace(doubleSlash, 2, "/");
		doubleSlash = pathStr.find("//");
	}

	//Parse all /../
	size_t doubleDot = pathStr.find("/../");
	size_t previousSlash;
	while(doubleDot != std::string::npos)
	{
		//We are at root, .. doesn't mean anything, just erase
		if(doubleDot == 0)
		{
			pathStr.replace(doubleDot, 3, "");
		}
		//Replace .. with one dir up
		else
		{
			previousSlash = pathStr.rfind("/", doubleDot-2);
			pathStr.replace(previousSlash, doubleDot-previousSlash+3, "");
		}
		doubleDot = pathStr.find("/../");
	}

	//Replace last /.. with one dir up
	if(pathStr.length() >= 3 && pathStr.substr(pathStr.length()-3, 3) == "/..")
	{
		previousSlash = pathStr.rfind("/", pathStr.length()-4);
		pathStr.replace(previousSlash, pathStr.length()-previousSlash+2, "/");
	}

	//Eliminate meaningless /./
	size_t singleDot = pathStr.find("/./");
	while(singleDot != std::string::npos)
	{
		pathStr.replace(singleDot, 2, "");
		singleDot = pathStr.find("/./");
	}


	//Remove redundant last dot
	if(pathStr.length() >= 2 && pathStr.substr(pathStr.length()-2, 2) == "/.")
		pathStr.replace(pathStr.length()-1, 1, "");
	if(pathStr.length() == 1 && pathStr.substr(pathStr.length()-1, 1) == ".")
		pathStr.replace(pathStr.length()-1, 1, "");

	return tiny_string(pathStr);
}

const URLInfo URLInfo::goToURL(const tiny_string& u) const
{
	std::string str = u.raw_buf();

	//No protocol, treat this as an unqualified URL
	if(str.find("://") == std::string::npos)
	{
		tiny_string qualified;

		qualified = getProtocol();
		qualified += "://";
		qualified += getHostname();
		if(getPort() > 0)
		{
			qualified += ":";
			qualified += tiny_string((int) getPort());
		}
		qualified += getPathDirectory();
		qualified += str;
		return URLInfo(qualified);
	}
	else //Protocol specified, treat this as a qualified URL
		return URLInfo(u);
}

bool URLInfo::isSubOf(const URLInfo& url) const
{
	//Check if the beginning of the new pathDirectory equals the old pathDirectory
	if(getPathDirectory().substr(0, url.getPathDirectory().len()) != url.getPathDirectory())
		return false;
	else
		return true;
}

std::string URLInfo::encode(const std::string& u, ENCODING type)
{
	std::string str;
	//Worst case, we need to encode EVERY character: X --> %YZ
	//Expect moderate density, assume we need to encode about half of all characters
	str.reserve(u.length()*2);
	char buf[4];
	
	for(size_t i=0;i<u.length();i++)
	{
		if(type == ENCODE_SPACES)
		{
			if(u[i] == ' ')
				str += "%20";
			else
				str += u[i];
		}
		else {
			//A-Z, a-z or 0-9, all encoding types except encode spaces don't encode these characters
			if((u[i] >= 0x41 && u[i] <= 0x5a) || (u[i] >= 0x61 && u[i] <= 0x7a) || (u[i] >= 0x30 && u[i] <= 0x39))
				str += u[i];
			//Additionally ENCODE_FORM doesn't decode: - _ . ~
			else if(type == ENCODE_FORM && 
					(u[i] == '-' || u[i] == '_' || u[i] == '.' || u[i] == '~'))
				str += u[i];
			//ENCODE_FORM encodes spaces as + instead of %20
			else if(type == ENCODE_FORM && u[i] == ' ')
				str += '+';
			//Additionally ENCODE_URICOMPONENT and ENCODE_URI don't encode:
			//- _ . ! ~ * ' ( )
			else if((type == ENCODE_URI || ENCODE_URICOMPONENT) && 
					(u[i] == '-' || u[i] == '_' || u[i] == '.' || u[i] == '!' || 
					 u[i] == '~' || u[i] == '*' || u[i] == '\'' ||	u[i] == '(' || 
					 u[i] == ')'))
				str += u[i];
			//Additionally ENCODE_URI doesn't encode:
			//; / ? : @ & = + $ , # 
			else if(type == ENCODE_URI && 
						(u[i] == ';' || u[i] == '/' || u[i] == '?' || u[i] == ':' || 
						 u[i] == '@' || u[i] == '&' || u[i] == '=' || u[i] == '+' || 
						 u[i] == '$' || u[i] == ',' || u[i] == '#'))
				str += u[i];
			//Additionally ENCODE_ESCAPE doesn't encode:
			//@ - _ . * + /
			else if(type == ENCODE_ESCAPE && 
						(u[i] == '@' || u[i] == '-' || u[i] == '_' || u[i] == '.' || 
						 u[i] == '*' || u[i] == '+' || u[i] == '/'))
				str += u[i];

			else
			{
				sprintf(buf,"%%%X",(unsigned char)u[i]);
				str += buf;
			}
		}
	}

	return str;
}

std::string URLInfo::decode(const std::string& u, ENCODING type)
{
	std::string str;
	//The string can only shrink
	str.reserve(u.length());

	std::string stringBuf;
	stringBuf.reserve(3);

	for(size_t i=0;i<u.length();i++)
	{
		if(i > u.length()-3 || u[i] != '%')
			str += u[i];
		else
		{
			stringBuf = u[i];
			stringBuf += u[i+1];
			stringBuf += u[i+2];
			std::transform(stringBuf.begin(), stringBuf.end(), stringBuf.begin(), toupper);

			//ENCODE_SPACES only decodes %20 to space
			if(type == ENCODE_SPACES && stringBuf == "%20")
				str += " ";
			//ENCODE_SPACES doesn't decode other characters
			else if(type == ENCODE_SPACES)
			{
				str += stringBuf;
				i+=2;
			}
			//ENCODE_URI and ENCODE_URICOMPONENT don't decode:
			//- _ . ! ~ * ' ( ) 
			else if((type == ENCODE_URI || type == ENCODE_URICOMPONENT) && 
					(stringBuf == "%2D" || stringBuf == "%5F" || stringBuf == "%2E" || stringBuf == "%21" ||
					 stringBuf == "%7E" || stringBuf == "%2A" || stringBuf == "%27" || stringBuf == "%28" || 
					 stringBuf == "%29"))
			{
				str += stringBuf;
				i+=2;
			}
			//Additionally ENCODE_URI doesn't decode:
			//; / ? : @ & = + $ , # 
			else if(type == ENCODE_URI && 
					(stringBuf == "%23" || stringBuf == "%24" || stringBuf == "%26" || stringBuf == "%2B" ||
					 stringBuf == "%2C" || stringBuf == "%2F" || stringBuf == "%3A" || stringBuf == "%3B" ||
					 stringBuf == "%3D" || stringBuf == "%3F" || stringBuf == "%40"))
			{
				str += stringBuf;
				i+=2;
			}
			//All encoded characters that weren't excluded above are now decoded
			else
			{	
				i++;
				str += (unsigned char) strtoul(u.substr(i, 2).c_str(), NULL, 16);
				i++;
			}
		}
	}
	return str;
}

