/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "abc.h"
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Constants.h> 
#include <llvm/Support/IRBuilder.h> 
#include <llvm/Target/TargetData.h>
#include <sstream>
#include "swftypes.h"

void debug(int* i)
{
	LOG(CALLS, "debug "<< i);
}

void debug_d(number_t f)
{
	LOG(CALLS, "debug_d "<< f);
}

void debug_i(intptr_t i)
{
	LOG(CALLS, "debug_i "<< i);
}

opcode_handler ABCVm::opcode_table_args0_lazy[]={
	{"getGlobalScope",(void*)&ABCVm::getGlobalScope}
};

opcode_handler ABCVm::opcode_table_args0[]={
	{"pushScope",(void*)&ABCVm::pushScope},
	{"pushWith",(void*)&ABCVm::pushWith},
	{"throw",(void*)&ABCVm::_throw},
	{"popScope",(void*)&ABCVm::popScope},
	{"asTypelate",(void*)&ABCVm::asTypelate},
};

opcode_handler ABCVm::opcode_table_args1_lazy[]={
	{"pushString",(void*)&ABCVm::pushString},
	{"pushDouble",(void*)&ABCVm::pushDouble},
	{"pushInt",(void*)&ABCVm::pushInt},
	{"newFunction",(void*)&ABCVm::newFunction},
	{"newCatch",(void*)&ABCVm::newCatch},
	{"getScopeObject",(void*)&ABCVm::getScopeObject}
};

opcode_handler ABCVm::opcode_table_args1[]={
	{"getSuper",(void*)&ABCVm::getSuper},
	{"setSuper",(void*)&ABCVm::setSuper},
	{"newObject",(void*)&ABCVm::newObject},
	{"getDescendants",(void*)&ABCVm::getDescendants},
	{"deleteProperty",(void*)&ABCVm::deleteProperty},
	{"jump",(void*)&ABCVm::jump},
	{"getLocal",(void*)&ABCVm::getLocal},
	{"setLocal",(void*)&ABCVm::setLocal},
	{"call",(void*)&ABCVm::call},
	{"coerce",(void*)&ABCVm::coerce},
	{"getLex",(void*)&ABCVm::getLex},
	{"findPropStrict",(void*)&ABCVm::findPropStrict},
	{"incLocal_i",(void*)&ABCVm::incLocal_i},
	{"findProperty",(void*)&ABCVm::findProperty},
	{"construct",(void*)&ABCVm::construct},
	{"constructSuper",(void*)&ABCVm::constructSuper},
	{"newArray",(void*)&ABCVm::newArray},
	{"newClass",(void*)&ABCVm::newClass},
	{"initProperty",(void*)&ABCVm::initProperty},
	{"kill",(void*)&ABCVm::kill}
};

opcode_handler ABCVm::opcode_table_args1_pointers_int[]={
	{"getSlot",(void*)&ABCVm::getSlot}
};

opcode_handler ABCVm::opcode_table_args1_pointers[]={
	{"convert_d",(void*)&ABCVm::convert_d},
	{"convert_b",(void*)&ABCVm::convert_b},
	{"convert_i",(void*)&ABCVm::convert_i},
	{"coerce_s",(void*)&ABCVm::coerce_s},
	{"checkfilter",(void*)&ABCVm::checkfilter},
	{"decrement_i",(void*)&ABCVm::decrement_i},
	{"negate",(void*)&ABCVm::negate},
	{"typeOf",(void*)ABCVm::typeOf}
};

opcode_handler ABCVm::opcode_table_args2_pointers_int[]={
	{"getMultiname",(void*)&ABCContext::s_getMultiname}
};

opcode_handler ABCVm::opcode_table_args_pointer_2int[]={
	{"getMultiname_i",(void*)&ABCContext::s_getMultiname_i}
};

opcode_handler ABCVm::opcode_table_args_pointer_number_int[]={
	{"getMultiname_d",(void*)&ABCContext::s_getMultiname_d}
};

opcode_handler ABCVm::opcode_table_args3_pointers[]={
	{"setProperty",(void*)&ABCVm::setProperty},
};

opcode_handler ABCVm::opcode_table_args2_pointers[]={
	{"nextName",(void*)&ABCVm::nextName},
	{"nextValue",(void*)&ABCVm::nextValue},
	{"lessThan",(void*)&ABCVm::lessThan},
	{"greaterThan",(void*)&ABCVm::greaterThan},
	{"greaterEquals",(void*)&ABCVm::greaterEquals},
	{"lessEquals",(void*)&ABCVm::lessEquals},
	{"strictEquals",(void*)&ABCVm::strictEquals},
};

typed_opcode_handler ABCVm::opcode_table_uintptr_t[]={
	{"bitAnd_oo",(void*)&ABCVm::bitAnd,ARGS_OBJ_OBJ},
	{"bitAnd_oi",(void*)&ABCVm::bitAnd_oi,ARGS_OBJ_INT},
	{"pushByte",(void*)&ABCVm::pushByte,ARGS_INT},
	{"pushShort",(void*)&ABCVm::pushShort,ARGS_INT},
	{"increment",(void*)&ABCVm::increment,ARGS_OBJ},
	{"increment_i",(void*)&ABCVm::increment_i,ARGS_OBJ},
	{"decrement",(void*)&ABCVm::decrement,ARGS_OBJ},
	{"bitNot",(void*)&ABCVm::bitNot,ARGS_OBJ},
	{"bitXor",(void*)&ABCVm::bitXor,ARGS_OBJ_OBJ},
	{"bitOr",(void*)&ABCVm::bitOr,ARGS_OBJ_OBJ},
	{"bitOr_oi",(void*)&ABCVm::bitOr_oi,ARGS_OBJ_INT},
	{"lShift",(void*)&ABCVm::lShift,ARGS_OBJ_OBJ},
	{"lShift_io",(void*)&ABCVm::lShift_io,ARGS_INT_OBJ},
	{"modulo",(void*)&ABCVm::modulo,ARGS_OBJ_OBJ},
	{"urShift",(void*)&ABCVm::urShift,ARGS_OBJ_OBJ},
	{"rShift",(void*)&ABCVm::rShift,ARGS_OBJ_OBJ},
	{"urShift_io",(void*)&ABCVm::urShift_io,ARGS_INT_OBJ},
	{"toInt",(void*)&ABCVm::s_toInt,ARGS_OBJ},
	{"getProperty_i",(void*)&ABCVm::getProperty_i,ARGS_OBJ_OBJ}
};

typed_opcode_handler ABCVm::opcode_table_number_t[]={
	{"multiply",(void*)&ABCVm::multiply,ARGS_OBJ_OBJ},
	{"multiply_oi",(void*)&ABCVm::multiply_oi,ARGS_OBJ_INT},
	{"divide",(void*)&ABCVm::divide,ARGS_OBJ_OBJ},
	{"subtract",(void*)&ABCVm::subtract,ARGS_OBJ_OBJ},
	{"subtract_oi",(void*)&ABCVm::subtract_oi,ARGS_OBJ_INT},
	{"subtract_io",(void*)&ABCVm::subtract_io,ARGS_INT_OBJ},
	{"subtract_do",(void*)&ABCVm::subtract_do,ARGS_NUMBER_OBJ}
};

typed_opcode_handler ABCVm::opcode_table_void[]={
	{"setSlot",(void*)&ABCVm::setSlot,ARGS_OBJ_OBJ_INT},
	{"debug_d",(void*)&debug_d,ARGS_NUMBER},
	{"debug_i",(void*)&debug_i,ARGS_INT},
	{"label",(void*)&ABCVm::label,ARGS_NONE},
	{"pop",(void*)&ABCVm::pop,ARGS_NONE},
	{"dup",(void*)&ABCVm::dup,ARGS_NONE},
	{"swap",(void*)&ABCVm::swap,ARGS_NONE},
	{"coerce_a",(void*)&ABCVm::coerce_a,ARGS_NONE},
	{"lookupswitch",(void*)&ABCVm::lookupswitch,ARGS_NONE},
};

typed_opcode_handler ABCVm::opcode_table_voidptr[]={
	{"add",(void*)&ABCVm::add,ARGS_OBJ_OBJ},
	{"add_oi",(void*)&ABCVm::add_oi,ARGS_OBJ_INT},
	{"add_od",(void*)&ABCVm::add_od,ARGS_OBJ_NUMBER},
	{"abstract_d",(void*)&abstract_d,ARGS_NUMBER},
	{"abstract_i",(void*)&abstract_i,ARGS_INT},
	{"abstract_b",(void*)&abstract_b,ARGS_BOOL},
	{"pushNaN",(void*)&ABCVm::pushNaN,ARGS_NONE},
	{"pushNull",(void*)&ABCVm::pushNull,ARGS_NONE},
	{"pushUndefined",(void*)&ABCVm::pushUndefined,ARGS_NONE},
	{"getProperty",(void*)&ABCVm::getProperty,ARGS_OBJ_OBJ}
};

typed_opcode_handler ABCVm::opcode_table_bool_t[]={
	{"not",(void*)&ABCVm::_not,ARGS_OBJ},
	{"equals",(void*)&ABCVm::equals,ARGS_OBJ_OBJ},
	{"ifEq",(void*)&ABCVm::ifEq,ARGS_OBJ_OBJ},
	{"ifNE",(void*)&ABCVm::ifNE,ARGS_OBJ_OBJ},
	{"ifNE_oi",(void*)&ABCVm::ifNE_oi,ARGS_OBJ_INT},
	{"ifLT",(void*)&ABCVm::ifLT,ARGS_OBJ_OBJ},
	{"ifNLT",(void*)&ABCVm::ifNLT,ARGS_OBJ_OBJ},
	{"ifLT_io",(void*)&ABCVm::ifLT_io,ARGS_INT_OBJ},
	{"ifLT_oi",(void*)&ABCVm::ifLT_oi,ARGS_OBJ_INT},
	{"ifGT",(void*)&ABCVm::ifGT,ARGS_OBJ_OBJ},
	{"ifNGT",(void*)&ABCVm::ifNGT,ARGS_OBJ_OBJ},
	{"ifGE",(void*)&ABCVm::ifGE,ARGS_OBJ_OBJ},
	{"ifNGE",(void*)&ABCVm::ifNGE,ARGS_OBJ_OBJ},
	{"ifLE",(void*)&ABCVm::ifLE,ARGS_OBJ_OBJ},
	{"ifNLE",(void*)&ABCVm::ifNLE,ARGS_OBJ_OBJ},
	{"in",(void*)&ABCVm::in,ARGS_OBJ_OBJ},
	{"ifStrictNE",(void*)&ABCVm::ifStrictNE,ARGS_OBJ_OBJ},
	{"ifStrictEq",(void*)&ABCVm::ifStrictEq,ARGS_OBJ_OBJ},
	{"pushTrue",(void*)&ABCVm::pushTrue,ARGS_NONE},
	{"pushFalse",(void*)&ABCVm::pushFalse,ARGS_NONE},
	{"isTypelate",(void*)&ABCVm::isTypelate,ARGS_OBJ_OBJ},
	{"ifTrue",(void*)&ABCVm::ifTrue,ARGS_OBJ},
	{"ifFalse",(void*)&ABCVm::ifFalse,ARGS_OBJ},
};

extern __thread SystemState* sys;
extern __thread ParseThread* pt;
extern __thread Manager* iManager;

using namespace std;

//Be careful, arguments nubering starts from 1
ASObject* argumentDumper(arguments* arg, uint32_t n)
{
	//Really implement default values, we now fill with Undefined
	if(n-1<arg->size())
	{
		arg->at(n-1)->incRef();
		return arg->at(n-1);
	}
	else
		return new Undefined;
}

ASObject* createRest()
{
	abort();
/*	Array* ret=new Array();
	ret->_constructor(ret,NULL);
	return ret;*/
}

llvm::Function* debug_i_f;

void ABCVm::registerFunctions()
{
	vector<const llvm::Type*> sig;
	llvm::FunctionType* FT=llvm::FunctionType::get(llvm::Type::getVoidTy(llvm_context), sig, false);
	llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"abort",module);

	const llvm::Type* ptr_type=ex->getTargetData()->getIntPtrType(llvm_context);
	const llvm::Type* int_type=ptr_type;
	const llvm::Type* voidptr_type=llvm::PointerType::getUnqual(ptr_type);
	const llvm::Type* number_type=llvm::Type::getDoubleTy(llvm_context);
	const llvm::Type* bool_type=llvm::IntegerType::get(llvm_context,1);
	const llvm::Type* void_type=llvm::Type::getVoidTy(llvm_context);

	sig.push_back(int_type);
	FT=llvm::FunctionType::get(void_type, sig, false);
	llvm::Function* F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"not_impl",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::not_impl);
	sig.clear();

	sig.push_back(ptr_type);
	FT=llvm::FunctionType::get(void_type, sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"debug",module);
	ex->addGlobalMapping(F,(void*)debug);
	sig.clear();

	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"createRest",module);
	ex->addGlobalMapping(F,(void*)createRest);
	sig.clear();

	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"argumentDumper",module);
	ex->addGlobalMapping(F,(void*)&argumentDumper);
	sig.clear();

	//All the opcodes needs a pointer to the context
	std::vector<const llvm::Type*> struct_elems;
	struct_elems.push_back(llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(ptr_type)));
	struct_elems.push_back(llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(ptr_type)));
	struct_elems.push_back(int_type);
	llvm::Type* context_type=llvm::PointerType::getUnqual(llvm::StructType::get(llvm_context,struct_elems,true));

	//newActivation needs both method_info and the context
	sig.push_back(context_type);
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"newActivation",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::newActivation);
	sig.clear();

	// (method_info*)
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	FT=llvm::FunctionType::get(void_type, sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"decRef",module);
	ex->addGlobalMapping(F,(void*)&ASObject::s_decRef);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"incRef",module);
	ex->addGlobalMapping(F,(void*)&ASObject::s_incRef);

	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	FT=llvm::FunctionType::get(void_type, sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"decRef_safe",module);
	ex->addGlobalMapping(F,(void*)&ASObject::s_decRef_safe);
	sig.clear();

	// (call_context*)
	sig.push_back(context_type);
	FT=llvm::FunctionType::get(void_type, sig, false);

	int elems=sizeof(opcode_table_args0)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args0[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args0[i].addr);
	}
	//Lazy pushing
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args0_lazy)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args0_lazy[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args0_lazy[i].addr);
	}
	//End of lazy pushing

	// (call_context*,int)
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(void_type, sig, false);
	elems=sizeof(opcode_table_args1)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args1[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args1[i].addr);
	}
	//Lazy pushing
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args1_lazy)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args1_lazy[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args1_lazy[i].addr);
	}
	//End of lazy pushing

	// (call_context*,int,int)
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(void_type, sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"callPropVoid",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::callPropVoid);

	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"callSuper",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::callSuper);

	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"callSuperVoid",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::callSuperVoid);

	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"callProperty",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::callProperty);

	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"constructProp",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::constructProp);


	//Lazy pushing
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"hasNext2",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::hasNext2);
	//End of lazy pushing
	
	//Lazy pushing, no context, (ASObject*)
	sig.clear();
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args1_pointers)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args1_pointers[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args1_pointers[i].addr);
	}
	//End of lazy pushing

	//Lazy pushing, no context, (ASObject*, int)
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args1_pointers_int)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args1_pointers_int[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args1_pointers_int[i].addr);
	}
	//End of lazy pushing

	//Lazy pushing, no context, (ASObject*, ASObject*)
	sig.clear();
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args2_pointers)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args2_pointers[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args2_pointers[i].addr);
	}
	//End of lazy pushing

	//Lazy pushing, no context, (ASObject*, ASObject*, int)
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args2_pointers_int)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args2_pointers_int[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args2_pointers_int[i].addr);
	}
	//End of lazy pushing
	
	//Lazy pushing, no context, (ASObject*, uintptr_t, int)
	sig.clear();
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(int_type);
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args_pointer_2int)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args_pointer_2int[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args_pointer_2int[i].addr);
	}

	sig.clear();
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(number_type);
	sig.push_back(int_type);
	FT=llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
	elems=sizeof(opcode_table_args_pointer_number_int)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args_pointer_number_int[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args_pointer_number_int[i].addr);
	}
	//End of lazy pushing

	//Lazy pushing, no context, (ASObject*, ASObject*, void*)
	sig.clear();
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	FT=llvm::FunctionType::get(void_type, sig, false);
	elems=sizeof(opcode_table_args3_pointers)/sizeof(opcode_handler);
	for(int i=0;i<elems;i++)
	{
		F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,opcode_table_args3_pointers[i].name,module);
		ex->addGlobalMapping(F,opcode_table_args3_pointers[i].addr);
	}

	//Build the concrete interface
	sig[0]=int_type;
	FT=llvm::FunctionType::get(void_type, sig, false);
	F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,"setProperty_i",module);
	ex->addGlobalMapping(F,(void*)&ABCVm::setProperty_i);

	register_table(int_type,opcode_table_uintptr_t,sizeof(opcode_table_uintptr_t)/sizeof(typed_opcode_handler));
	register_table(number_type,opcode_table_number_t,sizeof(opcode_table_number_t)/sizeof(typed_opcode_handler));
	register_table(void_type,opcode_table_void,sizeof(opcode_table_void)/sizeof(typed_opcode_handler));
	register_table(voidptr_type,opcode_table_voidptr,sizeof(opcode_table_voidptr)/sizeof(typed_opcode_handler));
	register_table(bool_type,opcode_table_bool_t,sizeof(opcode_table_bool_t)/sizeof(typed_opcode_handler));
}

void ABCVm::register_table(const llvm::Type* ret_type,typed_opcode_handler* table, int table_len)
{
	const llvm::Type* int_type=ex->getTargetData()->getIntPtrType(llvm_context);
	const llvm::Type* voidptr_type=llvm::PointerType::getUnqual(int_type);
	const llvm::Type* number_type=llvm::Type::getDoubleTy(llvm_context);
	const llvm::Type* bool_type=llvm::IntegerType::get(llvm_context,1);
	llvm::FunctionType* FT;

	vector<const llvm::Type*> sig_obj_obj;
	sig_obj_obj.push_back(voidptr_type);
	sig_obj_obj.push_back(voidptr_type);

	vector<const llvm::Type*> sig_obj_int;
	sig_obj_int.push_back(voidptr_type);
	sig_obj_int.push_back(int_type);

	vector<const llvm::Type*> sig_int_obj;
	sig_int_obj.push_back(int_type);
	sig_int_obj.push_back(voidptr_type);

	vector<const llvm::Type*> sig_obj_number;
	sig_obj_number.push_back(voidptr_type);
	sig_obj_number.push_back(number_type);

	vector<const llvm::Type*> sig_bool;
	sig_bool.push_back(bool_type);

	vector<const llvm::Type*> sig_int;
	sig_int.push_back(int_type);

	vector<const llvm::Type*> sig_number;
	sig_number.push_back(number_type);

	vector<const llvm::Type*> sig_obj;
	sig_obj.push_back(voidptr_type);

	vector<const llvm::Type*> sig_none;

	vector<const llvm::Type*> sig_obj_obj_int;
	sig_obj_obj_int.push_back(voidptr_type);
	sig_obj_obj_int.push_back(voidptr_type);
	sig_obj_obj_int.push_back(int_type);

	vector<const llvm::Type*> sig_number_obj;
	sig_number_obj.push_back(number_type);
	sig_number_obj.push_back(voidptr_type);

	for(int i=0;i<table_len;i++)
	{
		if(table[i].type==ARGS_OBJ_OBJ)
			FT=llvm::FunctionType::get(ret_type, sig_obj_obj, false);
		else if(table[i].type==ARGS_NONE)
			FT=llvm::FunctionType::get(ret_type, sig_none, false);
		else if(table[i].type==ARGS_INT_OBJ)
			FT=llvm::FunctionType::get(ret_type, sig_int_obj, false);
		else if(table[i].type==ARGS_OBJ_INT)
			FT=llvm::FunctionType::get(ret_type, sig_obj_int, false);
		else if(table[i].type==ARGS_OBJ_NUMBER)
			FT=llvm::FunctionType::get(ret_type, sig_obj_number, false);
		else if(table[i].type==ARGS_INT)
			FT=llvm::FunctionType::get(ret_type, sig_int, false);
		else if(table[i].type==ARGS_NUMBER)
			FT=llvm::FunctionType::get(ret_type, sig_number, false);
		else if(table[i].type==ARGS_BOOL)
			FT=llvm::FunctionType::get(ret_type, sig_bool, false);
		else if(table[i].type==ARGS_OBJ)
			FT=llvm::FunctionType::get(ret_type, sig_obj, false);
		else if(table[i].type==ARGS_OBJ_OBJ_INT)
			FT=llvm::FunctionType::get(ret_type, sig_obj_obj_int, false);
		else if(table[i].type==ARGS_NUMBER_OBJ)
			FT=llvm::FunctionType::get(ret_type, sig_number_obj, false);
		llvm::Function* F=llvm::Function::Create(FT,llvm::Function::ExternalLinkage,table[i].name,module);
		ex->addGlobalMapping(F,table[i].addr);
	}
}

llvm::Value* method_info::llvm_stack_pop(llvm::IRBuilder<>& builder,llvm::Value* dynamic_stack,llvm::Value* dynamic_stack_index)
{
	//decrement stack index
	llvm::Value* index=builder.CreateLoad(dynamic_stack_index);
	llvm::Constant* constant = llvm::ConstantInt::get(llvm::IntegerType::get(context->vm->llvm_context,64), 1);
	llvm::Value* index2=builder.CreateSub(index,constant);
	builder.CreateStore(index2,dynamic_stack_index);

	llvm::Value* dest=builder.CreateGEP(dynamic_stack,index2);
	return builder.CreateLoad(dest);
}

llvm::Value* method_info::llvm_stack_peek(llvm::IRBuilder<>& builder,llvm::Value* dynamic_stack,llvm::Value* dynamic_stack_index)
{
	llvm::Value* index=builder.CreateLoad(dynamic_stack_index);
	llvm::Constant* constant = llvm::ConstantInt::get(llvm::IntegerType::get(context->vm->llvm_context,64), 1);
	llvm::Value* index2=builder.CreateSub(index,constant);
	llvm::Value* dest=builder.CreateGEP(dynamic_stack,index2);
	return builder.CreateLoad(dest);
}

void method_info::llvm_stack_push(llvm::ExecutionEngine* ex, llvm::IRBuilder<>& builder, llvm::Value* val,
		llvm::Value* dynamic_stack,llvm::Value* dynamic_stack_index)
{
	llvm::Value* index=builder.CreateLoad(dynamic_stack_index);
	llvm::Value* dest=builder.CreateGEP(dynamic_stack,index);
	builder.CreateStore(val,dest);

	//increment stack index
	llvm::Constant* constant = llvm::ConstantInt::get(llvm::IntegerType::get(context->vm->llvm_context,64), 1);
	llvm::Value* index2=builder.CreateAdd(index,constant);
	builder.CreateStore(index2,dynamic_stack_index);
}

inline stack_entry method_info::static_stack_pop(llvm::IRBuilder<>& builder, vector<stack_entry>& static_stack, 
		llvm::Value* dynamic_stack, llvm::Value* dynamic_stack_index) 
{
	//try to get the tail value from the static stack
	if(!static_stack.empty())
	{
		stack_entry ret=static_stack.back();
		static_stack.pop_back();
		return ret;
	}
	//try to pop the tail value of the dynamic stack
	return stack_entry(llvm_stack_pop(builder,dynamic_stack,dynamic_stack_index),STACK_OBJECT);
}

inline stack_entry method_info::static_stack_peek(llvm::IRBuilder<>& builder, vector<stack_entry>& static_stack, 
		llvm::Value* dynamic_stack, llvm::Value* dynamic_stack_index) 
{
	//try to get the tail value from the static stack
	if(!static_stack.empty())
		return static_stack.back();
	//try to load the tail value of the dynamic stack
	return stack_entry(llvm_stack_peek(builder,dynamic_stack,dynamic_stack_index),STACK_OBJECT);
}

inline void method_info::static_stack_push(vector<stack_entry>& static_stack, const stack_entry& e)
{
	static_stack.push_back(e);
}

void method_info::abstract_value(llvm::ExecutionEngine* ex, llvm::IRBuilder<>& builder, stack_entry& e)
{
	switch(e.second)
	{
		case STACK_OBJECT:
			break;
		case STACK_INT:
			e.first=builder.CreateCall(ex->FindFunctionNamed("abstract_i"),e.first);
			break;
		case STACK_NUMBER:
			e.first=builder.CreateCall(ex->FindFunctionNamed("abstract_d"),e.first);
			break;
		case STACK_BOOLEAN:
			e.first=builder.CreateCall(ex->FindFunctionNamed("abstract_b"),e.first);
			break;
		default:
			abort();
	}
	
}

inline void method_info::syncStacks(llvm::ExecutionEngine* ex,llvm::IRBuilder<>& builder, bool jitted,
		vector<stack_entry>& static_stack,llvm::Value* dynamic_stack, llvm::Value* dynamic_stack_index) 
{
//	if(jitted)
	{
		for(int i=0;i<static_stack.size();i++)
		{
			if(static_stack[i].second==STACK_OBJECT)
			{
			}
			else if(static_stack[i].second==STACK_INT)
				static_stack[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_i"),static_stack[i].first);
			else if(static_stack[i].second==STACK_NUMBER)
				static_stack[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_d"),static_stack[i].first);
			else if(static_stack[i].second==STACK_BOOLEAN)
				static_stack[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_b"),static_stack[i].first);
			llvm_stack_push(ex,builder,static_stack[i].first,dynamic_stack,dynamic_stack_index);
		}
		static_stack.clear();
	}
}

inline void method_info::syncLocals(llvm::ExecutionEngine* ex,llvm::IRBuilder<>& builder,
	const vector<stack_entry>& static_locals,llvm::Value* locals,const vector<STACK_TYPE>& expected,
	const block_info& dest_block)
{
	for(int i=0;i<static_locals.size();i++)
	{
		if(static_locals[i].second==STACK_NONE)
			continue;

		//Let's sync with the expected values...
		//TODO: find a way to propagate the new info
		if(static_locals[i].second!=expected[i])
		{
			abort();
	/*		if(expected[i]==STACK_NONE && static_locals[i].second==STACK_INT)
			{
				static_locals[i].second=STACK_OBJECT;
				static_locals[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_i"),static_locals[i].first);
			}
			else if(expected[i]==STACK_NONE && static_locals[i].second==STACK_NUMBER)
			{
				static_locals[i].second=STACK_OBJECT;
				static_locals[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_d"),static_locals[i].first);
			}
			else if(expected[i]==STACK_NONE && static_locals[i].second==STACK_BOOLEAN)
			{
				static_locals[i].second=STACK_OBJECT;
				static_locals[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_b"),static_locals[i].first);
			}
			else if(expected[i]==STACK_NONE && static_locals[i].second==STACK_OBJECT)
			{
	//				static_locals[i].second=STACK_OBJECT;
	//				static_locals[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_b"),static_locals[i].first);
			}
			else if(expected[i]==STACK_NONE)
			{
				cout << i << " not NONE but " << static_locals[i].second << endl;
				abort();
			}
			else if(expected[i]==STACK_OBJECT && static_locals[i].second==STACK_INT)
			{
				static_locals[i].second=STACK_OBJECT;
				static_locals[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_i"),static_locals[i].first);
			}
			else if(expected[i]==STACK_OBJECT && static_locals[i].second==STACK_NUMBER)
			{
				static_locals[i].second=STACK_OBJECT;
				static_locals[i].first=builder.CreateCall(ex->FindFunctionNamed("abstract_d"),static_locals[i].first);
			}
			else
			{
				cout << "Locals type not coerent with prediction" << endl;
				cout << expected[i] << endl;
				cout << static_locals[i].second << endl;
				abort();
			}*/
		}

		if(dest_block.locals_reset[i])
		{
			if(static_locals[i].second==STACK_OBJECT)
				builder.CreateCall(ex->FindFunctionNamed("decRef"), static_locals[i].first);
			continue;
		}

		if(static_locals[i].second==dest_block.locals_start[i])
			builder.CreateStore(static_locals[i].first,dest_block.locals_start_obj[i]);
		else
		{
			llvm::Value* constant = llvm::ConstantInt::get(llvm::IntegerType::get(context->vm->llvm_context,32), i);
			llvm::Value* t=builder.CreateGEP(locals,constant);
			llvm::Value* old=builder.CreateLoad(t);
			if(static_locals[i].second==STACK_OBJECT)
			{
				builder.CreateCall(ex->FindFunctionNamed("decRef"), old);
				builder.CreateStore(static_locals[i].first,t);
			}
			else if(static_locals[i].second==STACK_INT)
			{
				//decRef the previous contents
				builder.CreateCall(ex->FindFunctionNamed("decRef"), old);
				llvm::Value* v=builder.CreateCall(ex->FindFunctionNamed("abstract_i"),static_locals[i].first);
				builder.CreateStore(v,t);
			}
			else if(static_locals[i].second==STACK_NUMBER)
			{
				//decRef the previous contents
				builder.CreateCall(ex->FindFunctionNamed("decRef"), old);
				llvm::Value* v=builder.CreateCall(ex->FindFunctionNamed("abstract_d"),static_locals[i].first);
				builder.CreateStore(v,t);
			}
			else if(static_locals[i].second==STACK_BOOLEAN)
			{
				//decRef the previous contents
				builder.CreateCall(ex->FindFunctionNamed("decRef"), old);
				llvm::Value* v=builder.CreateCall(ex->FindFunctionNamed("abstract_b"),static_locals[i].first);
				builder.CreateStore(v,t);
			}
			else
				abort();
		}
	}
}

STACK_TYPE block_info::checkProactiveCasting(int local_ip,STACK_TYPE type)
{
	pair< map<int,STACK_TYPE>::iterator, bool> result=push_types.insert(make_pair(local_ip,type));
	return result.first->second;
}

llvm::FunctionType* method_info::synt_method_prototype(llvm::ExecutionEngine* ex)
{
	//whatever pointer is good
	const llvm::Type* ptr_type=ex->getTargetData()->getIntPtrType(context->vm->llvm_context);

	std::vector<const llvm::Type*> struct_elems;
	struct_elems.push_back(llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(ptr_type)));
	struct_elems.push_back(llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(ptr_type)));
	struct_elems.push_back(ptr_type);
	llvm::Type* context_type=llvm::PointerType::getUnqual(llvm::StructType::get(context->vm->llvm_context,struct_elems,true));

	//Initialize LLVM representation of method
	vector<const llvm::Type*> sig;
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(llvm::PointerType::getUnqual(ptr_type));
	sig.push_back(context_type);

	return llvm::FunctionType::get(llvm::PointerType::getUnqual(ptr_type), sig, false);
}

SyntheticFunction::synt_function method_info::synt_method()
{
	if(f)
		return f;

	string n="method";
	n+=context->getString(name).raw_buf();
	if(!body)
	{
		LOG(CALLS,"Method " << n << " should be intrinsic");;
		return NULL;
	}
	llvm::ExecutionEngine* ex=context->vm->ex;
	llvm::LLVMContext& llvm_context=context->vm->llvm_context;
	llvm::FunctionType* method_type=synt_method_prototype(ex);
	llvmf=llvm::Function::Create(method_type,llvm::Function::ExternalLinkage,n,context->vm->module);

	//The pointer size compatible int type will be useful
	//TODO: void*
	const llvm::Type* int_type=ex->getTargetData()->getIntPtrType(context->vm->llvm_context);
	const llvm::Type* voidptr_type=llvm::PointerType::getUnqual(int_type);
	const llvm::Type* number_type=llvm::Type::getDoubleTy(llvm_context);
	const llvm::Type* bool_type=llvm::IntegerType::get(llvm_context,1);

	llvm::BasicBlock *BB = llvm::BasicBlock::Create(llvm_context,"entry", llvmf);
	llvm::IRBuilder<> Builder(llvm_context);
	Builder.SetInsertPoint(BB);

	//We define a couple of variables that will be used a lot
	llvm::Constant* constant;
	llvm::Constant* constant2;
	llvm::Value* value;
	//let's give access to method data to llvm
	constant = llvm::ConstantInt::get(int_type, (uintptr_t)this);
	llvm::Value* th = llvm::ConstantExpr::getIntToPtr(constant, llvm::PointerType::getUnqual(int_type));

	llvm::Function::ArgumentListType::iterator it=llvmf->getArgumentList().begin();
	it++;
	it++;
	llvm::Value* context=it;

	//let's give access to local data storage
	value=Builder.CreateStructGEP(context,0);
	llvm::Value* locals=Builder.CreateLoad(value);

	//the stack is statically handled as much as possible to allow llvm optimizations
	//on branch and on interpreted/jitted code transition it is synchronized with the dynamic one
	vector<stack_entry> static_stack;
	static_stack.reserve(body->max_stack);
	//Get the pointer to the dynamic stack
	value=Builder.CreateStructGEP(context,1);
	llvm::Value* dynamic_stack=Builder.CreateLoad(value);
	//Get the index of the dynamic stack
	llvm::Value* dynamic_stack_index=Builder.CreateStructGEP(context,2);

/*	//Allocate a fast dynamic stack based on LLVM alloca instruction
	//This is used on branches
	vactor<llvm::Value*> fast_dynamic_stack(body->max_stack);
	for(int i=0;i<body->max_stack;i++)
		fast_dynamic_stack[i]=Builder.CreateAlloca(voidptr_type);
	//Allocate also a stack pointer
	llvm::Value* fast_dynamic_stack_index=*/

	//the scope stack is not accessible to llvm code

	//Creating a mapping between blocks and starting address
	//The current header block is ended
	llvm::BasicBlock *StartBB = llvm::BasicBlock::Create(llvm_context,"entry", llvmf);
	Builder.CreateBr(StartBB);
	//CHECK: maybe not needed
	Builder.SetInsertPoint(StartBB);

	map<int,block_info> blocks;
	blocks[-1].BB=StartBB;

	bool jitted=false;

/*	//We fill locals with function arguments
	//First argument is the 'this'
	constant = llvm::ConstantInt::get(int_type, 0);
	llvm::Value* t=Builder.CreateGEP(locals,constant);
	it=llvmf->getArgumentList().begin();
	llvm::Value* arg=it;
	Builder.CreateStore(arg,t);
	//Second argument is the arguments pointer
	it++;
	for(int i=0;i<param_count;i++)
	{
		constant = llvm::ConstantInt::get(int_type, i+1);
		t=Builder.CreateGEP(locals,constant);
		arg=Builder.CreateCall2(ex->FindFunctionNamed("argumentDumper"), it, constant);
		Builder.CreateStore(arg,t);
	}*/
	vector<stack_entry> static_locals;
	static_locals.resize(body->local_count,stack_entry(NULL,STACK_NONE));

	//Let's build a block for the real function code
	blocks[0].BB=llvm::BasicBlock::Create(llvm_context,"begin", llvmf);
	block_info* cur_block=NULL;

	u8 opcode;
	bool last_is_branch=true;
	map<int,block_info>::iterator bit=blocks.begin();
	for(bit;bit!=blocks.end();bit++)
	{
		block_info& cur=bit->second;
		cur.locals_start.resize(body->local_count,STACK_NONE);
		cur.locals_reset.resize(body->local_count,false);
		cur.locals_used.resize(body->local_count,false);
	}

	//We try to analyze the blocks first to find if locals can survive the jumps
	bool stop;
	while(1)
	{
		//This is initialized to true so that on first iteration the entry block is used
		last_is_branch=true;
		stringstream code(body->code);
		stop=true;
		cur_block=NULL;
		vector<pair<int, STACK_TYPE> > static_stack_types;
		int local_ip=0;

		while(1)
		{
			local_ip=code.tellg();
			code >> opcode;
			if(code.eof())
				break;
			//Check if we are expecting a new block start
			map<int,block_info>::iterator it=blocks.find(local_ip);
			if(it!=blocks.end())
			{
				if(cur_block)
				{
					it->second.preds.insert(cur_block);
					cur_block->seqs.insert(&it->second);
				}
				cur_block=&it->second;
				LOG(TRACE,"New block at " << local_ip);
				cur_block->locals=cur_block->locals_start;
				//A new block starts, the last instruction should have been a branch?
				last_is_branch=false;
				static_stack_types.clear();
			}
			else if(last_is_branch)
			{
				//TODO: Check this. It seems that there may be invalid code after
				//block end
				LOG(TRACE,"Ignoring at " << local_ip);
				continue;
			}
			switch(opcode)
			{
				case 0x03:
				{
					//throw
					static_stack_types.clear();
					break;
				}
				case 0x04:
				case 0x05:
				{
					//getsuper
					//setsuper
					u30 t;
					code >> t;
					static_stack_types.clear();
					break;
				}
				case 0x08:
				{
					//kill
					LOG(TRACE, "block analysis: kill" );
					u30 t;
					code >> t;

					cur_block->locals[t]=STACK_NONE;
					break;
				}
				case 0x09:
				{
					//label
					//Create a new block and insert it in the mapping
					LOG(TRACE, "block analysis: label" );
					int here=local_ip;

					if(blocks[here].BB==NULL)
					{
						blocks[here].BB=llvm::BasicBlock::Create(llvm_context,"label", llvmf);
						blocks[here].locals_start.resize(body->local_count,STACK_NONE);
						blocks[here].locals_reset.resize(body->local_count,false);
						blocks[here].locals_used.resize(body->local_count,false);
					}

					last_is_branch=false;

					blocks[here].preds.insert(cur_block);
					cur_block->seqs.insert(&blocks[here]);
					static_stack_types.clear();
					cur_block=&blocks[here];
					LOG(TRACE,"New block at " << local_ip);
					cur_block->locals=cur_block->locals_start;
					//A new block starts, the last instruction should have been a branch?
					break;
				}
				case 0x0c:
				case 0x0d:
				case 0x0e:
				case 0x0f:
				case 0x10:
				case 0x11:
				case 0x12:
				case 0x13:
				case 0x14:
				case 0x15:
				case 0x16:
				case 0x17:
				case 0x18:
				case 0x19:
				case 0x1a:
				{
					//ifnlt
					//ifnle
					//ifngt
					//ifnge
					//jump
					//iftrue
					//iffalse
					//ifeq
					//ifne
					//iflt
					//ifle
					//ifge
					//ifgt
					//ifstricteq
					//ifstrictne
					LOG(TRACE, "block analysis: branches" );
					//TODO: implement common data comparison
					last_is_branch=true;
					s24 t;
					code >> t;

					int here=code.tellg();
					int dest=here+t;
					//Create a block for the fallthrough code and insert in the mapping
					if(blocks[here].BB==NULL)
					{
						blocks[here].BB=llvm::BasicBlock::Create(llvm_context,"fall", llvmf);
						blocks[here].locals_start.resize(body->local_count,STACK_NONE);
						blocks[here].locals_reset.resize(body->local_count,false);
						blocks[here].locals_used.resize(body->local_count,false);
					}
					blocks[here].preds.insert(cur_block);
					cur_block->seqs.insert(&blocks[here]);

					//And for the branch destination, if they are not in the blocks mapping
					if(blocks[dest].BB==NULL)
					{
						blocks[dest].BB=llvm::BasicBlock::Create(llvm_context,"then", llvmf);
						blocks[dest].locals_start.resize(body->local_count,STACK_NONE);
						blocks[dest].locals_reset.resize(body->local_count,false);
						blocks[dest].locals_used.resize(body->local_count,false);
					}
					blocks[dest].preds.insert(cur_block);
					cur_block->seqs.insert(&blocks[dest]);
		
					static_stack_types.clear();
					break;
				}
				case 0x1b:
				{
					//lookupswitch
					LOG(TRACE, "synt lookupswitch" );
					last_is_branch=true;

					int here=int(code.tellg())-1; //Base for the jumps is the instruction itself for the switch
					s24 t;
					code >> t;
					int defaultdest=here+t;
					LOG(TRACE,"default " << int(t));
					u30 count;
					code >> count;
					LOG(TRACE,"count "<< int(count));
					vector<s24> offsets(count+1);
					for(int i=0;i<count+1;i++)
					{
						code >> offsets[i];
						LOG(TRACE,"Case " << i << " " << offsets[i]);
					}
					static_stack_types.clear();
					jitted=false;
					if(blocks[defaultdest].BB==NULL)
					{
						blocks[defaultdest].BB=llvm::BasicBlock::Create(llvm_context,"default", llvmf);
						blocks[defaultdest].locals_start.resize(body->local_count,STACK_NONE);
						blocks[defaultdest].locals_reset.resize(body->local_count,false);
						blocks[defaultdest].locals_used.resize(body->local_count,false);
					}
					blocks[defaultdest].preds.insert(cur_block);
					cur_block->seqs.insert(&blocks[defaultdest]);

					for(int i=0;i<offsets.size();i++)
					{
						int casedest=here+offsets[i];
						if(blocks[casedest].BB==NULL)
						{
							blocks[casedest].BB=llvm::BasicBlock::Create(llvm_context,"default", llvmf);
							blocks[casedest].locals_start.resize(body->local_count,STACK_NONE);
							blocks[casedest].locals_reset.resize(body->local_count,false);
							blocks[casedest].locals_used.resize(body->local_count,false);
						}
						blocks[casedest].preds.insert(cur_block);
						cur_block->seqs.insert(&blocks[casedest]);
					}

					break;
				}
				case 0x1c:
				{
					//pushwith
					static_stack_types.clear();
					break;
				}
				case 0x1d:
				{
					//popscope
					break;
				}
				case 0x1e:
				{
					//nextname
					if(!static_stack.empty())
						static_stack_types.pop_back();
					if(!static_stack.empty())
						static_stack_types.pop_back();

					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x20:
				case 0x21:
				case 0x28:
				{
					//pushnull
					//pushundefined
					//pushnan
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x23:
				{
					//nextvalue
					if(!static_stack.empty())
						static_stack_types.pop_back();
					if(!static_stack.empty())
						static_stack_types.pop_back();

					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x24:
				{
					//pushbyte
					int8_t t;
					code.read((char*)&t,1);
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0x25:
				{
					//pushshort
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0x26:
				case 0x27:
				{
					//pushtrue
					//pushfalse
					static_stack_types.push_back(make_pair(local_ip,STACK_BOOLEAN));
					cur_block->checkProactiveCasting(local_ip,STACK_BOOLEAN);
					break;
				}
				case 0x29:
				{
					//pop
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					break;
				}
				case 0x2a:
				{
					//dup
					pair <int, STACK_TYPE> val;
					if(!static_stack_types.empty())
						val=static_stack_types.back();
					else
						val=make_pair(local_ip,STACK_OBJECT);

					val.first=local_ip;
					static_stack_types.push_back(val);
					cur_block->checkProactiveCasting(local_ip,val.second);
					break;
				}
				case 0x2b:
				{
					//swap
					pair <int, STACK_TYPE> t1,t2;
					if(!static_stack_types.empty())
					{
						t1=static_stack_types.back();
						static_stack_types.pop_back();
					}
					else
						t1=make_pair(local_ip,STACK_OBJECT);

					if(!static_stack_types.empty())
					{
						t2=static_stack_types.back();
						static_stack_types.pop_back();
					}
					else
						t2=make_pair(local_ip,STACK_OBJECT);

					static_stack_types.push_back(t1);
					static_stack_types.push_back(t2);

					break;
				}
				case 0x2c:
				{
					//pushstring
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x2d:
				{
					//pushint
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0x2f:
				{
					//pushdouble
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_NUMBER));
					cur_block->checkProactiveCasting(local_ip,STACK_NUMBER);
					break;
				}
				case 0x30:
				{
					//pushscope
					static_stack_types.clear();
					break;
				}
				case 0x32:
				{
					//hasnext2
					u30 t;
					code >> t;
					cur_block->locals[t]=STACK_NONE;
					code >> t;
					cur_block->locals[t]=STACK_NONE;
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x40:
				{
					//newfunction
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x41:
				case 0x42:
				case 0x49:
				{
					//call
					//construct
					//constructsuper
					static_stack_types.clear();
					u30 t;
					code >> t;
					break;
				}
				case 0x45:
				case 0x46:
				case 0x4a:
				case 0x4e:
				case 0x4f:
				{
					//callsuper
					//callproperty
					//constructprop
					//callsupervoid
					//callpropvoid
					static_stack_types.clear();
					u30 t;
					code >> t;
					code >> t;
					break;
				}
				case 0x47:
				case 0x48:
				{
					//returnvoid
					//returnvalue
					last_is_branch=true;
					static_stack_types.clear();
					for(int i=0;i<body->local_count;i++)
					{
						if(cur_block->locals_used[i]==false)
							cur_block->locals_reset[i]=true;
					}
					break;
				}
				case 0x55:
				case 0x56:
				case 0x58:
				case 0x59:
				case 0x5d:
				case 0x5e:
				case 0x60:
				{
					//newobject
					//newarray
					//newclass
					//getdescendants
					//findpropstrict
					//getlex
					//findproperty
					static_stack_types.clear();
					u30 t;
					code >> t;
					break;
				}
				case 0x57:
				{
					//newactivation
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x5a:
				{
					//newcatch
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x61:
				{
					//setproperty
					u30 t;
					code >> t;
					int rtdata=this->context->getMultinameRTData(t);
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(rtdata==1)
					{
						if(!static_stack_types.empty())
							static_stack_types.pop_back();
					}
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					break;
				}
				case 0x62:
				{
					//getlocal
					u30 i;
					code >> i;
					cur_block->locals_used[i]=true;
					if(cur_block->locals[i]==STACK_NONE)
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
						cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
						cur_block->locals[i]=STACK_OBJECT;
					}
					else
					{
						static_stack_types.push_back(make_pair(local_ip,cur_block->locals[i]));
						cur_block->checkProactiveCasting(local_ip,cur_block->locals[i]);
					}
					break;
				}
				case 0x63:
				{
					//setlocal
					LOG(TRACE, "synt setlocal" );
					u30 i;
					code >> i;
					cur_block->locals_used[i]=true;
					STACK_TYPE t;
					if(!static_stack_types.empty())
					{
						t=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t=STACK_OBJECT;
					
					if(cur_block->locals[i]==STACK_NONE)
						cur_block->locals_reset[i]=true;
					cur_block->locals[i]=t;
					break;
				}
				case 0x64:
				{
					//getglobalscope
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x65:
				{
					//getscopeobject
					u30 t;
					code >> t;
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x66:
				{
					//getproperty
					u30 t;
					code >> t;
					constant = llvm::ConstantInt::get(int_type, t);
					int rtdata=this->context->getMultinameRTData(t);
					if(rtdata==1)
					{
						if(!static_stack_types.empty())
							static_stack_types.pop_back();
					}
					if(!static_stack_types.empty())
						static_stack_types.pop_back();

					STACK_TYPE actual_type=cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					if(actual_type==STACK_OBJECT)
						static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					else if(actual_type==STACK_INT)
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					else if(actual_type==STACK_BOOLEAN)
						static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					else
					{
						cout << cur_block->push_types[local_ip] << endl;
						abort();
					}
					break;
				}
				case 0x68:
				case 0x6a:
				{
					//initproperty
					//deleteproperty
					static_stack_types.clear();
					u30 t;
					code >> t;
					break;
				}
				case 0x6c:
				{
					//getslot
					u30 t;
					code >> t;
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x6d:
				{
					//setslot
					u30 t;
					code >> t;
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					break;
				}
				case 0x73:
				case 0x74:
				case 0x75:
				case 0x76:
				{
					//convert_i
					//convert_u
					//convert_d
					//convert_b
					break;
				}
				case 0x78:
				{
					//checkfilter
					break;
				}
				case 0x80:
				{
					//coerce
					static_stack_types.clear();
					u30 t;
					code >> t;
					break;
				}
				case 0x82:
				case 0x85:
				{
					//coerce_a
					//coerce_s
					break;
				}
				case 0x87:
				{
					//astypelate
					static_stack_types.clear();
					break;
				}
				case 0x90:
				case 0x95:
				{
					//negate
					//typeof
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0x91:
				case 0x93:
				case 0xc0:
				{
					//increment
					//decrement
					//increment_i
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0x96:
				{
					//not
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_BOOLEAN));
					cur_block->checkProactiveCasting(local_ip,STACK_BOOLEAN);
					break;
				}
				case 0x97:
				{
					//bitnot
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xa0:
				{
					//add
					STACK_TYPE t1,t2;
					if(!static_stack_types.empty())
					{
						t1=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t1=STACK_OBJECT;
					if(!static_stack_types.empty())
					{
						t2=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t2=STACK_OBJECT;

					if(t1==STACK_OBJECT || t2==STACK_OBJECT)
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
						cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					}
					else if(t1==STACK_INT && t2==STACK_INT)
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
						cur_block->checkProactiveCasting(local_ip,STACK_INT);
					}
					else if(t1==STACK_NUMBER || t2==STACK_NUMBER)
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_NUMBER));
						cur_block->checkProactiveCasting(local_ip,STACK_NUMBER);
					}
					else
						abort();

					break;
				}
				case 0xa1:
				{
					//subtract
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_NUMBER));
					cur_block->checkProactiveCasting(local_ip,STACK_NUMBER);
					break;
				}
				case 0xa2:
				{
					//multiply
					STACK_TYPE t1,t2;
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						t1=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t1=STACK_OBJECT;
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						t2=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t2=STACK_OBJECT;

					if(t1==STACK_INT && t2==STACK_INT)
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
						cur_block->checkProactiveCasting(local_ip,STACK_INT);
					}
					else
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_NUMBER));
						cur_block->checkProactiveCasting(local_ip,STACK_NUMBER);
					}
					break;
				}
				case 0xa3:
				{
					//divide
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_NUMBER));
					cur_block->checkProactiveCasting(local_ip,STACK_NUMBER);
					break;
				}
				case 0xa4:
				{
					//modulo
					STACK_TYPE t1,t2;
					if(!static_stack_types.empty())
					{
						t2=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t2=STACK_OBJECT;
					if(!static_stack_types.empty())
					{
						t1=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t1=STACK_OBJECT;

					if(t1==STACK_OBJECT || t2==STACK_OBJECT)
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					else if(t1==STACK_INT && t2==STACK_NUMBER)
					{
						exit(2);
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					}
					else if(t1==STACK_NUMBER && t2==STACK_INT)
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					else if(t1==STACK_INT && t2==STACK_INT)
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					else if(t1==STACK_NUMBER && t2==STACK_NUMBER)
						static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					else
						abort();
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xa5:
				{
					//lshift
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xa6:
				{
					//rshift
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xa7:
				{
					//urshift
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xa8:
				{
					//bitand
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					if(!static_stack_types.empty())
					{
						cur_block->push_types[static_stack_types.back().first]=STACK_INT;
						static_stack_types.pop_back();
					}
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xa9:
				{
					//bitor
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xaa:
				{
					//bitxor
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_INT));
					cur_block->checkProactiveCasting(local_ip,STACK_INT);
					break;
				}
				case 0xab:
				{
					//equals
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_BOOLEAN));
					cur_block->checkProactiveCasting(local_ip,STACK_BOOLEAN);
					break;
				}
				case 0xac:
				{
					//strictequals
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0xad:
				{
					//lessthan
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0xae:
				{
					//lessequals
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0xaf:
				{
					//greaterthan
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0xb0:
				{
					//greaterequals
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0xb3:
				{
					//istypelate
					static_stack_types.clear();
					break;
				}
				case 0xb4:
				{
					//in
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_BOOLEAN));
					cur_block->checkProactiveCasting(local_ip,STACK_BOOLEAN);
					break;
				}
				case 0xc1:
				{
					//decrement_i
					if(!static_stack_types.empty())
						static_stack_types.pop_back();
					static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
					cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
					break;
				}
				case 0xc2:
				{
					//inclocal_i
					static_stack_types.clear();
					u30 t;
					code >> t;
					break;
				}
				case 0xd0:
				case 0xd1:
				case 0xd2:
				case 0xd3:
				{
					//getlocal_n
					int i=opcode&3;
					cur_block->locals_used[i]=true;
					if(cur_block->locals[i]==STACK_NONE)
					{
						static_stack_types.push_back(make_pair(local_ip,STACK_OBJECT));
						cur_block->checkProactiveCasting(local_ip,STACK_OBJECT);
						cur_block->locals[i]=STACK_OBJECT;
					}
					else
					{
						static_stack_types.push_back(make_pair(local_ip,cur_block->locals[i]));
						cur_block->checkProactiveCasting(local_ip,cur_block->locals[i]);
					}
					break;
				}
				case 0xd5:
				case 0xd6:
				case 0xd7:
				{
					//setlocal_n
					int i=opcode&3;
					LOG(TRACE, "synt setlocal " << i);
					cur_block->locals_used[i]=true;
					STACK_TYPE t;
					if(!static_stack_types.empty())
					{
						t=static_stack_types.back().second;
						static_stack_types.pop_back();
					}
					else
						t=STACK_OBJECT;

					if(cur_block->locals[i]==STACK_NONE)
						cur_block->locals_reset[i]=true;
					cur_block->locals[i]=t;
					break;
				}
				case 0xef:
				{
					//debug
					uint8_t debug_type;
					u30 index;
					uint8_t reg;
					u30 extra;
					code.read((char*)&debug_type,1);
					code >> index;
					code.read((char*)&reg,1);
					code >> extra;
					break;
				}
				case 0xf0:
				case 0xf1:
				{
					//debugline
					//debugfile
					u30 t;
					code >> t;
					break;
				}
				default:
					LOG(ERROR,"Not implemented instruction @" << code.tellg());
					u8 a,b,c;
					code >> a >> b >> c;
					LOG(ERROR,"dump " << hex << (unsigned int)opcode << ' ' << (unsigned int)a << ' ' 
							<< (unsigned int)b << ' ' << (unsigned int)c);
			}
		}

		//Let's propagate locals reset information
		while(1)
		{
			stop=true;
			map<int,block_info>::iterator bit=blocks.begin();
			for(bit;bit!=blocks.end();bit++)
			{
				block_info& cur=bit->second;
				std::vector<bool> new_reset(body->local_count,false);
				for(int i=0;i<body->local_count;i++)
				{
					if(cur.locals_used[i]==false)
						new_reset[i]=true;
				}
				set<block_info*>::iterator seq=cur.seqs.begin();
				for(seq;seq!=cur.seqs.end();seq++)
				{
					for(int i=0;i<body->local_count;i++)
					{
						if((*seq)->locals_reset[i]==false)
							new_reset[i]=false;
					}
				}
				for(int i=0;i<body->local_count;i++)
				{
					if(cur.locals_reset[i]==true)
						new_reset[i]=true;
				}
				if(new_reset!=cur.locals_reset)
				{
					stop=false;
					cur.locals_reset=new_reset;
				}
			}

			if(stop)
				break;
		}

		//We can now search for locals that can be saved
		//If every predecessor blocks agree with the type of a local we pass it over
		map<int,block_info>::iterator bit=blocks.begin();
		for(bit;bit!=blocks.end();bit++)
		{
			block_info& cur=bit->second;
			vector<STACK_TYPE> new_start;
			new_start.resize(body->local_count,STACK_NONE);
			if(!cur.preds.empty())
			{
				set<block_info*>::iterator pred=cur.preds.begin();
				new_start=(*pred)->locals;
				pred++;
				for(pred;pred!=cur.preds.end();pred++)
				{
					for(int j=0;j<(*pred)->locals.size();j++)
					{
						if(new_start[j]!=(*pred)->locals[j])
							new_start[j]=STACK_NONE;
					}
				}
			}
			//It's not useful to sync variables that are going to be resetted
			for(int i=0;i<body->local_count;i++)
			{
				if(cur.locals_reset[i])
					new_start[i]=STACK_NONE;
			}

			if(new_start!=cur.locals_start)
			{
			/*	for(int i=0;i<new_start.size();i++)
					cout << new_start[i] << ' ';
				cout << endl;
				for(int i=0;i<cur.locals.size();i++)
					cout << cur.locals_start[i] << ' ';
				cout << endl;*/
				stop=false;
			//	cout << "again" << endl;
				cur.locals_start=new_start;
			}
		}

		if(stop)
			break;
	}

	bit=blocks.begin();
	for(bit;bit!=blocks.end();bit++)
	{
		block_info& cur=bit->second;
		cur.locals_start_obj.resize(cur.locals_start.size(),NULL);
		for(int i=0;i<cur.locals_start.size();i++)
		{
			switch(cur.locals_start[i])
			{
				case STACK_NONE:
					break;
				case STACK_OBJECT:
					cur.locals_start_obj[i]=Builder.CreateAlloca(voidptr_type);
					break;
				case STACK_INT:
					cur.locals_start_obj[i]=Builder.CreateAlloca(int_type);
					break;
				case STACK_NUMBER:
					cur.locals_start_obj[i]=Builder.CreateAlloca(number_type);
					break;
				case STACK_BOOLEAN:
					cur.locals_start_obj[i]=Builder.CreateAlloca(bool_type);
					break;
				default:
					abort();
			}
		}
	}

	//Let's get another stream
	//TODO: is there a way to reset the old one?
	stringstream code2(body->code);;
	last_is_branch=true;

	static_stack.clear();
	Builder.CreateBr(blocks[0].BB);

	int local_ip=0;
	//Each case block builds the correct parameters for the interpreter function and call it
	while(1)
	{
		local_ip=code2.tellg();
		code2 >> opcode;
		if(code2.eof())
			break;
		//Check if we are expecting a new block start
		map<int,block_info>::iterator it=blocks.find(local_ip);
		if(it!=blocks.end())
		{
			//A new block starts, the last instruction should have been a branch?
			if(!last_is_branch)
			{
				LOG(TRACE, "Last instruction before a new block was not a branch.");
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,it->second);
				jitted=false;
				Builder.CreateBr(it->second.BB);
			}
			LOG(TRACE,"Starting block at "<<local_ip);
			Builder.SetInsertPoint(it->second.BB);
			cur_block=&it->second;

			if(!cur_block->locals_start.empty())
			{
				//Generate prologue, LLVM should optimize register usage
				if(static_locals.size()!=cur_block->locals_start.size())
					abort();
				for(int i=0;i<static_locals.size();i++)
				{
					static_locals[i].second=cur_block->locals_start[i];
					if(cur_block->locals_start[i]!=STACK_NONE)
						static_locals[i].first=Builder.CreateLoad(cur_block->locals_start_obj[i]);
				}
			}

			last_is_branch=false;
		}
		else if(last_is_branch)
		{
			//TODO: Check this. It seems that there may be invalid code after
			//block end
			LOG(TRACE,"Ignoring at " << local_ip);
			continue;
			/*LOG(CALLS,"Inserting block at "<<int(code.tellg())-1);
			abort();
			llvm::BasicBlock* A=llvm::BasicBlock::Create("fall", m->f);
			Builder.CreateBr(A);
			Builder.SetInsertPoint(A);*/
		}

		switch(opcode)
		{
			case 0x03:
			{
				//throw
				LOG(TRACE, "synt throw" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				//syncLocals(ex,Builder,static_locals,locals,cur_block->locals,*cur_block);
				jitted=false;
				Builder.CreateCall(ex->FindFunctionNamed("throw"), context);
				break;
			}
			case 0x04:
			{
				//getsuper
				LOG(TRACE, "synt getsuper" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("getSuper"), context, constant);
				break;
			}
			case 0x05:
			{
				//setsuper
				LOG(TRACE, "synt setsuper" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("setSuper"), context, constant);
				break;
			}
			case 0x08:
			{
				//kill
				u30 t;
				code2 >> t;
				LOG(TRACE, "synt kill " << t );
				if(Log::getLevel()==CALLS)
				{
					constant = llvm::ConstantInt::get(int_type, t);
					Builder.CreateCall2(ex->FindFunctionNamed("kill"), context, constant);
				}
				int i=t;
				if(static_locals[i].second==STACK_OBJECT)
					Builder.CreateCall(ex->FindFunctionNamed("decRef"), static_locals[i].first);

				static_locals[i].second=STACK_NONE;

				jitted=true;
				break;
			}
			case 0x09:
			{
				//label
				//Create a new block and insert it in the mapping
				LOG(TRACE, "synt label" );
				if(Log::getLevel()>=CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("label"));
				break;
			}
			case 0x0c:
			{
				//ifnlt
				LOG(TRACE, "synt ifnlt" );
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				llvm::Value* cond;
				if(v1.second==STACK_INT && v2.second==STACK_INT)
					cond=Builder.CreateICmpSGE(v2.first,v1.first); //GE == NLT
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNLT"), v1.first, v2.first);
				}

				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x0d:
			{
				//ifnle
				LOG(TRACE, "synt ifnle");
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;
				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				llvm::Value* cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNLE"), v1.first, v2.first);
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x0e:
			{
				//ifngt
				LOG(TRACE, "synt ifngt" );
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				llvm::Value* cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNGT"), v1.first, v2.first);
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x0f:
			{
				//ifnge
				LOG(TRACE, "synt ifnge");
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;
				//Make comparision

				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				llvm::Value* cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNGE"), v1.first, v2.first);
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x10:
			{
				//jump
				last_is_branch=true;

				s24 t;
				code2 >> t;
				LOG(TRACE, "synt jump " << t );
				if(Log::getLevel()==CALLS)
				{
					constant = llvm::ConstantInt::get(int_type, t);
					Builder.CreateCall2(ex->FindFunctionNamed("jump"), context, constant);
				}
				int here=code2.tellg();
				int dest=here+t;
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x11:
			{
				//iftrue
				LOG(TRACE, "synt iftrue" );
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				if(v1.second==STACK_BOOLEAN)
					cond=v1.first;
				else
					cond=Builder.CreateCall(ex->FindFunctionNamed("ifTrue"), v1.first);
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x12:
			{
				//iffalse

				last_is_branch=true;
				s24 t;
				code2 >> t;
				LOG(TRACE, "synt iffalse " << t );

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				if(v1.second==STACK_BOOLEAN)
					cond=Builder.CreateNot(v1.first);
				else
				{
					abstract_value(ex,Builder,v1);
					cond=Builder.CreateCall(ex->FindFunctionNamed("ifFalse"), v1.first);
				}
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x13:
			{
				//ifeq
				LOG(TRACE, "synt ifeq" );
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;
			
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				//Make comparision
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifEq"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v1.first);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifEq"), v1.first, v2.first);
				}
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
				{
					v2.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v2.first);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifEq"), v1.first, v2.first);
				}
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					cond=Builder.CreateFCmpOEQ(v1.first,v2.first);
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifEq"), v1.first, v2.first);
				}
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x14:
			{
				//ifne
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;
				LOG(TRACE, "synt ifne " << t );
			
				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNE_oi"), v2.first, v1.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNE_oi"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					cond=Builder.CreateFCmpONE(v1.first,v2.first);
				}
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
				{
					cond=Builder.CreateICmpNE(v1.first,v2.first);
				}
				else
				{
					//Abstract default
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifNE"), v1.first, v2.first);
				}
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x15:
			{
				//iflt
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;
				LOG(TRACE, "synt iflt "<< t );

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifLT"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifLT_io"), v1.first, v2.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifLT_oi"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
					cond=Builder.CreateICmpSLT(v2.first,v1.first);
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifLT"), v1.first, v2.first);
				}
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x16:
			{
				//ifle
				LOG(TRACE, "synt ifle" );
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifLE"), v1.first, v2.first);
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifLE"), v1.first, v2.first);
				}
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x17:
			{
				//ifgt
				LOG(TRACE, "synt ifgt" );
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				cond=Builder.CreateCall2(ex->FindFunctionNamed("ifGT"), v1.first, v2.first);
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x18:
			{
				//ifge
				LOG(TRACE, "synt ifge");
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				cond=Builder.CreateCall2(ex->FindFunctionNamed("ifGE"), v1.first, v2.first);
			
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x19:
			{
				//ifstricteq
				LOG(TRACE, "synt ifstricteq" );
				//TODO: implement common data comparison
				last_is_branch=true;
				s24 t;
				code2 >> t;
			
				//Make comparision
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				llvm::Value* v2=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				llvm::Value* cond=Builder.CreateCall2(ex->FindFunctionNamed("ifStrictEq"), v1, v2);

				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x1a:
			{
				//ifstrictne
				LOG(TRACE, "synt ifstrictne" );
				last_is_branch=true;
				s24 t;
				code2 >> t;

				//Make comparision
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* cond;
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifStrictNE"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v1.first);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifStrictNE"), v1.first, v2.first);
				}
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
				{
					v2.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v2.first);
					cond=Builder.CreateCall2(ex->FindFunctionNamed("ifStrictNE"), v1.first, v2.first);
				}
				else
					abort();

				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				int here=code2.tellg();
				int dest=here+t;
				llvm::BasicBlock* A=llvm::BasicBlock::Create(llvm_context,"epilogueA", llvmf);
				llvm::BasicBlock* B=llvm::BasicBlock::Create(llvm_context,"epilogueB", llvmf);
				Builder.CreateCondBr(cond,B,A);
				Builder.SetInsertPoint(A);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[here]);
				Builder.CreateBr(blocks[here].BB);

				Builder.SetInsertPoint(B);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[dest]);
				Builder.CreateBr(blocks[dest].BB);
				break;
			}
			case 0x1b:
			{
				//lookupswitch
				LOG(TRACE, "synt lookupswitch" );
				if(Log::getLevel()>=CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("lookupswitch"));
				last_is_branch=true;

				int here=int(code2.tellg())-1; //Base for the jumps is the instruction itself for the switch
				s24 t;
				code2 >> t;
				int defaultdest=here+t;
				LOG(TRACE,"default " << int(t));
				u30 count;
				code2 >> count;
				LOG(TRACE,"count "<< int(count));
				vector<s24> offsets(count+1);
				for(int i=0;i<count+1;i++)
				{
					code2 >> offsets[i];
					LOG(TRACE,"Case " << i << " " << offsets[i]);
				}
				stack_entry e=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(e.second==STACK_INT);
				else if(e.second==STACK_OBJECT)
				{
					e.first=Builder.CreateCall(ex->FindFunctionNamed("toInt"),e.first);
				}
				else
					abort();
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;

				//Generate epilogue for default dest
				llvm::BasicBlock* Default=llvm::BasicBlock::Create(llvm_context,"epilogueDest", llvmf);

				llvm::SwitchInst* sw=Builder.CreateSwitch(e.first,Default);
				Builder.SetInsertPoint(Default);
				syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[defaultdest]);
				Builder.CreateBr(blocks[defaultdest].BB);

				for(int i=0;i<offsets.size();i++)
				{
					int casedest=here+offsets[i];
					llvm::ConstantInt* constant = static_cast<llvm::ConstantInt*>(llvm::ConstantInt::get(int_type, i));
					llvm::BasicBlock* Case=llvm::BasicBlock::Create(llvm_context,"epilogueCase", llvmf);
					sw->addCase(constant,Case);
					Builder.SetInsertPoint(Case);
					syncLocals(ex,Builder,static_locals,locals,cur_block->locals,blocks[casedest]);
					Builder.CreateBr(blocks[casedest].BB);
				}

				break;
			}
			case 0x1c:
			{
				//pushwith
				LOG(TRACE, "synt pushwith" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				Builder.CreateCall(ex->FindFunctionNamed("pushWith"), context);
				break;
			}
			case 0x1d:
			{
				//popscope
				LOG(TRACE, "synt popscope" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				Builder.CreateCall(ex->FindFunctionNamed("popScope"), context);
				break;
			}
			case 0x1e:
			{
				//nextname
				LOG(TRACE, "synt nextname" );
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				llvm::Value* v2=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;

				value=Builder.CreateCall2(ex->FindFunctionNamed("nextName"), v1, v2);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x20:
			{
				//pushnull
				LOG(TRACE, "synt pushnull" );
				value=Builder.CreateCall(ex->FindFunctionNamed("pushNull"));
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x21:
			{
				//pushundefined
				LOG(TRACE, "synt pushundefined" );
				value=Builder.CreateCall(ex->FindFunctionNamed("pushUndefined"));
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x23:
			{
				//nextvalue
				LOG(TRACE, "synt nextvalue" );
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				llvm::Value* v2=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;

				value=Builder.CreateCall2(ex->FindFunctionNamed("nextValue"), v1, v2);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x24:
			{
				//pushbyte
				LOG(TRACE, "synt pushbyte" );
				int8_t t;
				code2.read((char*)&t,1);
				constant = llvm::ConstantInt::get(int_type, (int)t);
				static_stack_push(static_stack,stack_entry(constant,STACK_INT));
				if(Log::getLevel()==CALLS)
					value=Builder.CreateCall(ex->FindFunctionNamed("pushByte"), constant);
				jitted=true;
				break;
			}
			case 0x25:
			{
				//pushshort
				LOG(TRACE, "synt pushshort" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				static_stack_push(static_stack,stack_entry(constant,STACK_INT));
				if(Log::getLevel()==CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("pushShort"), constant);
				jitted=true;
				break;
			}
			case 0x26:
			{
				//pushtrue
				LOG(TRACE, "synt pushtrue" );
				value=Builder.CreateCall(ex->FindFunctionNamed("pushTrue"));
				static_stack_push(static_stack,stack_entry(value,STACK_BOOLEAN));
				jitted=true;
				break;
			}
			case 0x27:
			{
				//pushfalse
				LOG(TRACE, "synt pushfalse" );
				value=Builder.CreateCall(ex->FindFunctionNamed("pushFalse"));
				static_stack_push(static_stack,stack_entry(value,STACK_BOOLEAN));
				jitted=true;
				break;
			}
			case 0x28:
			{
				//pushnan
				LOG(TRACE, "synt pushnan" );
				value=Builder.CreateCall(ex->FindFunctionNamed("pushNaN"));
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x29:
			{
				//pop
				LOG(TRACE, "synt pop" );
				if(Log::getLevel()==CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("pop"));
				stack_entry e=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(e.second==STACK_OBJECT)
					Builder.CreateCall(ex->FindFunctionNamed("decRef"), e.first);
				jitted=true;
				break;
			}
			case 0x2a:
			{
				//dup
				LOG(TRACE, "synt dup" );
				if(Log::getLevel()==CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("dup"));
				stack_entry e=static_stack_peek(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				static_stack_push(static_stack,e);

				if(e.second==STACK_OBJECT)
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), e.first);
				jitted=true;
				break;
			}
			case 0x2b:
			{
				//swap
				LOG(TRACE, "synt swap" );
				if(Log::getLevel()==CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("swap"));
				stack_entry e1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry e2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				static_stack_push(static_stack,e1);
				static_stack_push(static_stack,e2);
				jitted=true;
				break;
			}
			case 0x2c:
			{
				//pushstring
				LOG(TRACE, "synt pushstring" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				value=Builder.CreateCall2(ex->FindFunctionNamed("pushString"), context, constant);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x2d:
			{
				//pushint
				LOG(TRACE, "synt pushint" );
				u30 t;
				code2 >> t;
				if(Log::getLevel()==CALLS)
				{
					constant = llvm::ConstantInt::get(int_type, t);
					Builder.CreateCall2(ex->FindFunctionNamed("pushInt"), context, constant);
				}
				s32 i=this->context->constant_pool.integer[t];
				constant = llvm::ConstantInt::get(int_type, i);
				static_stack_push(static_stack,stack_entry(constant,STACK_INT));
				jitted=true;
				break;
			}
			case 0x2f:
			{
				//pushdouble
				LOG(TRACE, "synt pushdouble" );
				u30 t;
				code2 >> t;

				if(Log::getLevel()==CALLS)
				{
					constant = llvm::ConstantInt::get(int_type, t);
					Builder.CreateCall2(ex->FindFunctionNamed("pushDouble"), context, constant);
				}
				number_t d=this->context->constant_pool.doubles[t];
				constant = llvm::ConstantFP::get(number_type,d);
				static_stack_push(static_stack,stack_entry(constant,STACK_NUMBER));
				jitted=true;
				break;
			}
			case 0x30:
			{
				//pushscope
				LOG(TRACE, "synt pushscope" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				Builder.CreateCall(ex->FindFunctionNamed("pushScope"), context);
				break;
			}
			case 0x32:
			{
				//hasnext2
				LOG(TRACE, "synt hasnext2" );
				u30 t,t2;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				code2 >> t2;
				constant2 = llvm::ConstantInt::get(int_type, t2);
				//Sync the locals to memory
				if(static_locals[t].second!=STACK_NONE)
				{
					llvm::Value* gep=Builder.CreateGEP(locals,constant);
					llvm::Value* old=Builder.CreateLoad(gep);
					Builder.CreateCall(ex->FindFunctionNamed("decRef"), old);
					abstract_value(ex,Builder,static_locals[t]);
					Builder.CreateStore(static_locals[t].first,gep);
					static_locals[t].second=STACK_NONE;
				}

				if(static_locals[t2].second!=STACK_NONE)
				{
					llvm::Value* gep=Builder.CreateGEP(locals,constant2);
					llvm::Value* old=Builder.CreateLoad(gep);
					Builder.CreateCall(ex->FindFunctionNamed("decRef"), old);
					abstract_value(ex,Builder,static_locals[t2]);
					Builder.CreateStore(static_locals[t2].first,gep);
					static_locals[t2].second=STACK_NONE;
				}

				value=Builder.CreateCall3(ex->FindFunctionNamed("hasNext2"), context, constant, constant2);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x40:
			{
				//newfunction
				LOG(TRACE, "synt newfunction" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				value=Builder.CreateCall2(ex->FindFunctionNamed("newFunction"), context, constant);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x41:
			{
				//call
				LOG(TRACE, "synt call" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("call"), context, constant);
				break;
			}
			case 0x42:
			{
				//construct
				LOG(TRACE, "synt construct" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("construct"), context, constant);
				break;
			}
			case 0x45:
			{
				//callsuper
				LOG(TRACE, "synt callsuper" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				code2 >> t;
				constant2 = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall3(ex->FindFunctionNamed("callSuper"), context, constant, constant2);
				break;
			}
			case 0x46:
			{
				//callproperty
				//TODO: Implement static resolution where possible
				LOG(TRACE, "synt callproperty" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				code2 >> t;
				constant2 = llvm::ConstantInt::get(int_type, t);

	/*				//Pop the stack arguments
				vector<llvm::Value*> args(t+1);
				for(int i=0;i<t;i++)
					args[t-i]=static_stack_pop(Builder,static_stack,m).first;*/
				//Call the function resolver, static case could be resolved at this time (TODO)
				Builder.CreateCall3(ex->FindFunctionNamed("callProperty"), context, constant, constant2);
	/*				//Pop the function object, and then the object itself
				llvm::Value* fun=static_stack_pop(Builder,static_stack,m).first;

				llvm::Value* fun2=Builder.CreateBitCast(fun,synt_method_prototype(t));
				args[0]=static_stack_pop(Builder,static_stack,m).first;
				Builder.CreateCall(fun2,args.begin(),args.end());*/

				break;
			}
			case 0x47:
			{
				//returnvoid
				LOG(TRACE, "synt returnvoid" );
				last_is_branch=true;
				constant = llvm::ConstantInt::get(int_type, NULL);
				value = llvm::ConstantExpr::getIntToPtr(constant, llvm::PointerType::getUnqual(int_type));
				for(int i=0;i<static_locals.size();i++)
				{
					if(static_locals[i].second==STACK_OBJECT)
						Builder.CreateCall(ex->FindFunctionNamed("decRef"),static_locals[i].first);
					static_locals[i].second=STACK_NONE;
				}
				for(int i=0;i<static_stack.size();i++)
				{
					if(static_stack[i].second==STACK_OBJECT)
						Builder.CreateCall(ex->FindFunctionNamed("decRef"),static_stack[i].first);
				}
				Builder.CreateRet(value);
				break;
			}
			case 0x48:
			{
				//returnvalue
				//TODO: Should coerce the return type to the expected one
				LOG(TRACE, "synt returnvalue" );
				last_is_branch=true;
				stack_entry e=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(e.second==STACK_INT)
					e.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),e.first);
				else if(e.second==STACK_NUMBER)
					e.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_d"),e.first);
				else if(e.second==STACK_OBJECT)
				{
				}
				else if(e.second==STACK_BOOLEAN)
					e.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_b"),e.first);
				else
					abort();
				for(int i=0;i<static_locals.size();i++)
				{
					if(static_locals[i].second==STACK_OBJECT)
						Builder.CreateCall(ex->FindFunctionNamed("decRef"),static_locals[i].first);
					static_locals[i].second=STACK_NONE;
				}
				for(int i=0;i<static_stack.size();i++)
				{
					if(static_stack[i].second==STACK_OBJECT)
						Builder.CreateCall(ex->FindFunctionNamed("decRef"),static_stack[i].first);
				}
				Builder.CreateRet(e.first);
				break;
			}
			case 0x49:
			{
				//constructsuper
				LOG(TRACE, "synt constructsuper" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("constructSuper"), context, constant);
				break;
			}
			case 0x4a:
			{
				//constructprop
				LOG(TRACE, "synt constructprop" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				code2 >> t;
				constant2 = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall3(ex->FindFunctionNamed("constructProp"), context, constant, constant2);
				break;
			}
			case 0x4e:
			{
				//callsupervoid
				LOG(TRACE, "synt callsupervoid" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				code2 >> t;
				constant2 = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall3(ex->FindFunctionNamed("callSuperVoid"), context, constant, constant2);
				break;
			}
			case 0x4f:
			{
				//callpropvoid
				LOG(TRACE, "synt callpropvoid" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				code2 >> t;
				constant2 = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall3(ex->FindFunctionNamed("callPropVoid"), context, constant, constant2);
				break;
			}
			case 0x55:
			{
				//newobject
				LOG(TRACE, "synt newobject" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("newObject"), context, constant);
				break;
			}
			case 0x56:
			{
				//newarray
				LOG(TRACE, "synt newarray" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("newArray"), context, constant);
				break;
			}
			case 0x57:
			{
				//newactivation
				LOG(TRACE, "synt newactivation" );
				value=Builder.CreateCall2(ex->FindFunctionNamed("newActivation"), context, th);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x58:
			{
				//newclass
				LOG(TRACE, "synt newclass" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("newClass"), context, constant);
				break;
			}
			case 0x59:
			{
				//getdescendants
				LOG(TRACE, "synt getdescendants" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("getDescendants"), context, constant);
				break;
			}
			case 0x5a:
			{
				//newcatch
				LOG(TRACE, "synt newcatch" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				value=Builder.CreateCall2(ex->FindFunctionNamed("newCatch"), context, constant);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x5d:
			{
				//findpropstrict
				LOG(TRACE, "synt findpropstrict" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("findPropStrict"), context, constant);
				break;
			}
			case 0x5e:
			{
				//findproperty
				LOG(TRACE, "synt findproperty" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("findProperty"), context, constant);
				break;
			}
			case 0x60:
			{
				//getlex
				LOG(TRACE, "synt getlex" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("getLex"), context, constant);
				break;
			}
			case 0x61:
			{
				//setproperty
				LOG(TRACE, "synt setproperty" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				int rtdata=this->context->getMultinameRTData(t);
				stack_entry value=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				llvm::Value* name;
				//HACK: we need to reinterpret the pointer to the generic type
				llvm::Value* reint_context=Builder.CreateBitCast(context,voidptr_type);
				if(rtdata==0)
				{
					//We pass a dummy second context param
					name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname"), reint_context, reint_context, constant);
				}
				else if(rtdata==1)
				{
					stack_entry rt1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

					if(rt1.second==STACK_INT)
						name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname_i"), reint_context, rt1.first, constant);
					else if(rt1.second==STACK_NUMBER)
						name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname_d"), reint_context, rt1.first, constant);
					else if(rt1.second==STACK_OBJECT)
						name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname"), reint_context, rt1.first, constant);
					else
						abort();
				}
				stack_entry obj=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(value.second==STACK_INT)
					Builder.CreateCall3(ex->FindFunctionNamed("setProperty_i"),value.first, obj.first, name);
				else if(value.second==STACK_NUMBER)
				{
					value.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_d"),value.first);
					Builder.CreateCall3(ex->FindFunctionNamed("setProperty"),value.first, obj.first, name);
				}
				else if(value.second==STACK_BOOLEAN)
				{
					value.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_b"),value.first);
					Builder.CreateCall3(ex->FindFunctionNamed("setProperty"),value.first, obj.first, name);
				}
				else
					Builder.CreateCall3(ex->FindFunctionNamed("setProperty"),value.first, obj.first, name);
				jitted=true;
				break;
			}
			case 0x62:
			{
				//getlocal
				LOG(TRACE, "synt getlocal" );
				u30 i;
				code2 >> i;
				constant = llvm::ConstantInt::get(int_type, i);
				if(Log::getLevel()==CALLS)
					Builder.CreateCall2(ex->FindFunctionNamed("getLocal"), context, constant);

				if(static_locals[i].second==STACK_NONE)
				{
					llvm::Value* t=Builder.CreateGEP(locals,constant);
					t=Builder.CreateLoad(t,"stack");
					static_stack_push(static_stack,stack_entry(t,STACK_OBJECT));
					static_locals[i]=stack_entry(t,STACK_OBJECT);
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), t);
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), t);
				}
				else if(static_locals[i].second==STACK_OBJECT)
				{
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), static_locals[i].first);
					static_stack_push(static_stack,static_locals[i]);
				}
				else if(static_locals[i].second==STACK_INT || static_locals[i].second==STACK_NUMBER || static_locals[i].second==STACK_BOOLEAN)
					static_stack_push(static_stack,static_locals[i]);
				else
					abort();

				jitted=true;
				break;
			}
			case 0x63:
			{
				//setlocal
				u30 i;
				code2 >> i;
				LOG(TRACE, "synt setlocal " << i);
				stack_entry e=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(static_locals[i].second==STACK_OBJECT)
					Builder.CreateCall(ex->FindFunctionNamed("decRef"), static_locals[i].first);

				static_locals[i]=e;
				if(Log::getLevel()==CALLS)
				{
					constant = llvm::ConstantInt::get(int_type, i);
					Builder.CreateCall2(ex->FindFunctionNamed("setLocal"), context, constant);
				}
				jitted=true;
				break;
			}
			case 0x64:
			{
				//getglobalscope
				LOG(TRACE, "synt getglobalscope" );
				value=Builder.CreateCall(ex->FindFunctionNamed("getGlobalScope"), context);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x65:
			{
				//getscopeobject
				LOG(TRACE, "synt getscopeobject" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				value=Builder.CreateCall2(ex->FindFunctionNamed("getScopeObject"), context, constant);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x66:
			{
				//getproperty
				LOG(TRACE, "synt getproperty" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				int rtdata=this->context->getMultinameRTData(t);
				llvm::Value* name;
				//HACK: we need to reinterpret the pointer to the generic type
				llvm::Value* reint_context=Builder.CreateBitCast(context,voidptr_type);
				if(rtdata==0)
				{
					//We pass a dummy second context param
					name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname"), reint_context, reint_context, constant);
				}
				else if(rtdata==1)
				{
					stack_entry rt1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

					if(rt1.second==STACK_INT)
						name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname_i"), reint_context, rt1.first, constant);
					else if(rt1.second==STACK_NUMBER)
						name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname_d"), reint_context, rt1.first, constant);
					else if(rt1.second==STACK_OBJECT)
						name = Builder.CreateCall3(ex->FindFunctionNamed("getMultiname"), reint_context, rt1.first, constant);
					else
						abort();
				}
				stack_entry obj=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				if(cur_block->push_types[local_ip]==STACK_OBJECT ||
					cur_block->push_types[local_ip]==STACK_BOOLEAN)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("getProperty"), obj.first, name);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(cur_block->push_types[local_ip]==STACK_INT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("getProperty_i"), obj.first, name);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else
					abort();
				jitted=true;
				break;
			}
			case 0x68:
			{
				//initproperty
				LOG(TRACE, "synt initproperty" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("initProperty"), context, constant);
				break;
			}
			case 0x6a:
			{
				//deleteproperty
				LOG(TRACE, "synt deleteproperty" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("deleteProperty"), context, constant);
				break;
			}
			case 0x6c:
			{
				//getslot
				LOG(TRACE, "synt getslot" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall2(ex->FindFunctionNamed("getSlot"), v1, constant);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x6d:
			{
				//setslot
				LOG(TRACE, "synt setslot" );
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
				{
				}
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v1.first);
				else if(v1.second==STACK_NUMBER && v2.second==STACK_OBJECT)
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_d"),v1.first);
				else if(v1.second==STACK_BOOLEAN && v2.second==STACK_OBJECT)
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_b"),v1.first);
				else
					abort();

				Builder.CreateCall3(ex->FindFunctionNamed("setSlot"), v1.first, v2.first, constant);
				jitted=true;
				break;
			}
			case 0x73:
			{
				//convert_i
				LOG(TRACE, "synt convert_i" );
				/*llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall(ex->FindFunctionNamed("convert_i"), v1);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));*/
				jitted=true;
				break;
			}
			case 0x74:
			{
				//convert_u
				LOG(TRACE, "synt convert_u" );
				/*llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall(ex->FindFunctionNamed("convert_i"), v1);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));*/
				jitted=true;
				break;
			}
			case 0x75:
			{
				//convert_d
				LOG(TRACE, "synt convert_d" );
				/*llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall(ex->FindFunctionNamed("convert_d"), v1);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));*/
				jitted=true;
				break;
			}
			case 0x76:
			{
				//convert_b
				LOG(TRACE, "synt convert_b" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT)
				{
					value=Builder.CreateCall(ex->FindFunctionNamed("convert_b"), v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(v1.second==STACK_BOOLEAN)
					static_stack_push(static_stack,v1);
				else
					abort();
				jitted=true;
				break;
			}
			case 0x78:
			{
				//checkfilter
				LOG(TRACE, "synt checkfilter" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				abstract_value(ex,Builder,v1);
				value=Builder.CreateCall(ex->FindFunctionNamed("checkfilter"), v1.first);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x80:
			{
				//coerce
				LOG(TRACE, "synt coerce" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("coerce"), context, constant);
				break;
			}
			case 0x82:
			{
				//coerce_a
				LOG(TRACE, "synt coerce_a" );
				if(Log::getLevel()==CALLS)
					Builder.CreateCall(ex->FindFunctionNamed("coerce_a"));
				/*stack_entry v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				else
					value=v1.first;
				
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));*/
				jitted=true;
				break;
			}
			case 0x85:
			{
				//coerce_s
				LOG(TRACE, "synt coerce_s" );
				/*llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall(ex->FindFunctionNamed("coerce_s"), v1);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));*/
				jitted=true;
				break;
			}
			case 0x87:
			{
				//astypelate
				LOG(TRACE, "synt astypelate" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				Builder.CreateCall(ex->FindFunctionNamed("asTypelate"), context);
				break;
			}
			case 0x90:
			{
				//negate
				LOG(TRACE, "synt negate" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				abstract_value(ex,Builder,v1);
				value=Builder.CreateCall(ex->FindFunctionNamed("negate"), v1.first);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x91:
			{
				//increment
				LOG(TRACE, "synt increment" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT)
					value=Builder.CreateCall(ex->FindFunctionNamed("increment"), v1.first);
				else if(v1.second==STACK_INT)
				{
					constant = llvm::ConstantInt::get(int_type, 1);
					value=Builder.CreateAdd(v1.first,constant);
				}
				else
					abort();
				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0x93:
			{
				//decrement
				LOG(TRACE, "synt decrement" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT)
				{
					constant = llvm::ConstantInt::get(int_type, 1);
					value=Builder.CreateSub(v1.first,constant);
				}
				else
				{
					abstract_value(ex,Builder,v1);
					value=Builder.CreateCall(ex->FindFunctionNamed("decrement"), v1.first);
				}
				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0x95:
			{
				//typeof
				LOG(TRACE, "synt typeof" );
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall(ex->FindFunctionNamed("typeOf"), v1);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0x96:
			{
				//not
				LOG(TRACE, "synt not" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT)
					value=Builder.CreateCall(ex->FindFunctionNamed("not"), v1.first);
				else if(v1.second==STACK_BOOLEAN)
					value=Builder.CreateNot(v1.first);
				else
					abort();
				static_stack_push(static_stack,stack_entry(value,STACK_BOOLEAN));
				jitted=true;
				break;
			}
			case 0x97:
			{
				//bitnot
				LOG(TRACE, "synt bitnot" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT)
					value=Builder.CreateCall(ex->FindFunctionNamed("bitNot"), v1.first);
				else
					abort();
				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xa0:
			{
				//add
				LOG(TRACE, "synt add" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("add"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("add_oi"), v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("add_oi"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(v1.second==STACK_OBJECT && v2.second==STACK_NUMBER)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("add_od"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("add_od"), v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
				{
					value=Builder.CreateAdd(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					//TODO: 32bit check
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					value=Builder.CreateAdd(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_INT)
				{
					//TODO: 32bit check
					v2.first=Builder.CreateSIToFP(v2.first,number_type);
					value=Builder.CreateAdd(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_NUMBER)
				{
					//TODO: 32bit check
					value=Builder.CreateAdd(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else
					abort();

				jitted=true;
				break;
			}
			case 0xa1:
			{
				//subtract
				LOG(TRACE, "synt subtract" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("subtract"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
				{
					value=Builder.CreateSub(v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_NUMBER)
				{
					value=Builder.CreateSub(v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					value=Builder.CreateSub(v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_INT)
				{
					v2.first=Builder.CreateSIToFP(v2.first,number_type);
					value=Builder.CreateSub(v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("subtract_oi"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("subtract_do"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("subtract_io"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else
					abort();

				jitted=true;
				break;
			}
			case 0xa2:
			{
				//multiply
				LOG(TRACE, "synt multiply" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("multiply_oi"), v2.first, v1.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
				{
					value=Builder.CreateCall2(ex->FindFunctionNamed("multiply_oi"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
				{
					value=Builder.CreateMul(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_INT)
				{
					v2.first=Builder.CreateSIToFP(v2.first,number_type);
					value=Builder.CreateMul(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					value=Builder.CreateMul(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("multiply"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}

				jitted=true;
				break;
			}
			case 0xa3:
			{
				//divide
				LOG(TRACE, "synt divide" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=	static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					value=Builder.CreateFDiv(v2.first,v1.first);
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_INT)
				{
					v2.first=Builder.CreateSIToFP(v2.first,number_type);
					value=Builder.CreateFDiv(v2.first,v1.first);
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_NUMBER)
				{
					value=Builder.CreateFDiv(v2.first,v1.first);
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("divide"), v1.first, v2.first);
				}
				static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				jitted=true;
				break;
			}
			case 0xa4:
			{
				//modulo
				LOG(TRACE, "synt modulo" );
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					abort();
					v1.first=Builder.CreateSIToFP(v1.first,number_type);
					value=Builder.CreateCall2(ex->FindFunctionNamed("modulo"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_NUMBER));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_INT)
				{
					v1.first=Builder.CreateFPToSI(v1.first,int_type);
					value=Builder.CreateSRem(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_NUMBER)
				{
					v1.first=Builder.CreateFPToSI(v1.first,int_type);
					v2.first=Builder.CreateFPToSI(v2.first,int_type);
					value=Builder.CreateSRem(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
				{
					value=Builder.CreateSRem(v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("modulo"), v1.first, v2.first);
					static_stack_push(static_stack,stack_entry(value,STACK_INT));
				}
				jitted=true;
				break;
			}
			case 0xa5:
			{
				//lshift
				LOG(TRACE, "synt lshift" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("lShift_io"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
					value=Builder.CreateShl(v2.first,v1.first); //Check for trucation of v1.first
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("lShift"), v1.first, v2.first);
				}

				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xa6:
			{
				//rshift
				LOG(TRACE, "synt rshift" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v1.first);
					value=Builder.CreateCall2(ex->FindFunctionNamed("rShift"), v1.first, v2.first);
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("lShift"), v1.first, v2.first);
				}

				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xa7:
			{
				//urshift
				LOG(TRACE, "synt urshift" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("urShift"), v1.first, v2.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
					exit(43);
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("urShift_io"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
					value=Builder.CreateLShr(v2.first,v1.first); //Check for trucation of v1.first
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v2.first=Builder.CreateFPToSI(v2.first,int_type);
					value=Builder.CreateLShr(v2.first,v1.first); //Check for trucation of v1.first
				}
				else if(v1.second==STACK_NUMBER && v2.second==STACK_OBJECT)
				{
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_d"),v1.first);
					value=Builder.CreateCall2(ex->FindFunctionNamed("urShift"), v1.first, v2.first);
				}
				else
					abort();

				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xa8:
			{
				//bitand
				LOG(TRACE, "synt bitand" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitAnd_oi"), v2.first, v1.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitAnd_oi"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
					value=Builder.CreateAnd(v1.first,v2.first);
				else if(v1.second==STACK_NUMBER && v2.second==STACK_INT)
				{
					v1.first=Builder.CreateFPToUI(v1.first,int_type);
					value=Builder.CreateAnd(v1.first,v2.first);
				}
				else if(v1.second==STACK_INT && v2.second==STACK_NUMBER)
				{
					v2.first=Builder.CreateFPToUI(v2.first,int_type);
					value=Builder.CreateAnd(v1.first,v2.first);
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitAnd_oo"), v1.first, v2.first);
				}

				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xa9:
			{
				//bitor
				LOG(TRACE, "synt bitor" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT && v2.second==STACK_INT)
					value=Builder.CreateOr(v1.first,v2.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitOr_oi"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitOr_oi"), v2.first, v1.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitOr"), v1.first, v2.first);
				else
					abort();

				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xaa:
			{
				//bitxor
				LOG(TRACE, "synt bitxor" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT && v2.second==STACK_INT)
					value=Builder.CreateXor(v1.first,v2.first);
				else if(v1.second==STACK_OBJECT && v2.second==STACK_INT)
				{
					v2.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v2.first);
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitXor"), v1.first, v2.first);
				}
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("bitXor"), v1.first, v2.first);
				}

				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xab:
			{
				//equals
				LOG(TRACE, "synt equals" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT)
					value=Builder.CreateCall2(ex->FindFunctionNamed("equals"), v1.first, v2.first);
				else if(v1.second==STACK_INT && v2.second==STACK_OBJECT)
				{
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v1.first);
					value=Builder.CreateCall2(ex->FindFunctionNamed("equals"), v1.first, v2.first);
				}
				else if(v1.second==STACK_BOOLEAN && v2.second==STACK_OBJECT)
				{
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_b"),v1.first);
					value=Builder.CreateCall2(ex->FindFunctionNamed("equals"), v1.first, v2.first);
				}
				else if(v1.second==STACK_INT && v2.second==STACK_INT)
					value=Builder.CreateICmpEQ(v1.first,v2.first);
				else
				{
					abstract_value(ex,Builder,v1);
					abstract_value(ex,Builder,v2);
					value=Builder.CreateCall2(ex->FindFunctionNamed("equals"), v1.first, v2.first);
				}
				static_stack_push(static_stack,stack_entry(value,STACK_BOOLEAN));
				jitted=true;
				break;
			}
			case 0xac:
			{
				//strictequals
				LOG(TRACE, "synt strictequals" );
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				llvm::Value* v2=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall2(ex->FindFunctionNamed("strictEquals"), v1, v2);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0xad:
			{
				//lessthan
				LOG(TRACE, "synt lessthan" );
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);

				value=Builder.CreateCall2(ex->FindFunctionNamed("lessThan"), v1.first, v2.first);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0xae:
			{
				//lessequals
				LOG(TRACE, "synt lessequals" );
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				value=Builder.CreateCall2(ex->FindFunctionNamed("lessEquals"), v1.first, v2.first);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0xaf:
			{
				//greaterthan
				LOG(TRACE, "synt greaterthan" );
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);

				abstract_value(ex,Builder,v1);
				abstract_value(ex,Builder,v2);
				value=Builder.CreateCall2(ex->FindFunctionNamed("greaterThan"), v1.first, v2.first);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0xb0:
			{
				//greaterequals
				LOG(TRACE, "synt greaterequals" );
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_INT)
					v1.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v1.first);
				if(v2.second==STACK_INT)
					v2.first=Builder.CreateCall(ex->FindFunctionNamed("abstract_i"),v2.first);
				value=Builder.CreateCall2(ex->FindFunctionNamed("greaterEquals"), v1.first, v2.first);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0xb3:
			{
				//istypelate
				LOG(TRACE, "synt istypelate" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				stack_entry v2=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				assert(v1.second==STACK_OBJECT && v2.second==STACK_OBJECT);
				value=Builder.CreateCall2(ex->FindFunctionNamed("isTypelate"),v1.first,v2.first);
				static_stack_push(static_stack,stack_entry(value,STACK_BOOLEAN));
				jitted=false;
				break;
			}
			case 0xb4:
			{
				//in
				LOG(TRACE, "synt in" );
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				llvm::Value* v2=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall2(ex->FindFunctionNamed("in"), v1, v2);
				static_stack_push(static_stack,stack_entry(value,STACK_BOOLEAN));
				jitted=true;
				break;
			}
			case 0xc0:
			{
				//increment_i
				LOG(TRACE, "synt increment_i" );
				stack_entry v1=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				if(v1.second==STACK_OBJECT)
					value=Builder.CreateCall(ex->FindFunctionNamed("increment_i"), v1.first);
				else if(v1.second==STACK_INT)
				{
					constant = llvm::ConstantInt::get(int_type, 1);
					value=Builder.CreateAdd(v1.first,constant);
				}
				else
					abort();
				static_stack_push(static_stack,stack_entry(value,STACK_INT));
				jitted=true;
				break;
			}
			case 0xc1:
			{
				//decrement_i
				LOG(TRACE, "synt decrement_i" );
				llvm::Value* v1=
					static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index).first;
				value=Builder.CreateCall(ex->FindFunctionNamed("decrement_i"), v1);
				static_stack_push(static_stack,stack_entry(value,STACK_OBJECT));
				jitted=true;
				break;
			}
			case 0xc2:
			{
				//inclocal_i
				LOG(TRACE, "synt inclocal_i" );
				syncStacks(ex,Builder,jitted,static_stack,dynamic_stack,dynamic_stack_index);
				jitted=false;
				u30 t;
				code2 >> t;
				constant = llvm::ConstantInt::get(int_type, t);
				Builder.CreateCall2(ex->FindFunctionNamed("incLocal_i"), context, constant);
				break;
			}
			case 0xd0:
			case 0xd1:
			case 0xd2:
			case 0xd3:
			{
				//getlocal_n
				int i=opcode&3;
				LOG(TRACE, "synt getlocal_n " << i );
				constant = llvm::ConstantInt::get(int_type, i);
				if(Log::getLevel()==CALLS)
					Builder.CreateCall2(ex->FindFunctionNamed("getLocal"), context, constant);

				if(static_locals[i].second==STACK_NONE)
				{
					llvm::Value* t=Builder.CreateGEP(locals,constant);
					t=Builder.CreateLoad(t,"stack");
					static_stack_push(static_stack,stack_entry(t,STACK_OBJECT));
					static_locals[i]=stack_entry(t,STACK_OBJECT);
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), t);
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), t);
				}
				else if(static_locals[i].second==STACK_OBJECT)
				{
					Builder.CreateCall(ex->FindFunctionNamed("incRef"), static_locals[i].first);
					static_stack_push(static_stack,static_locals[i]);
				}
				else if(static_locals[i].second==STACK_INT || 
						static_locals[i].second==STACK_NUMBER ||
						static_locals[i].second==STACK_BOOLEAN)
				{
					static_stack_push(static_stack,static_locals[i]);
				}
				else
					abort();

				jitted=true;
				break;
			}
			case 0xd5:
			case 0xd6:
			case 0xd7:
			{
				//setlocal_n
				int i=opcode&3;
				LOG(TRACE, "synt setlocal_n " << i );
				stack_entry e=static_stack_pop(Builder,static_stack,dynamic_stack,dynamic_stack_index);
				//DecRef previous local
				if(static_locals[i].second==STACK_OBJECT)
					Builder.CreateCall(ex->FindFunctionNamed("decRef"), static_locals[i].first);

				static_locals[i]=e;
				if(Log::getLevel()==CALLS)
				{
					constant = llvm::ConstantInt::get(int_type, opcode&3);
					Builder.CreateCall2(ex->FindFunctionNamed("setLocal"), context, constant);
				}
				jitted=true;
				break;
			}
			case 0xef:
			{
				//debug
				LOG(TRACE, "synt debug" );
				uint8_t debug_type;
				u30 index;
				uint8_t reg;
				u30 extra;
				code2.read((char*)&debug_type,1);
				code2 >> index;
				code2.read((char*)&reg,1);
				code2 >> extra;
				break;
			}
			case 0xf0:
			{
				//debugline
				LOG(TRACE, "synt debugline" );
				u30 t;
				code2 >> t;
				break;
			}
			case 0xf1:
			{
				//debugfile
				LOG(TRACE, "synt debugfile" );
				u30 t;
				code2 >> t;
				break;
			}
			default:
				LOG(ERROR,"Not implemented instruction @" << code2.tellg());
				u8 a,b,c;
				code2 >> a >> b >> c;
				LOG(ERROR,"dump " << hex << (unsigned int)opcode << ' ' << (unsigned int)a << ' ' 
						<< (unsigned int)b << ' ' << (unsigned int)c);
				constant = llvm::ConstantInt::get(int_type, opcode);
				Builder.CreateCall(ex->FindFunctionNamed("not_impl"), constant);
				Builder.CreateRetVoid();

				f=(SyntheticFunction::synt_function)this->context->vm->ex->getPointerToFunction(llvmf);
				return f;
		}
	}

	map<int,block_info>::iterator it2=blocks.begin();
	for(it2;it2!=blocks.end();it2++)
	{
		if(it2->second.BB->getTerminator()==NULL)
		{
			cout << "start at " << it2->first << endl;
			abort();
		}
	}

	//llvmf->dump();
	this->context->vm->FPM->run(*llvmf);
	f=(SyntheticFunction::synt_function)this->context->vm->ex->getPointerToFunction(llvmf);
	//cerr << "Address " << reinterpret_cast<void*>(f) << endl;
	return f;
}