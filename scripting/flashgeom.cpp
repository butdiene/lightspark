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

#include "abc.h"
#include "flashgeom.h"
#include "class.h"

using namespace lightspark;
using namespace std;

SET_NAMESPACE("flash.geom");

REGISTER_CLASS_NAME(Transform);
REGISTER_CLASS_NAME(ColorTransform);
REGISTER_CLASS_NAME(Point);
REGISTER_CLASS_NAME2(lightspark::Rectangle,"Rectangle","flash.geom");

void Rectangle::sinit(Class_base* c)
{
	c->setConstructor(Class<IFunction>::getFunction(_constructor));
	IFunction* gleft=Class<IFunction>::getFunction(_getLeft);
	c->setGetterByQName("left","",gleft);
	gleft->incRef();
	c->setGetterByQName("x","",gleft);
	IFunction* sleft=Class<IFunction>::getFunction(_setLeft);
	c->setSetterByQName("left","",sleft);
	sleft->incRef();
	c->setSetterByQName("x","",sleft);
	c->setGetterByQName("right","",Class<IFunction>::getFunction(_getRight));
	c->setSetterByQName("right","",Class<IFunction>::getFunction(_setRight));
	c->setGetterByQName("width","",Class<IFunction>::getFunction(_getWidth));
	c->setSetterByQName("width","",Class<IFunction>::getFunction(_setWidth));

	IFunction* gtop=Class<IFunction>::getFunction(_getTop);
	c->setGetterByQName("top","",gtop);
	gtop->incRef();
	c->setGetterByQName("y","",gtop);
	IFunction* stop=Class<IFunction>::getFunction(_setTop);
	c->setSetterByQName("top","",stop);
	stop->incRef();
	c->setSetterByQName("y","",stop);

	c->setGetterByQName("bottom","",Class<IFunction>::getFunction(_getBottom));
	c->setSetterByQName("bottom","",Class<IFunction>::getFunction(_setBottom));
	c->setGetterByQName("height","",Class<IFunction>::getFunction(_getHeight));
	c->setSetterByQName("height","",Class<IFunction>::getFunction(_setHeight));

	c->setVariableByQName("clone","",Class<IFunction>::getFunction(clone));
}

void Rectangle::buildTraits(ASObject* o)
{
}

tiny_string Rectangle::toString(bool debugMsg)
{
	assert_and_throw(implEnable);
	
	char buf[512];
	snprintf(buf,512,"(x=%f, y=%f, w=%f, h=%f)",x,y,width,height);
	
	return tiny_string(buf, true);
}

const lightspark::RECT Rectangle::getRect() const
{
	return lightspark::RECT(x,y,x+width,y+height);
}

ASFUNCTIONBODY(Rectangle,_constructor)
{
	Rectangle* th=static_cast<Rectangle*>(obj);

	if(argslen>=1)
		th->x=args[0]->toNumber();
	if(argslen>=2)
		th->y=args[1]->toNumber();
	if(argslen>=3)
		th->width=args[2]->toNumber();
	if(argslen>=4)
		th->height=args[3]->toNumber();

	return NULL;
}

ASFUNCTIONBODY(Rectangle,_getLeft)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	return abstract_d(th->x);
}

ASFUNCTIONBODY(Rectangle,_setLeft)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	assert_and_throw(argslen==1);
	th->x=args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Rectangle,_getRight)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	return abstract_d(th->x + th->width);
}

ASFUNCTIONBODY(Rectangle,_setRight)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	assert_and_throw(argslen==1);
	th->width=(args[0]->toNumber()-th->x);
	return NULL;
}

ASFUNCTIONBODY(Rectangle,_getWidth)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	return abstract_d(th->width);
}

ASFUNCTIONBODY(Rectangle,_setWidth)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	assert_and_throw(argslen==1);
	th->width=args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Rectangle,_getTop)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	return abstract_d(th->y);
}

ASFUNCTIONBODY(Rectangle,_setTop)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	assert_and_throw(argslen==1);
	th->y=args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Rectangle,_getBottom)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	return abstract_d(th->y + th->height);
}

ASFUNCTIONBODY(Rectangle,_setBottom)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	assert_and_throw(argslen==1);
	th->height=(args[0]->toNumber()-th->y);
	return NULL;
}

ASFUNCTIONBODY(Rectangle,_getHeight)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	return abstract_d(th->height);
}

ASFUNCTIONBODY(Rectangle,_setHeight)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	assert_and_throw(argslen==1);
	th->height=args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Rectangle,clone)
{
	Rectangle* th=static_cast<Rectangle*>(obj);
	Rectangle* ret=Class<Rectangle>::getInstanceS();
	ret->x=th->x;
	ret->y=th->y;
	ret->width=th->width;
	ret->height=th->height;
	return ret;
}

void ColorTransform::sinit(Class_base* c)
{
	c->setConstructor(Class<IFunction>::getFunction(_constructor));
}

ASFUNCTIONBODY(ColorTransform,_constructor)
{
	ColorTransform* th=static_cast<ColorTransform*>(obj);
	assert_and_throw(argslen<=8);
	if(0 < argslen)
		th->redMultiplier=args[0]->toNumber();
	else
		th->redMultiplier=1.0;
	if(1 < argslen)
		th->greenMultiplier=args[1]->toNumber();
	else
		th->greenMultiplier=1.0;
	if(2 < argslen)
		th->blueMultiplier=args[2]->toNumber();
	else
		th->blueMultiplier=1.0;
	if(3 < argslen)
		th->alphaMultiplier=args[3]->toNumber();
	else
		th->alphaMultiplier=1.0;
	if(4 < argslen)
		th->redOffset=args[4]->toNumber();
	else
		th->redOffset=0.0;
	if(5 < argslen)
		th->greenOffset=args[5]->toNumber();
	else
		th->greenOffset=0.0;
	if(6 < argslen)
		th->blueOffset=args[6]->toNumber();
	else
		th->blueOffset=0.0;
	if(7 < argslen)
		th->alphaOffset=args[7]->toNumber();
	else
		th->alphaOffset=0.0;
	return NULL;
}

ASFUNCTIONBODY(ColorTransform,setColor)
{
	ColorTransform* th=static_cast<ColorTransform*>(obj);
	assert_and_throw(argslen==1);
	uintptr_t tmp=args[0]->toInt();
	//Setting multiplier to 0
	th->redMultiplier=0;
	th->greenMultiplier=0;
	th->blueMultiplier=0;
	th->alphaMultiplier=0;
	//Setting offset to the input value
	th->alphaOffset=(tmp>>24)&0xff;
	th->redOffset=(tmp>>16)&0xff;
	th->greenOffset=(tmp>>8)&0xff;
	th->blueOffset=tmp&0xff;
	return NULL;
}

ASFUNCTIONBODY(ColorTransform,getColor)
{
	assert_and_throw(false && "getColor not implemented");
	return NULL;
}

void Point::sinit(Class_base* c)
{
	c->setConstructor(Class<IFunction>::getFunction(_constructor));
	c->setGetterByQName("x","",Class<IFunction>::getFunction(_getX));
	c->setGetterByQName("y","",Class<IFunction>::getFunction(_getY));
	c->setGetterByQName("length","",Class<IFunction>::getFunction(_getlength));
	c->setSetterByQName("x","",Class<IFunction>::getFunction(_setX));
	c->setSetterByQName("y","",Class<IFunction>::getFunction(_setY));
	c->setVariableByQName("interpolate","",Class<IFunction>::getFunction(interpolate));
	c->setVariableByQName("distance","",Class<IFunction>::getFunction(distance));
	c->setVariableByQName("add","",Class<IFunction>::getFunction(add));
	c->setVariableByQName("subtract","",Class<IFunction>::getFunction(subtract));
	c->setVariableByQName("clone","",Class<IFunction>::getFunction(clone));
	c->setVariableByQName("equals","",Class<IFunction>::getFunction(equals));
	c->setVariableByQName("normalize","",Class<IFunction>::getFunction(normalize));
	c->setVariableByQName("offset","",Class<IFunction>::getFunction(offset));
	c->setVariableByQName("polar","",Class<IFunction>::getFunction(polar));
}

void Point::buildTraits(ASObject* o)
{
}

tiny_string Point::toString(bool debugMsg)
{
	assert_and_throw(implEnable);
	
	char buf[512];
	snprintf(buf,512,"(a=%f, b=%f)",x,y);
	
	return tiny_string(buf, true);
}

number_t Point::len() const
{
	return sqrt(x*x + y*y);
}

ASFUNCTIONBODY(Point,_constructor)
{
	Point* th=static_cast<Point*>(obj);
	if(argslen>=1)
		th->x=args[0]->toNumber();
	if(argslen>=2)
		th->y=args[1]->toNumber();

	return NULL;
}

ASFUNCTIONBODY(Point,_getX)
{
	Point* th=static_cast<Point*>(obj);
	return abstract_d(th->x);
}

ASFUNCTIONBODY(Point,_getY)
{
	Point* th=static_cast<Point*>(obj);
	return abstract_d(th->y);
}

ASFUNCTIONBODY(Point,_setX)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==1);
	th->x = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Point,_setY)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==1);
	th->y = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Point,_getlength)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==0);
	return abstract_d(th->len());
}

ASFUNCTIONBODY(Point,interpolate)
{
	assert_and_throw(argslen==3);
	Point* pt1=static_cast<Point*>(args[0]);
	Point* pt2=static_cast<Point*>(args[1]);
	number_t f=args[2]->toNumber();
	Point* ret=Class<Point>::getInstanceS();
	ret->x = pt1->x + pt2->x * f;
	ret->y = pt1->y + pt2->y * f;
	return ret;
}

ASFUNCTIONBODY(Point,distance)
{
	assert_and_throw(argslen==2);
	Point* pt1=static_cast<Point*>(args[0]);
	Point* pt2=static_cast<Point*>(args[1]);
	Point temp(pt2->x - pt1->x, pt2->y - pt1->x);
	return abstract_d(temp.len());
}

ASFUNCTIONBODY(Point,add)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==1);
	Point* v=static_cast<Point*>(args[0]);
	Point* ret=Class<Point>::getInstanceS();
	ret->x = th->x + v->x;
	ret->y = th->y + v->y;
	return ret;
}

ASFUNCTIONBODY(Point,subtract)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==1);
	Point* v=static_cast<Point*>(args[0]);
	Point* ret=Class<Point>::getInstanceS();
	ret->x = th->x - v->x;
	ret->y = th->y - v->y;
	return ret;
}

ASFUNCTIONBODY(Point,clone)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==0);
	Point* ret=Class<Point>::getInstanceS();
	ret->x = th->x;
	ret->y = th->y;
	return ret;
}

ASFUNCTIONBODY(Point,equals)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==1);
	Point* toCompare=static_cast<Point*>(args[0]);
	return abstract_b((th->x == toCompare->x) & (th->y == toCompare->y));
}

ASFUNCTIONBODY(Point,normalize)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen<2);
	number_t thickness = argslen > 0 ? args[0]->toNumber() : 1.0;
	number_t len = th->len();
	//What if len == 0?
	th->x = th->x * thickness / len;
	th->y = th->y * thickness / len;
	return NULL;
}

ASFUNCTIONBODY(Point,offset)
{
	Point* th=static_cast<Point*>(obj);
	assert_and_throw(argslen==2);
	number_t dx = args[0]->toNumber();
	number_t dy = args[1]->toNumber();
	Point* ret=Class<Point>::getInstanceS();
	ret->x = th->x + dx;
	ret->y = th->y + dy;
	return ret;
}

ASFUNCTIONBODY(Point,polar)
{
	assert_and_throw(argslen==2);
	number_t len = args[0]->toNumber();
	number_t angle = args[1]->toNumber();
	Point* ret=Class<Point>::getInstanceS();
	ret->x = len * cos(angle);
	ret->y = len * sin(angle);
	return ret;
}

void Transform::sinit(Class_base* c)
{
	//c->constructor=Class<IFunction>::getFunction(_constructor);
	c->setConstructor(NULL);
	c->setSetterByQName("colorTransform","",Class<IFunction>::getFunction(undefinedFunction));
}

void Transform::buildTraits(ASObject* o)
{
}

void Matrix::sinit(Class_base* c)
{
	c->setConstructor(Class<IFunction>::getFunction(_constructor));
	
	//Properties
	c->setGetterByQName("a","",Class<IFunction>::getFunction(_get_a));
	c->setGetterByQName("b","",Class<IFunction>::getFunction(_get_b));
	c->setGetterByQName("c","",Class<IFunction>::getFunction(_get_c));
	c->setGetterByQName("d","",Class<IFunction>::getFunction(_get_d));
	c->setGetterByQName("tx","",Class<IFunction>::getFunction(_get_tx));
	c->setGetterByQName("ty","",Class<IFunction>::getFunction(_get_ty));
	
	c->setSetterByQName("a","",Class<IFunction>::getFunction(_set_a));
	c->setSetterByQName("b","",Class<IFunction>::getFunction(_set_b));
	c->setSetterByQName("c","",Class<IFunction>::getFunction(_set_c));
	c->setSetterByQName("d","",Class<IFunction>::getFunction(_set_d));
	c->setSetterByQName("tx","",Class<IFunction>::getFunction(_set_tx));
	c->setSetterByQName("ty","",Class<IFunction>::getFunction(_set_ty));
	
	//Methods 
	c->setVariableByQName("identity","",Class<IFunction>::getFunction(identity));
	c->setVariableByQName("rotate","",Class<IFunction>::getFunction(rotate));
	c->setVariableByQName("scale","",Class<IFunction>::getFunction(scale));
	c->setVariableByQName("translate","",Class<IFunction>::getFunction(translate));
}

ASFUNCTIONBODY(Matrix,_constructor)
{
	ASObject::_constructor(obj,NULL,0);
	
	Matrix* th=static_cast<Matrix*>(obj);
	
	//Identity matrix
	if(argslen!=6)
	{
		th->a = 1.0; th->c = 0.0; th->tx = 0.0;
		th->b = 0.0; th->d = 0.0; th->ty = 0.0;
	}
	else
	{
		//Initialize from args
		th->a = args[0]->toNumber();
		th->b = args[0]->toNumber();
		th->c = args[0]->toNumber();
		th->d = args[0]->toNumber();
		th->tx = args[0]->toNumber();
		th->ty = args[0]->toNumber();
	}

	return NULL;
}

void Matrix::buildTraits(ASObject* o)
{
}

tiny_string Matrix::toString(bool debugMsg)
{
	assert_and_throw(implEnable);
	
	char buf[512];
	snprintf(buf,512,"(a=%f, b=%f, c=%f, d=%f, tx=%f, ty=%f)",
		a, b, c, d, tx, ty);
	
	return tiny_string(buf, true);
}

ASFUNCTIONBODY(Matrix,_get_a)
{
	Matrix* th=static_cast<Matrix*>(obj);
	return abstract_d(th->a);
}

ASFUNCTIONBODY(Matrix,_set_a)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	th->a = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Matrix,_get_b)
{
	Matrix* th=static_cast<Matrix*>(obj);
	return abstract_d(th->b);
}

ASFUNCTIONBODY(Matrix,_set_b)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	th->b = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Matrix,_get_c)
{
	Matrix* th=static_cast<Matrix*>(obj);
	return abstract_d(th->c);
}

ASFUNCTIONBODY(Matrix,_set_c)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	th->c = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Matrix,_get_d)
{
	Matrix* th=static_cast<Matrix*>(obj);
	return abstract_d(th->d);
}

ASFUNCTIONBODY(Matrix,_set_d)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	th->d = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Matrix,_get_tx)
{
	Matrix* th=static_cast<Matrix*>(obj);
	return abstract_d(th->tx);
}

ASFUNCTIONBODY(Matrix,_set_tx)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	th->tx = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Matrix,_get_ty)
{
	Matrix* th=static_cast<Matrix*>(obj);
	return abstract_d(th->ty);
}

ASFUNCTIONBODY(Matrix,_set_ty)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	th->ty = args[0]->toNumber();
	return NULL;
}

ASFUNCTIONBODY(Matrix,identity)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==0);
	
	th->a = 1.0; th->c = 0.0; th->tx = 0.0;
	th->b = 0.0; th->d = 0.0; th->ty = 0.0;
		
	return NULL;
}

ASFUNCTIONBODY(Matrix,rotate)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==1);
	double angle = args[0]->toNumber();
	th->a = ::cos(angle); th->c = -::sin(angle); th->tx = 0.0;
	th->b = ::sin(angle); th->d =  ::cos(angle); th->ty = 0.0;
		
	return NULL;
}

ASFUNCTIONBODY(Matrix,scale)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==2);
	double sx = args[0]->toNumber();
	double sy = args[1]->toNumber();
	th->a = sx;   th->c = 0.0; th->tx = 0.0;
	th->b = 0.0;  th->d = sy;  th->ty = 0.0;
		
	return NULL;
}

ASFUNCTIONBODY(Matrix,translate)
{
	Matrix* th=static_cast<Matrix*>(obj);
	assert_and_throw(argslen==2);
	double dx = args[0]->toNumber();
	double dy = args[1]->toNumber();
	th->tx += dx;
	th->ty += dy;
		
	return NULL;
}
