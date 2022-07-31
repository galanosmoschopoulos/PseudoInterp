#include "operators.h"
#include <type_traits>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;


static VariantType cast_to_str(VariantType& var)
{
	std::string result;
	std::visit(overload{
		[&result](bool &val) {result = (val)?("true"):("false"); },
		[&result](unsigned char &val) {result = std::string(1, val); },
		[&result](int& val) {result = std::to_string(val); },
		[&result](float& val) {result = std::to_string(val); },
		[&result](std::string& i) {result = i; },
		[](auto&) {throw std::runtime_error("Impossible cast!"); }
		}, var);
	return VariantType(result);
}

static VariantType cast_to_char(VariantType& var)
{
	unsigned char result = 0;
	std::visit(overload{
		//[&result](bool& val) { result = static_cast<unsigned char>(val); },
		[&result](unsigned char& val) { result = val; },
		[](auto&) {throw std::runtime_error("Impossible cast!"); }
		}, var);
	return VariantType(result);
}

static VariantType cast_to_int(VariantType& v)
{
	int result = 0;
	std::visit(overload{
		[&result](int& val) {result = val; },
		[&result, &v](auto&) {result = static_cast<int>(std::get<unsigned char>(cast_to_char(v))); }
		}, v);
	return VariantType(result);
}
static VariantType cast_to_float(VariantType& var)
{
	float result = 0.0f;
	std::visit(overload{
		[&result](float& val) {result = val; },
		[&result, &var](auto&) {result = static_cast<float>(std::get<int>(cast_to_int(var))); }
		}, var);
	return VariantType(result);
}

static void cast_numerical(VariantType& varL, VariantType& varR)
{
	if (std::holds_alternative<float>(varL) || std::holds_alternative<float>(varR))
		varL = cast_to_float(varL), varR = cast_to_float(varR);
	else if (std::holds_alternative<int>(varL) || std::holds_alternative<int>(varR))
		varL = cast_to_int(varL), varR = cast_to_int(varR);
	else if (std::holds_alternative<unsigned char>(varL) || std::holds_alternative<unsigned char>(varR))
		varL = cast_to_char(varL), varR = cast_to_char(varR);
	else
		throw std::runtime_error("Unknown types in + operator.");
}

template<typename T>
static VariantType numericalOperator(VariantType varL, VariantType varR, T opFunc)
{
	cast_numerical(varL, varR);
	VariantType result;
	std::visit(overload{
		           [&result, &opFunc](unsigned char& left, unsigned char& right) { result = opFunc(left, right); },
		           [&result, &opFunc](int& left, int& right) { result = opFunc(left, right); },
		           [&result, &opFunc](float& left, float& right) { result = opFunc(left, right); },
		           [&result, &opFunc](auto&, auto&) {}
	           }, varL, varR);
	return result;
}
template<typename T>
static void numericalUnaryOperator(VariantType& var, T opFunc)
{
	VariantType result;
	std::visit(overload{
		           [&opFunc](unsigned char& val) { opFunc(val); },
		           [&opFunc](int& val) { opFunc(val); },
		           [&opFunc](float& val) { opFunc(val); },
		           [&opFunc](auto&) {}
	           }, var);
}

template<typename T>
static VariantType numericalOperatorNonFloat(VariantType varL, VariantType varR, T opFunc)
{
	cast_numerical(varL, varR);
	VariantType result;
	std::visit(overload{
		           [&result, &opFunc](unsigned char& left, unsigned char& right) { result = opFunc(left, right); },
		           [&result, &opFunc](int& left, int& right) { result = opFunc(left, right); },
		           [&result, &opFunc](auto&, auto&) {}
	           }, varL, varR);
	return result;
}

Object& Object::operator=(const Object& obj2)
{
	if (this == &obj2)
		return *this;
	this->data = obj2.data;
	return *this;
}

Object& Object::operator+=(Object& rhs)
{
	VariantType varL = this->data, varR = rhs.data;
	if (std::holds_alternative<std::string>(varL) || std::holds_alternative<std::string>(varR)) {
		varL = cast_to_str(varL), varR = cast_to_str(varR);
		this->data = VariantType(std::get<std::string>(varL) + std::get<std::string>(varR));
	}
	else {
		this->data = numericalOperator(varL, varR, [](auto &x, auto &y) {return x + y; });
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
	this->data = numericalOperator(varL, varR, [](auto &x, auto &y) {return x - y; });
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
	this->data = numericalOperator(varL, varR, [](auto &x, auto &y) {return x * y; });
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
	this->data = numericalOperator(varL, varR, [](auto &x, auto &y) {return x / y; });
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
	this->data = numericalOperatorNonFloat(varL, varR, [](auto &x, auto &y) {return x % y; });
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
	numericalUnaryOperator(data, [](auto &x) {++x; });
	return *this;
}

Object& Object::operator--()
{
	numericalUnaryOperator(data, [](auto &x) {--x; });
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
	numericalUnaryOperator(tmp.data, [](auto &x) {x *= -1; });
	return tmp;
}

Object Object::operator+() const
{
	Object tmp = *this;
	tmp.setLval(false);
	numericalUnaryOperator(tmp.data, [](auto &) { });
	return tmp;
}

Object operator<(Object& lhs, Object& rhs)
{
	VariantType varL = lhs.data, varR = rhs.data;
	Object result;
	if (std::holds_alternative<std::string>(varL) && std::holds_alternative<std::string>(varR)) 
		result.data = std::get<std::string>(varL) < std::get<std::string>(varR);
	else 
		result.data = numericalOperator(varL, varR, [](auto &x, auto &y) {return x < y; });
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
	if (std::holds_alternative<std::string>(varL) && std::holds_alternative<std::string>(varR))
		result.data = std::get<std::string>(varL) == std::get<std::string>(varR);
	else 
		result.data = numericalOperator(varL, varR, [](auto &x, auto &y) {return x == y; });
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

