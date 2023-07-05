/* operators.h */

#pragma once
#include "object.h"
// Each of those casts an object to said type
static VariantType cast_to_char(VariantType&);
static VariantType cast_to_int(VariantType&);
static VariantType cast_to_float(VariantType&);
static VariantType cast_to_str(VariantType&);
// Brings the two object to the same type
static void cast_numerical(VariantType&, VariantType&);

// Applies operator defined in function T on operands after casting
template <typename T>
static VariantType numericalOperator(VariantType, VariantType, T);
template <typename T>
static void numericalUnaryOperator(VariantType&, T); // Similar for unary
template <typename T> // For integer-only operators
static VariantType numericalOperatorNonFloat(VariantType, VariantType, T);

