#include "object.h"
#include "errors.h"

template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};

ArrayContainer::ArrayContainer() = default;

ArrayContainer::ArrayContainer(const std::vector<Object*>& dimVec)
{
	if (dimVec.empty()) throw ArgumentError("At least 1 array size parameter expected.");
	size_t currSize = 0;
	if (const int* size = std::get_if<int>(&dimVec[0]->data))
	{
		if (*size > 0) currSize = static_cast<size_t>(*size);
		else throw ValueError("Array size parameter must be a positive integer.");
	}
	else throw TypeError("Array size parameter must be an integer.");
	vecPtr = new std::vector<std::unique_ptr<Object>>(currSize);
	if (dimVec.size() == 1)
	{
		for (size_t i = 0; i != currSize; i++)
		{
			const auto objPtr = new Object();
			objPtr->setLval(true);
			(*vecPtr)[i].reset(objPtr);
		}
	}
	else
	{
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
		throw ArgumentError("At least 1 array subscript expected.");
	size_t currIdx = 0;
	if (const int* idx = std::get_if<int>(&idxVec[0]->data))
	{
		if (*idx >= 0) currIdx = static_cast<size_t>(*idx);
		else throw ValueError("Array subscript must be a non-negative integer.");
	}
	else throw TypeError("Array subscript must be an integer.");

	if (currIdx >= vecPtr->size())
		throw RangeError("Array subscript out of range.");
	if (idxVec.size() == 1)
		return (*vecPtr)[currIdx].get(); // Convert std::unique_ptr to raw pointer

	return (*(*vecPtr)[currIdx])[std::vector<Object*>(idxVec.begin() + 1, idxVec.end())];
}

StringContainer::StringContainer() = default;

StringContainer::StringContainer(const std::string& str)
{
	vecPtr = new std::vector<std::unique_ptr<Object>>(str.length());
	for(size_t i = 0; i != str.length(); i++)
	{
		const auto objPtr = new Object(str[i]);
		objPtr->setLval(true);
		objPtr->setPersistentType(true);
		(*vecPtr)[i].reset(objPtr);
	}
}

Object* StringContainer::getChar(const std::vector<Object*>& idxVec) const
{
	if (idxVec.size() != 1)
		throw ArgumentError("Exactly 1 string subscript expected.");
	size_t currIdx = 0;
	if (const int* idx = std::get_if<int>(&idxVec[0]->data))
	{
		if (*idx >= 0) currIdx = static_cast<size_t>(*idx);
		else throw ValueError("String subscript must be a non-negative integer.");
	}
	else throw TypeError("String subscript must be an integer.");
	if (currIdx >= vecPtr->size())
		throw RangeError("String subscript out of range.");
	return (*vecPtr)[currIdx].get(); // Convert std::unique_ptr to raw pointer
}

std::string StringContainer::getStr() const
{
	std::string str;
	for(const auto& charObj : *vecPtr)
	{
		str.push_back(std::get<char>(charObj->data));
	}
	return str;
}


StackContainer::StackContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("Stack constructor does not take any arguments!");
	stackPtr = new std::stack<std::unique_ptr<Object>>;
}

Object* StackContainer::push(const std::vector<Object*>& argVec) const
{
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	stackPtr->emplace(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}

Object* StackContainer::pop(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	if (stackPtr->empty()) throw CustomError("Called pop() on empty stack.");
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
		throw ArgumentError("Number of arguments not equal to number of declared parameters.");

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
		           [&result](bool& val) { result = val; },
		           [&result](char& val) { result = static_cast<bool>(val); },
		           [&result](int& val) { result = static_cast<bool>(val); },
		           [&result](float& val) { result = static_cast<bool>(val); },
		           [](auto&)
		           {
		           }
	           }, data);
	return result;
}

std::string Object::toStr()
{
	std::string result;
	std::visit(overload{
		           [&result](bool& val) { result = (val) ? ("true") : ("false"); },
		           [&result](char& val) { result = std::string(1, val); },
		           [&result](int& val) { result = std::to_string(val); },
		           [&result](float& val) { result = std::to_string(val); },
		           [&result](StringContainer sc) { result = sc.getStr(); },
		           [](auto&) { throw TypeError("Object does not have a string representation"); }
	           }, data);
	return result;
}

bool Object::isPersistentType() const
{
	return persistentType;
}

void Object::setPersistentType(const bool isIt)
{
	persistentType = isIt;
}
