#pragma once
#include "objects.h"
enum class OperatorType {
    UNARY_PLUS,
    UNARY_NEGATION,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    MODULO,
    OR,
    AND,
    NOT,
    BIT_AND,
    BIT_OR,
    BIT_NOT,
    BIT_XOR,
    BIT_LSHIFT,
    BIT_RSHIFT,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    LESS,
    GRE_EQ,
    LESS_EQ,
    ASSIGNMENT,
    OUTPUT,
    UNKNOWN
};

Object addition(const Object*, const Object*);
Object subtraction(const Object*, const Object*);
Object multiplication(const Object*, const Object*);
Object division(const Object*, const Object*);
Object l_or(const Object*, const Object*);
Object l_and(const Object*, const Object*);
