#include "operators.h"
#include "errors.h"

template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};


static VariantType cast_to_str(VariantType& var)
{
	StringContainer result;
	std::visit(overload{
		           [&result](bool& val) { result = StringContainer((val) ? ("true") : ("false")); },
		           [&result](char& val) { result = StringContainer(std::string(1, val)); },
		           [&result](int& val) { result = StringContainer(std::to_string(val)); },
		           [&result](float& val) { result = StringContainer(std::to_string(val)); },
		           [&result](StringContainer i) { result = StringContainer(i); },
		           [](auto&) { throw TypeError("Cannot cast to string."); }
	           }, var);
	return VariantType(result);
}

static VariantType cast_to_char(VariantType& var)
{
	char result = 0;
	std::visit(overload{
		           //[&result](bool& val) { result = static_cast<unsigned char>(val); },
		           [&result](char& val) { result = val; },
		           [](auto&) { throw TypeError("Impossible to implicitly cast type to numerical."); }
	           }, var);
	return VariantType(result);
}

static VariantType cast_to_int(VariantType& v)
{
	int result = 0;
	std::visit(overload{
		           [&result](int& val) { result = val; },
		           [&result, &v](auto&) { result = static_cast<int>(std::get<char>(cast_to_char(v))); }
	           }, v);
	return VariantType(result);
}

static VariantType cast_to_float(VariantType& var)
{
	float result = 0.0f;
	std::visit(overload{
		           [&result](float& val) { result = val; },
		           [&result, &var](auto&) { result = static_cast<float>(std::get<int>(cast_to_int(var))); }
	           }, var);
	return VariantType(result);
}

static void cast_numerical(VariantType& varL, VariantType& varR)
{
	if (std::holds_alternative<float>(varL) || std::holds_alternative<float>(varR))
	{
		varL = cast_to_float(varL);
		varR = cast_to_float(varR);
	}
	else if (std::holds_alternative<int>(varL) || std::holds_alternative<int>(varR))
	{
		varL = cast_to_int(varL);
		varR = cast_to_int(varR);
	}
	else if (std::holds_alternative<char>(varL) || std::holds_alternative<char>(varR))
	{
		varL = cast_to_char(varL);
		varR = cast_to_char(varR);
	}
	else
		throw TypeError("Impossible to implicitly cast type to numerical.");
}

template <typename T>
static VariantType numericalOperator(VariantType varL, VariantType varR, T opFunc)
{
	cast_numerical(varL, varR);
	VariantType result;
	std::visit(overload{
		           [&result, &opFunc](char& left, char& right) { result = opFunc(left, right); },
		           [&result, &opFunc](int& left, int& right) { result = opFunc(left, right); },
		           [&result, &opFunc](float& left, float& right) { result = opFunc(left, right); },
		           [](auto&, auto&)
		           {
		           }
	           }, varL, varR);
	return result;
}

template <typename T>
static void numericalUnaryOperator(VariantType& var, T opFunc)
{
	VariantType result;
	std::visit(overload{
		           [&opFunc](char& val) { opFunc(val); },
		           [&opFunc](int& val) { opFunc(val); },
		           [&opFunc](float& val) { opFunc(val); },
		           [](auto&)
		           {
		           }
	           }, var);
}

template <typename T>
static VariantType numericalOperatorNonFloat(VariantType varL, VariantType varR, T opFunc)
{
	cast_numerical(varL, varR);
	VariantType result;
	std::visit(overload{
		           [&result, &opFunc](char& left, char& right) { result = opFunc(left, right); },
		           [&result, &opFunc](int& left, int& right) { result = opFunc(left, right); },
		           [](auto&, auto&)
		           {
		           }
	           }, varL, varR);
	return result;
}

Object& Object::operator=(const Object& obj2)
{
	if (this == &obj2)
		return *this;
	if (isConst())
		throw TypeError("Cannot modify const object.");
	if (this->data.index() != obj2.data.index() && this->isPersistentType())
		throw TypeError("The type of the left hand side object cannot be changed.");
	std::visit(overload{
		[this](bool& i) {data = i; },
		[this](char& i) {data = i; },
		[this](float& i) {data = i; },
		[this](int& i) {data = i; },
		[this](StringContainer& i) {data = i; },
		[this](ArrayContainer& i) {data = i; },
		[this](Function& i) {data = i; },
		[this](ExternalFunction& i) {data = i; },
		[this](std::shared_ptr<StackContainer>& sc) { data = std::make_shared<StackContainer>(*sc); },
		[](auto&) { }
		}, const_cast<Object&>(obj2).data);

	return *this;
}

Object& Object::operator+=(Object& rhs)
{
	VariantType varL = this->data, varR = rhs.data;
	if (std::holds_alternative<StringContainer>(varL) || std::holds_alternative<StringContainer>(varR))
	{
		varL = cast_to_str(varL);
		varR = cast_to_str(varR);
		this->data = StringContainer(std::get<StringContainer>(varL).getStr() + std::get<StringContainer>(varR).getStr());
	}
	else
	{
		this->data = numericalOperator(varL, varR, [](auto& x, auto& y) { return x + y; });
	}
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
	VariantType varL = this->data, varR = rhs.data;
	this->data = numericalOperator(varL, varR, [](auto& x, auto& y) { return x - y; });
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
	VariantType varL = this->data, varR = rhs.data;
	this->data = numericalOperator(varL, varR, [](auto& x, auto& y) { return x * y; });
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
	VariantType varL = this->data, varR = rhs.data;
	this->data = numericalOperator(varL, varR, [](auto& x, auto& y) { return x / y; });
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
	VariantType varL = this->data, varR = rhs.data;
	this->data = numericalOperatorNonFloat(varL, varR, [](auto& x, auto& y) { return x % y; });
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
	numericalUnaryOperator(data, [](auto& x) { ++x; });
	return *this;
}

Object& Object::operator--()
{
	numericalUnaryOperator(data, [](auto& x) { --x; });
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
	tmp.setLval(false);
	numericalUnaryOperator(tmp.data, [](auto& x) { x *= -1; });
	return tmp;
}

Object Object::operator+() const
{
	Object tmp = *this;
	tmp.setLval(false);
	numericalUnaryOperator(tmp.data, [](auto&)
	{
	});
	return tmp;
}

Object operator<(Object& lhs, Object& rhs)
{
	VariantType varL = lhs.data, varR = rhs.data;
	Object result;
	if (std::holds_alternative<StringContainer>(varL) && std::holds_alternative<StringContainer>(varR))
		result.data = std::get<StringContainer>(varL).getStr() < std::get<StringContainer>(varR).getStr();
	else
		result.data = numericalOperator(varL, varR, [](auto& x, auto& y) { return x < y; });
	return result;
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
	VariantType varL = lhs.data, varR = rhs.data;
	Object result;
	if (std::holds_alternative<StringContainer>(varL) && std::holds_alternative<StringContainer>(varR))
		result.data = std::get<StringContainer>(varL).getStr() == std::get<StringContainer>(varR).getStr();
	else
		result.data = numericalOperator(varL, varR, [](auto& x, auto& y) { return x == y; });
	return result;
}

Object operator!=(Object& lhs, Object& rhs)
{
	return !(lhs == rhs);
}

Object Object::operator!()
{
	return Object(!this->isTrue());
}

Object operator||(Object& lhs, Object& rhs)
{
	return Object(lhs.isTrue() || rhs.isTrue());
}

Object operator&&(Object& lhs, Object& rhs)
{
	return Object(lhs.isTrue() && rhs.isTrue());
}

Object* Object::operator()(Scope* scope, const std::vector<Object*>& argVec)
{
	Object* result = nullptr;
	std::visit(overload{
		           [&result, &scope, &argVec](Function& func) { result = func.eval(scope, argVec); },
		           [&result, &argVec](ExternalFunction& external_function) { result = external_function(argVec); },
		           [](auto&) { throw TypeError("Not a callable object."); }
	           }, this->data);
	return result;
}

Object* Object::operator[](const std::vector<Object*>& indexVec)
{
	Object* result = nullptr;
	std::visit(overload{
		           [&result, &indexVec](ArrayContainer& array_container){ result = array_container.getArray(indexVec); },
		           [&result, &indexVec](StringContainer& string_container){ result = string_container.getChar(indexVec); },
		           [](auto&) { throw TypeError("Object does not accept a subscript."); }
	           }, this->data);
	return result;
}
