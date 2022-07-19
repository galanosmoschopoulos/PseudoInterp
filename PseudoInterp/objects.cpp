#include "objects.h"
#include <iostream>
#include <stdexcept>

ArrayContainer::ArrayContainer() = default;

ArrayContainer::ArrayContainer(const size_t size)
{
	vecPtr = new vecOfPtrs(size);
}

vecOfPtrs* ArrayContainer::getVecPtr() const
{
	return vecPtr;
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

	scope->incLevel();

	Object* funcResult = nullptr;
	for (int i = 0; i != argVec.size(); i++)
	{
		*paramVec[i]->eval(scope, true) = *argVec[i];
	}
	funcResult = block->eval(scope, true);
	if (funcResult == nullptr) funcResult = new Object;
	scope->decrLevel();
	return funcResult;
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

Object::Object(const ObjectType type, int val)
{
	currentType = type;
	data = val;
	if (type == ObjectType::ARR)
	{
		initArray(val);
	}
}

Object::Object(const Function& function)
{
	currentType = ObjectType::FUNC;
	data = function;
}

ObjectType Object::getType() const
{
	return currentType;
}

void Object::setType(const ObjectType type)
{
	currentType = type;
} /*
void Object::traceType()
{
	traceTypeRecursive(*this);
}
void Object::traceTypeRecursive(Object &obj)
{
	typeSeq.push_back(obj.getType());
	if (obj.getType() == ObjectType::ARR)
	{
		if (obj.getArrayContainer().getVecPtr()->empty())
		{
			typeSeq.push_back(obj.getArrayContainer().getContainedType());
		}
		else
		{
			traceTypeRecursive(*(*obj.getArrayContainer().getVecPtr())[0]);
			//Assumes that the 0th element will have an object to trace... this is bad
		}
	}
}*/
void Object::initArray(const size_t size)
{
	if (getType() != ObjectType::ARR)
	{
		throw std::runtime_error("This object is not an array.");
	}
	data = ArrayContainer(size);
	for (size_t i = 0; i != size; i++)
	{
		(*getArrayContainer().getVecPtr())[i].reset(new Object);
	}
}

void Object::setArray(const Object& obj, const size_t pos) // Will not be useful, just for testing purposes
{
	if (getType() != ObjectType::ARR)
	{
		throw std::runtime_error("This object is not an array.");
	}
	if (pos >= getArrayContainer().getVecPtr()->size())
	{
		throw std::runtime_error("Array subscript out of range.");
		return;
	}
	(*getArrayContainer().getVecPtr())[pos].reset(new Object(obj));
}

Object* Object::getArray(const size_t pos)
{
	if (currentType != ObjectType::ARR)
	{
		throw std::runtime_error("Object not array");
	}
	if (getType() != ObjectType::ARR)
	{
		throw std::runtime_error("This object is not an array.");
	}
	if (pos >= getArrayContainer().getVecPtr()->size())
	{
		throw std::runtime_error("Array subscript out of range.");
	}

	return (*getArrayContainer().getVecPtr())[pos].get(); // Convert std::shared_ptr to raw pointer
}

ArrayContainer& Object::getArrayContainer()
{
	return std::get<ArrayContainer>(data);
}

void Object::setVal(auto val)
{
	data = val;
	int x;
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

Object* Object::operator()(Scope* scope, const std::vector<Object*>& argVec)
{
	Object* result = nullptr;
	std::visit(overload{
		           [&result, &scope, &argVec](Function& func) { result = func.eval(scope, argVec); },
		           [](auto&) { throw std::runtime_error("Not a function."); }
	           }, this->data);
	return result;
}

ObjKey::ObjKey() = default;

ObjKey::ObjKey(const int scopeLevel, const int funcLevel, std::string ID) : scopeLevel(scopeLevel),
                                                                            funcLevel(funcLevel), ID(std::move(ID))
{
}

Scope::Scope() = default;

const ObjMap& Scope::getMap()
{
	return scopeMap;
}

int Scope::getLevel() const { return scopeLevel; }
int Scope::getFuncLevel() const { return funcLevel; }
void Scope::incLevel() { scopeLevel++; }
void Scope::incFuncLevel() { funcLevel++; }
void Scope::decrFuncLevel() { funcLevel--; }

void Scope::decrLevel()
{
	if (scopeLevel == 0)
	{
		throw std::runtime_error("Scope level cannot be negative.");
	}
	std::vector<ObjKey> toBeDeleted;
	for (auto itr = scopeMap.rbegin(); itr != scopeMap.rend(); ++itr)
	{
		if (itr->first.scopeLevel == scopeLevel)
		{
			toBeDeleted.push_back(itr->first);
		}
	}
	for (const auto& x : toBeDeleted)
	{
		scopeMap.erase(x);
	}
	scopeLevel--;
}

void Scope::addObj(const Object& obj, const std::string& id)
{
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = new Object(obj);
	scopeMap[ObjKey(scopeLevel, funcLevel, id)]->setLval(true);
}

Object* Scope::getObj(const std::string& id)
{
	for (auto itr = scopeMap.rbegin(); itr != scopeMap.rend(); ++itr)
	{
		if (itr->first.ID == id)
		{
			return itr->second;
		}
	}
	throw std::runtime_error("Object with identifier \"" + id + "\" does not exist.");
}

bool Scope::checkObj(const std::string& id) const
{
	return scopeMap.contains(ObjKey(scopeLevel, funcLevel, id));
}

void Scope::printScope() const
{
	for (std::pair<ObjKey, Object*> x : scopeMap)
	{
		std::cout << "[ID: " << x.first.ID << ", Level: " << x.first.scopeLevel << "]\n";
	}
}

void output(const Object& obj)
{
	switch (obj.getType())
	{
	case ObjectType::INT:
		std::cout << std::get<int>(obj.data) << std::endl;
		break;
	case ObjectType::STR:
		std::cout << std::get<std::string>(obj.data) << std::endl;
		break;
	case ObjectType::ARR: break;

	case ObjectType::FUNC:
	case ObjectType::UNDEFINED:
		throw std::runtime_error("Object of type " + typeString[obj.getType()] + " is not printable.");
		break;
	}
}
