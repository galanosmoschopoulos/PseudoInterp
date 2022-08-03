#include "operators.h"
#include "errors.h"

template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};


static VariantType cast_to_str(VariantType& var) // Converts anything to a string
{
	StringContainer result;
	std::visit(overload{
		           [&result](bool& val) { result = StringContainer((val) ? ("true") : ("false")); },
		           [&result](char& val) { result = StringContainer(std::string(1, val)); },
		           [&result](int& val) { result = StringContainer(std::to_string(val)); },
		           [&result](float& val) { result = StringContainer(std::to_string(val)); },
		           [&result](std::shared_ptr<StringContainer> i) { result = StringContainer(*i); },
		           [](auto&) { throw TypeError("Cannot cast to string."); }
	           }, var);
	return VariantType(std::make_shared<StringContainer>(result));
}

static VariantType cast_to_char(VariantType& var) // We can't go lower than char
{
	char result = 0;
	std::visit(overload{
		           //[&result](bool& val) { result = static_cast<unsigned char>(val); },
		           [&result](char& val) { result = val; },
		           [](auto&) { throw TypeError("Impossible to implicitly cast type to numerical."); }
	           }, var);
	return VariantType(result);
}

static VariantType cast_to_int(VariantType& v)// If it holds an int, return it. If not, assume it is char, cast to char and then convert to float.
{
	int result = 0;
	std::visit(overload{
		           [&result](int& val) { result = val; },
		           [&result, &v](auto&) { result = static_cast<int>(std::get<char>(cast_to_char(v))); }
	           }, v);
	return VariantType(result);
}

static VariantType cast_to_float(VariantType& var) // If it holds a float, return it. If not, assume it is int, cast to int and then convert to float.
{
	float result = 0.0f;
	std::visit(overload{
		           [&result](float& val) { result = val; },
		           [&result, &var](auto&) { result = static_cast<float>(std::get<int>(cast_to_int(var))); }
	           }, var);
	return VariantType(result);
}

static void cast_numerical(VariantType& varL, VariantType& varR) // Tries to achieve the same type between varL and varR without loss of info.
{
	if (std::holds_alternative<float>(varL) || std::holds_alternative<float>(varR)) // If either one or the other holds float
	{
		varL = cast_to_float(varL);
		varR = cast_to_float(varR);
	}
	else if (std::holds_alternative<int>(varL) || std::holds_alternative<int>(varR)) // If either one or the other holds int
	{
		varL = cast_to_int(varL);
		varR = cast_to_int(varR);
	}
	else if (std::holds_alternative<char>(varL) || std::holds_alternative<char>(varR))  // If either one or the other holds char
	{
		varL = cast_to_char(varL);
		varR = cast_to_char(varR);
	}
	else
		throw TypeError("Impossible to implicitly cast type to numerical.");
}

template <typename T>
static VariantType numericalOperator(VariantType varL, VariantType varR, T opFunc) // Accepts two operands and a lambda with two parameters
{
	cast_numerical(varL, varR);
	VariantType result;
	std::visit(overload{
		           [&result, &opFunc](char& left, char& right) { result = static_cast<char>(opFunc(left, right)); },
		           [&result, &opFunc](int& left, int& right) { result = static_cast<int>(opFunc(left, right)); },
		           [&result, &opFunc](float& left, float& right) { result = static_cast<float>(opFunc(left, right)); },
		           [](auto&, auto&)
		           {
		           }
	           }, varL, varR);
	return result;
}

template <typename T>
static void numericalUnaryOperator(VariantType& var, T opFunc)// Accepts one operands and a lambda with one parameter
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
		           [&result, &opFunc](char& left, char& right) { result = static_cast<char>(opFunc(left, right)); },
		           [&result, &opFunc](int& left, int& right) { result = static_cast<int>(opFunc(left, right)); },
		           [](auto&, auto&)
		           {
		           }
	           }, varL, varR);
	return result;
}

template <typename T>
static T forceCast(VariantType& var)
{
	T result;
	std::visit(overload{
		[&result](char& x) {result = static_cast<T>(x); },
		[&result](int& x) {result = static_cast<T>(x); },
		[&result](float& x) {result = static_cast<T>(x); },
		[](auto&) {}
	}, var);
	return result;
}

Object& Object::operator=(const Object& obj2)
{
	auto& obj2_nonconst = const_cast<Object&>(obj2);
	if (this == &obj2)
		return *this;
	if (isConst())
		throw TypeError("Cannot modify const object.");
	if (data.index() != obj2_nonconst.data.index() && isPersistentType()) {
		std::visit(overload{
			[this, &obj2_nonconst](char& x) {x = forceCast<char>(obj2_nonconst.data); },
			[this, &obj2_nonconst](int& x) {x = forceCast<int>(obj2_nonconst.data); },
			[this, &obj2_nonconst](float& x) {x = forceCast<float>(obj2_nonconst.data); },
			[](auto& x) {throw TypeError("The type of the left hand side object cannot be changed."); }
			}, data);
		return *this;
	}
	std::visit(overload{
		           [this](bool& i) { data = i; },
		           [this](char& i) { data = i; },
		           [this](float& i) { data = i; },
		           [this](int& i) { data = i; },
		           [this](Function& i) { data = i; },
		           [this](ExternalFunction& i) { data = i; },
				   // Containers are in fact pointers to containers. Instead of copying the pointer, we should instantiate another object that is a copy of the one pointed by the pointer
		           [this](std::shared_ptr<StackContainer>& sc) { data = std::make_shared<StackContainer>(*sc); },
		           [this](std::shared_ptr<ArrayContainer>& ac) { data = std::make_shared<ArrayContainer>(*ac); },
		           [this](std::shared_ptr<QueueContainer>& qc) { data = std::make_shared<QueueContainer>(*qc); },
		           [this](std::shared_ptr<CollectionContainer>& cc) { data = std::make_shared<CollectionContainer>(*cc); },
		           [this](std::shared_ptr<StringContainer>& sc) { data = std::make_shared<StringContainer>(*sc); },
		           [](auto&)
		           {
		           }
	           }, obj2_nonconst.data);

	return *this;
}

Object& Object::operator+=(Object& rhs)
{
	//
	if (std::holds_alternative<std::shared_ptr<StringContainer>>(data) || std::holds_alternative<std::shared_ptr<StringContainer>>(rhs.data))
	{
		VariantType varL, varR;
		varL = cast_to_str(data);
		varR = cast_to_str(rhs.data);
		*this = Object(std::make_shared<StringContainer>(
			std::get<std::shared_ptr<StringContainer>>(varL)->getStr() + std::get<std::shared_ptr<StringContainer>>(varR)->getStr()));
	}
	else
	{
		*this = Object(numericalOperator(data, rhs.data, [](auto& x, auto& y) { return x + y; }));
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
	*this = Object(numericalOperator(data, rhs.data, [](auto& x, auto& y) { return x - y; }));
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
	*this = Object(numericalOperator(data, rhs.data, [](auto& x, auto& y) { return x * y; }));
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
	*this = Object(numericalOperator(data, rhs.data, [](auto& x, auto& y) { return x / y; }));
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
	*this = Object(numericalOperatorNonFloat(data, rhs.data, [](auto& x, auto& y) { return x % y; }));
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
	if (std::holds_alternative<std::shared_ptr<StringContainer>>(varL) && std::holds_alternative<std::shared_ptr<StringContainer>>(varR)) // Strings can be compared too
		result.data = std::get<std::shared_ptr<StringContainer>>(varL)->getStr() < std::get<std::shared_ptr<StringContainer>>(varR)->getStr();
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
	if (std::holds_alternative<std::shared_ptr<StringContainer>>(varL) && std::holds_alternative<std::shared_ptr<StringContainer>>(varR))
		result.data = std::get<std::shared_ptr<StringContainer>>(varL)->getStr() == std::get<std::shared_ptr<StringContainer>>(varR)->getStr();
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
	std::visit(overload{ // Function call operator
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
		           [&result, &indexVec](std::shared_ptr<ArrayContainer>& array_container)
		           {
			           result = array_container->getArray(indexVec);
		           },
		           [&result, &indexVec](std::shared_ptr<StringContainer>& string_container)
		           {
			           result = string_container->getChar(indexVec);
		           },
		           [](auto&) { throw TypeError("Object does not accept a subscript."); }
	           }, this->data);
	return result;
}
