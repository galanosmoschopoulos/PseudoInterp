#include "object.h"
#include <iostream>
#include <stdexcept>
#include "operators.h"
template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};

template <class... Ts>
explicit overload(Ts ...) -> overload<Ts...>;

ArrayContainer::ArrayContainer() = default;

ArrayContainer::ArrayContainer(const std::vector<Object*>& dimVec)
{
	if (dimVec.empty())
		throw std::runtime_error("Subscript indices must be positive integers.");
	size_t currSize = 0;
	std::visit(overload{
		           [&currSize](int& size)
		           {
					   if (size > 0) currSize = size;
					   else throw std::runtime_error("Array size parameters must be positive integers.");
		           },
		           [](auto&) { throw std::runtime_error("Array size parameters must be positive integers."); }
	           }, dimVec[0]->data);

	vecPtr = new std::vector<std::unique_ptr<Object>>(currSize);
	if (dimVec.size() == 1)
	{
		for(size_t i = 0; i != currSize; i++)
		{
			const auto objPtr = new Object();
			objPtr->setLval(true);
			(*vecPtr)[i].reset(objPtr);
		}
	}
	else {
		for (size_t i = 0; i != currSize; i++)
		{
			const auto objPtr = new Object(ArrayContainer(std::vector<Object*>(dimVec.begin() + 1, dimVec.end())));
			objPtr->setLval(true);
			(*vecPtr)[i].reset(objPtr);
		}
	}
}

Object* ArrayContainer::getArray(const std::vector<Object*>& idxVec) const
{
	if (idxVec.empty())
		throw std::runtime_error("Subscript indices must be non-negative integers.");
	size_t currIdx = 0;
	std::visit(overload{
		           [&currIdx](int& idx)
		           {
					   if (idx >= 0) currIdx = idx;
					   else throw std::runtime_error("Subscript indices must be non-negative integers.");
		           },
		           [](auto&) { throw std::runtime_error("Subscript indices must be non-negative integers."); }
	           }, idxVec[0]->data);

	if (currIdx >= vecPtr->size())
		throw std::runtime_error("Array subscript out of range.");
	if (idxVec.size() == 1)
		return (*vecPtr)[currIdx].get(); // Convert std::shared_ptr to raw pointer
	else
		return (*(*vecPtr)[currIdx])[std::vector<Object*>(idxVec.begin()+1, idxVec.end())];
}

StackContainer::StackContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw std::runtime_error("Stack initializer does not take any arguments!");
	stackPtr = new std::stack<std::unique_ptr<Object>>;
}

Object* StackContainer::push(const std::vector<Object*>& argVec) const
{
	if (argVec.size() != 1) throw std::runtime_error("Wrong arguments.");
	stackPtr->emplace(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}

Object* StackContainer::pop(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw std::runtime_error("Wrong arguments.");
	if (stackPtr->empty()) throw std::runtime_error("Called pop() on empty stack.");
	const auto poppedObj = new Object(*stackPtr->top());
	stackPtr->pop();
	return poppedObj;
}

Function::Function(CodeBlock* block, std::vector<ASTNode*> params, const int level) : block(block), paramVec(
	                                                                                      std::move(params)), definedFuncLevel(level)
{
};

Function::Function() = default;

Object* Function::eval(Scope* scope, const std::vector<Object*>& argVec) const
{
	if (argVec.size() != paramVec.size())
		throw std::runtime_error("Wrong number of arguments");

	Scope* newScope = scope->getRestricted(definedFuncLevel);

	newScope->incLevel();
	newScope->incFuncLevel();

	Object* funcResult = nullptr;
	for (size_t i = 0; i != argVec.size(); i++)
	{
		*paramVec[i]->eval(newScope, true) = *argVec[i];
	}
	funcResult = block->eval(newScope, true);
	if (funcResult == nullptr) funcResult = new Object;

	newScope->decrLevel();
	newScope->decrFuncLevel();
	delete newScope;
	return funcResult;
}

Object::Object() = default;

ArrayContainer& Object::getArrayContainer()
{
	return std::get<ArrayContainer>(data);
}

bool Object::isLval() const
{
	return lval;
}

void Object::setLval(const bool isIt)
{
	lval = isIt;
}

bool Object::isTrue()
{
	bool result = false;
	std::visit(overload{
		[&result](bool& val) {result = val; },
		[&result](unsigned char& val) {result = static_cast<bool>(val); },
		[&result](int& val) {result = static_cast<bool>(val); },
		[&result](float& val) {result = static_cast<bool>(val); },
		[](auto&) {}
	}, data);
	return result;
}

std::string Object::toStr()
{
	std::string result;
	std::visit(overload{
		[&result](bool &val) {result = (val)?("true"):("false"); },
		[&result](unsigned char &val) {result = std::string(1, val); },
		[&result](int& val) {result = std::to_string(val); },
		[&result](float& val) {result = std::to_string(val); },
		[&result](std::string& i) {result = i; },
		[](auto&) {throw std::runtime_error("Object does not have a string representation"); }
		}, data);
	return result;
}

