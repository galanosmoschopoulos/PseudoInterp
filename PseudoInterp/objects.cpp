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
Object::Object(ObjectType type, int val)
{
	currentType = type;
	data = val;
	if (type == ObjectType::ARR)
	{
		initArray(val);
	}
}
ObjectType Object::getType() const
{
	return currentType;
}
void Object::setType(ObjectType type)
{
	currentType = type;
}/*
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
	for (int i = 0; i != size; i++) {
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

Object* Object::getArray(size_t pos)
{
	if (currentType != ObjectType::ARR) {
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
ArrayContainer &Object::getArrayContainer()
{
	return std::get<ArrayContainer>(data);
}
void Object::printTypeTrace() const
{
	for (const auto &t : typeSeq)
	{
		std::cout << typeString[t] << std::endl;
	}
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
void Object::setLval(bool isIt)
{
	lval = isIt;
}
int Object::getInt() const
{
	if(getType() != ObjectType::INT)
	{
		throw std::runtime_error("Requesting int value from non int object.");
	}
	return std::get<int>(data);
}
std::string Object::getStr() const
{
	if(getType() != ObjectType::STR)
	{
		throw std::runtime_error("Requesting std::string from non string object.");
	}
	return std::get<std::string>(data);
}
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;
Object& Object::operator=(const Object& obj2) {
	if (this == &obj2)
		return *this;
	this->currentType = obj2.currentType;
	this->data = obj2.data;
	return *this;
}
Object& Object::operator+=(Object& rhs) {
	std::visit(overload{
		[](int& left ,int& right) {left += right; },
		[](int& left, std::string& right) {std::to_string(left) += right; },
		[](std::string& left, int& right) {left += std::to_string(right); },
		[](std::string& left, std::string& right) {left += right; },
		[](auto&, auto&) {throw std::runtime_error("+ operator: incompatible operand types"); }
		}, this->data, rhs.data);
	return *this;
}
Object operator+(Object lhs, Object& rhs) {
	lhs += rhs;
	lhs.setLval(false);
	return lhs;
}
Object& Object::operator-=(Object& rhs) {
	std::visit(overload{
		[](int& left ,int& right) {left -= right; },
		[](auto&, auto&) {throw std::runtime_error("- operator: incompatible operand types"); }
		}, this->data, rhs.data);
	return *this;
}
Object operator-(Object lhs, Object& rhs) {
	lhs -= rhs;
	lhs.setLval(false);
	return lhs;
}
Object& Object::operator*=(Object& rhs) {
	std::visit(overload{
		[](int& left ,int& right) {left *= right; },
		[](auto&, auto&) {throw std::runtime_error("* operator: incompatible operand types"); }
		}, this->data, rhs.data);
	return *this;
}
Object operator*(Object lhs, Object& rhs) {
	lhs *= rhs;
	lhs.setLval(false);
	return lhs;
}
Object& Object::operator/=(Object& rhs) {
	std::visit(overload{
		[](int& left ,int& right) {left /= right; },
		[](auto&, auto&) {throw std::runtime_error("/ operator: incompatible operand types"); }
		}, this->data, rhs.data);
	return *this;
}
Object operator/(Object lhs, Object& rhs) {
	lhs /= rhs;
	lhs.setLval(false);
	return lhs;
}
Object& Object::operator%=(Object& rhs) {
	std::visit(overload{
		[](int& left ,int& right) {left %= right; },
		[](auto&, auto&) {throw std::runtime_error("% operator: incompatible operand types"); }
		}, this->data, rhs.data);
	return *this;
}
Object operator%(Object lhs, Object& rhs) {
	lhs %= rhs;
	lhs.setLval(false);
	return lhs;
}
Object& Object::operator++() {
	std::visit(overload{
		[](int& x) {++x; },
		[](auto&) {throw std::runtime_error("++ operator: incompatible operand types"); }
		}, this->data);
	return *this;
}
Object& Object::operator--() {
	std::visit(overload{
		[](int& x) {--x; },
		[](auto&) {throw std::runtime_error("-- operator: incompatible operand types"); }
		}, this->data);
	return *this;
}
Object& Object::operator++(int) {
	Object old = *this;
	operator++();
	old.setLval(false);
	return old;
}
Object& Object::operator--(int) {
	Object old = *this;
	operator--();
	return old;
}
Object Object::operator-() {
	Object tmp = *this;
	std::visit(overload{
		[](int& x) {x *= -1; },
		[](auto&) {throw std::runtime_error("unary - operator: incompatible operand types"); }
		}, tmp.data);
	tmp.setLval(false);
	return tmp;
}
Object Object::operator+() {
	Object tmp = *this;
	std::visit(overload{
		[](int& x) {x *= +1; },
		[](auto&) {throw std::runtime_error("unary + operator: incompatible operand types"); }
		}, tmp.data);
	tmp.setLval(false);
	return tmp;
}
Object operator<(Object& lhs, Object& rhs) {
	int result = 0;
	std::visit(overload{
		[&result](int& left, int& right) {result = left<right; },
		[&result](std::string& left, std::string& right) {result = left<right; },
		[](auto&, auto &) {throw std::runtime_error("comparison operator: incompatible operand types"); }
		}, lhs.data, rhs.data);
	return Object(result);
}
Object operator>(Object& lhs, Object& rhs) {
	return rhs < lhs;
}
Object operator<=(Object& lhs, Object& rhs) {
	return !(lhs > rhs);
}
Object operator>=(Object& lhs, Object& rhs) {
	return !(lhs < rhs);
}
Object operator==(Object& lhs, Object& rhs) {
	int result = 0;
	std::visit(overload{
		[&result](int& left, int& right) {result = left==right; },
		[&result](std::string& left, std::string& right) {result = left==right; },
		[](auto&, auto &) {throw std::runtime_error("== operator: incompatible operand types"); }
		}, lhs.data, rhs.data);
	return Object(result);
}
Object operator!=(Object& lhs, Object& rhs) {
	return !(lhs == rhs);
}
Object Object::operator!() {
	int result = 0;
	std::visit(overload{
		[&result](int& x) {result = !x; },
		[](auto&) {throw std::runtime_error("! operator: incompatible operand types"); }
		}, this->data);
	return Object(result);
}
Object operator||(Object& lhs, Object& rhs) {
	int result = 0;
	std::visit(overload{
		[&result](int& left, int& right) {result = left||right; },
		[&result](std::string& left, std::string& right) {result = left<right; },
		[](auto&, auto &) {throw std::runtime_error("|| operator: incompatible operand types"); }
		}, lhs.data, rhs.data);
	return Object(result);
}
Object operator&&(Object& lhs, Object& rhs) {
	int result = 0;
	std::visit(overload{
		[&result](int& left, int& right) {result = left&&right; },
		[&result](std::string& left, std::string& right) {result = left<right; },
		[](auto&, auto &) {throw std::runtime_error("&& operator: incompatible operand types"); }
		}, lhs.data, rhs.data);
	return Object(result);
}

Scope::Scope() = default;
const ObjMap& Scope::getMap()
{
	return scopeMap;
}
int Scope::getLevel() { return level; }
void Scope::incLevel() { level++; }
void Scope::decrLevel() {
	if (level == 0) {
		throw std::runtime_error("Scope level cannot be negative.");
	}
	std::vector<std::pair<int, std::string>> toBeDeleted;
	for (ObjMap::reverse_iterator itr = scopeMap.rbegin(); itr != scopeMap.rend(); itr++) {
		if (itr->first.first == level) {
			toBeDeleted.push_back(itr->first);
		}
	}
	for (const auto& x : toBeDeleted) {
		scopeMap.erase(x);
	}
	level--;
}
void Scope::addObj(const Object& obj, const std::string& id)
{
	scopeMap[std::make_pair(level, id)] = obj;
	scopeMap[std::make_pair(level, id)].setLval(true);
}

Object* Scope::getObj(const std::string& id)
{/*
	for (ObjMap::reverse_iterator itr = scopeMap.rbegin(); itr != scopeMap.rend(); itr++) {
		if (itr->first.second == id) {
			return &itr->second;
		}
	}*/
	for (int i = level; i >= 0; i--) {
		if (scopeMap.contains(std::make_pair(i, id))) {
			return &scopeMap[std::make_pair(i, id)];
		}
	}
	throw std::runtime_error("Object with identifier \"" + id + "\" does not exist.");
}

bool Scope::checkObj(const std::string& id) const
{/*
	for (int i = level; i >= 0; i--) {
		if (scopeMap.contains(std::make_pair(i, id))) {
			return true;
		}
	}
	return false;*/
	return scopeMap.contains(std::make_pair(level, id));
}
void Scope::printScope() {
	for (auto x : scopeMap) {
		std::cout << "[ID: " << x.first.second << ", Level: " << x.first.first << "]\n";
	}
}

void output(const Object &obj)
{
	switch (obj.getType())
	{
	case ObjectType::INT:
		std::cout << std::get<int>(obj.data) << std::endl;
		break;
	case ObjectType::STR:
		std::cout << std::get<std::string>(obj.data) << std::endl;
		break;
	default:
		throw std::runtime_error("Object of type " + typeString[obj.getType()] + " is not printable.");
		break;
	}
}
