#include "objects.h"
#include "operators.h"
#include <string>
#include <stdexcept>
#include <map>


Object subtraction(const Object *left, const Object *right)
{
	if (left->getType() != ObjectType::INT || right->getType() != ObjectType::INT)
	{
        throw std::runtime_error("Operands of incompatible types in subtraction operator");
	}
    return Object(left->getInt() - right->getInt());
}

Object multiplication(const Object *left, const Object *right)
{
	if (left->getType() != ObjectType::INT || right->getType() != ObjectType::INT)
	{
        throw std::runtime_error("Operands of incompatible types in multiplication operator");
	}
    return Object(left->getInt() * right->getInt());
}

Object division(const Object *left, const Object *right)
{
	if (left->getType() != ObjectType::INT || right->getType() != ObjectType::INT)
	{
        throw std::runtime_error("Operands of incompatible types in division operator");
	}
    return Object(left->getInt() - right->getInt());
}

