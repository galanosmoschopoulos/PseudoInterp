#pragma once
#include "object.h"
static VariantType cast_to_char(VariantType&);
static VariantType cast_to_int(VariantType&);
static VariantType cast_to_float(VariantType&);
static VariantType cast_to_str(VariantType&);
static void cast_numerical(VariantType&, VariantType&);
template <typename T>
static VariantType numericalOperator(VariantType, VariantType, T);
template <typename T>
static void numericalUnaryOperator(VariantType&, T);
template <typename T>
static VariantType numericalOperatorNonFloat(VariantType, VariantType, T);
