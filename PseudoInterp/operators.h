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

