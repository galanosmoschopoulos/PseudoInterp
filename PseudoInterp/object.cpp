#include "object.h"
#include <iostream>
#include <stdexcept>
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

	vecPtr = new vecOfPtrs(currSize);
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


ArrayConstructor::ArrayConstructor() = default;

Object* ArrayConstructor::eval(const std::vector<Object*>& argVec) const
{
	return new Object(ArrayContainer(argVec));
}

Object::Object() = default;

Object::Object(std::string val)
{
	currentType = ObjectType::STR;
	data = val;
}

Object::Object(int val)
{
	currentType = ObjectType::INT;
	data = val;
}

Object::Object(const Function& function)
{
	currentType = ObjectType::FUNC;
	data = function;
}

Object::Object(const ArrayContainer& array_container)
{
	currentType = ObjectType::ARR;
	data = array_container;
}

Object::Object(const ArrayConstructor& array_constructor)
{
	currentType = ObjectType::ARRAY_CONSTRUCTOR;
	data = array_constructor;
}

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


Object& Object::operator=(const Object& obj2)
{
	if (this == &obj2)
		return *this;
	this->currentType = obj2.currentType;
	this->data = obj2.data;
	return *this;
}

Object& Object::operator+=(Object& rhs)
{
	std::visit(overload{
		           [](int& left, int& right) { left += right; },
		           [](int& left, std::string& right) { std::to_string(left) += right; },
		           [](std::string& left, int& right) { left += std::to_string(right); },
		           [](std::string& left, std::string& right) { left += right; },
		           [](auto&, auto&) { throw std::runtime_error("+ operator: incompatible operand types"); }
	           }, this->data, rhs.data);
	return *this;
}

Object operator+(Object lhs, Object& rhs)
{
	lhs += rhs;
	lhs.setLval(false);
	return lhs;
}

Object& Object::operator-=(Object& rhs)
{
	std::visit(overload{
		           [](int& left, int& right) { left -= right; },
		           [](auto&, auto&) { throw std::runtime_error("- operator: incompatible operand types"); }
	           }, this->data, rhs.data);
	return *this;
}

Object operator-(Object lhs, Object& rhs)
{
	lhs -= rhs;
	lhs.setLval(false);
	return lhs;
}

Object& Object::operator*=(Object& rhs)
{
	std::visit(overload{
		           [](int& left, int& right) { left *= right; },
		           [](auto&, auto&) { throw std::runtime_error("* operator: incompatible operand types"); }
	           }, this->data, rhs.data);
	return *this;
}

Object operator*(Object lhs, Object& rhs)
{
	lhs *= rhs;
	lhs.setLval(false);
	return lhs;
}

Object& Object::operator/=(Object& rhs)
{
	std::visit(overload{
		           [](int& left, int& right) { left /= right; },
		           [](auto&, auto&) { throw std::runtime_error("/ operator: incompatible operand types"); }
	           }, this->data, rhs.data);
	return *this;
}

Object operator/(Object lhs, Object& rhs)
{
	lhs /= rhs;
	lhs.setLval(false);
	return lhs;
}

Object& Object::operator%=(Object& rhs)
{
	std::visit(overload{
		           [](int& left, int& right) { left %= right; },
		           [](auto&, auto&) { throw std::runtime_error("% operator: incompatible operand types"); }
	           }, this->data, rhs.data);
	return *this;
}

Object operator%(Object lhs, Object& rhs)
{
	lhs %= rhs;
	lhs.setLval(false);
	return lhs;
}

Object& Object::operator++()
{
	std::visit(overload{
		           [](int& x) { ++x; },
		           [](auto&) { throw std::runtime_error("++ operator: incompatible operand types"); }
	           }, this->data);
	return *this;
}

Object& Object::operator--()
{
	std::visit(overload{
		           [](int& x) { --x; },
		           [](auto&) { throw std::runtime_error("-- operator: incompatible operand types"); }
	           }, this->data);
	return *this;
}

Object Object::operator++(int)
{
	Object old = *this;
	operator++();
	old.setLval(false);
	return old;
}

Object Object::operator--(int)
{
	Object old = *this;
	operator--();
	return old;
}

Object Object::operator-() const
{
	Object tmp = *this;
	std::visit(overload{
		           [](int& x) { x *= -1; },
		           [](auto&) { throw std::runtime_error("unary - operator: incompatible operand types"); }
	           }, tmp.data);
	tmp.setLval(false);
	return tmp;
}

Object Object::operator+() const
{
	Object tmp = *this;
	std::visit(overload{
		           [](int& x) { x *= +1; },
		           [](auto&) { throw std::runtime_error("unary + operator: incompatible operand types"); }
	           }, tmp.data);
	tmp.setLval(false);
	return tmp;
}

Object operator<(Object& lhs, Object& rhs)
{
	int result = 0;
	std::visit(overload{
		           [&result](int& left, int& right) { result = left < right; },
		           [&result](std::string& left, std::string& right) { result = left < right; },
		           [](auto&, auto&) { throw std::runtime_error("comparison operator: incompatible operand types"); }
	           }, lhs.data, rhs.data);
	return Object(result);
}

Object operator>(Object& lhs, Object& rhs)
{
	return rhs < lhs;
}

Object operator<=(Object& lhs, Object& rhs)
{
	return !(lhs > rhs);
}

Object operator>=(Object& lhs, Object& rhs)
{
	return !(lhs < rhs);
}

Object operator==(Object& lhs, Object& rhs)
{
	int result = 0;
	std::visit(overload{
		           [&result](int& left, int& right) { result = left == right; },
		           [&result](std::string& left, std::string& right) { result = left == right; },
		           [](auto&, auto&) { throw std::runtime_error("== operator: incompatible operand types"); }
	           }, lhs.data, rhs.data);
	return Object(result);
}

Object operator!=(Object& lhs, Object& rhs)
{
	return !(lhs == rhs);
}

Object Object::operator!()
{
	int result = 0;
	std::visit(overload{
		           [&result](int& x) { result = !x; },
		           [](auto&) { throw std::runtime_error("! operator: incompatible operand types"); }
	           }, this->data);
	return Object(result);
}

Object operator||(Object& lhs, Object& rhs)
{
	int result = 0;
	std::visit(overload{
		           [&result](int& left, int& right) { result = left || right; },
		           [&result](std::string& left, std::string& right) { result = left < right; },
		           [](auto&, auto&) { throw std::runtime_error("|| operator: incompatible operand types"); }
	           }, lhs.data, rhs.data);
	return Object(result);
}

Object operator&&(Object& lhs, Object& rhs)
{
	int result = 0;
	std::visit(overload{
		           [&result](int& left, int& right) { result = left && right; },
		           [&result](std::string& left, std::string& right) { result = left < right; },
		           [](auto&, auto&) { throw std::runtime_error("&& operator: incompatible operand types"); }
	           }, lhs.data, rhs.data);
	return Object(result);
}

bool Object::isTrue()
{
	bool result = false;
	std::visit(overload{
		           [&result](int& x) { result = (x == 0) ? (false) : (true); },
		           [](auto&) { throw std::runtime_error("Not a bool value."); }
	           }, this->data);
	return result;
}

std::string Object::toStr()
{
	std::string result;
	std::visit(overload{
				   [&result](int& x) { result = std::to_string(x); },
		           [&result](std::string& x) { result = x; },
				   [&result](auto&) { result = "Object does not have a string representation."; }
	           }, data);

	return result;
}

Object* Object::operator()(Scope* scope, const std::vector<Object*>& argVec)
{
	Object* result = nullptr;
	std::visit(overload{
		           [&result, &scope, &argVec](Function& func) { result = func.eval(scope, argVec); },
		           [&result, &argVec](ArrayConstructor& array_constructor) { result = array_constructor.eval(argVec); },
		           [](auto&) { throw std::runtime_error("Not a callable object."); }
	           }, this->data);
	return result;
}

Object* Object::operator[](const std::vector<Object*>& indexVec)
{
	Object* result = nullptr;
	std::visit(overload{
		           [&result, &indexVec](ArrayContainer& array_container) { result = array_container.getArray(indexVec); },
		           [](auto&) { throw std::runtime_error("Not an array."); }
	           }, this->data);
	return result;

}

