#pragma once
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <map>
#include "AST.h"
#include "scope.h"

class Object;
class Function;
class Scope;
class ArrayContainer;
class ArrayConstructor;
class VariantValueType;
using vecOfPtrs = std::vector<std::shared_ptr<Object>>;
using VariantType = std::variant<int, std::string, bool, ArrayContainer, ArrayConstructor, Function>;

class ASTNode;
class CodeBlock;

enum class ObjectType
{
	UNDEFINED,
	INT,
	STR,
	ARR,
	FUNC,
	ARRAY_CONSTRUCTOR
};

inline static std::map<ObjectType, std::string> typeString = {
	{ObjectType::INT, "int"},
	{ObjectType::STR, "std::string"},
	{ObjectType::ARR, "array"}
};

class ArrayContainer
{
public:
	ArrayContainer();
	explicit ArrayContainer(const std::vector<Object*>&);
	Object* getArray(const std::vector<Object*>&) const;
private:
	vecOfPtrs* vecPtr = nullptr;
};

class Function
{
public:
	Function();
	Function(CodeBlock*, std::vector<ASTNode*>, int);
	Object* eval(Scope* scope, const std::vector<Object*>& argVec) const;
private:
	CodeBlock* block = nullptr;
	std::vector<ASTNode*> paramVec{};
	int definedFuncLevel = 0;
};

class ArrayConstructor
{
public:
	ArrayConstructor();
	Object* eval(const std::vector<Object*>& argVec) const;
};

class Object
{
public:
	Object();
	explicit Object(std::string);
	explicit Object(int);
	explicit Object(const Function&);
	explicit Object(const ArrayContainer&);
	explicit Object(const ArrayConstructor&);

	[[nodiscard]] bool isLval() const;
	void setLval(bool isIt);
	VariantType data;

	Object& operator=(const Object&);
	Object& operator+=(Object&);
	friend Object operator+(Object, Object&);
	Object& operator-=(Object&);
	friend Object operator-(Object, Object&);
	Object& operator*=(Object&);
	friend Object operator*(Object, Object&);
	Object& operator/=(Object&);
	friend Object operator/(Object, Object&);
	Object& operator%=(Object&);
	friend Object operator%(Object, Object&);
	Object& operator++();
	Object& operator--();
	Object operator++(int);
	Object operator--(int);
	Object operator-() const;
	Object operator+() const;
	Object operator!();
	friend Object operator<(Object&, Object&);
	friend Object operator>(Object&, Object&);
	friend Object operator<=(Object&, Object&);
	friend Object operator>=(Object&, Object&);
	friend Object operator==(Object&, Object&);
	friend Object operator!=(Object&, Object&);
	friend Object operator||(Object&, Object&);
	friend Object operator&&(Object&, Object&);
	Object* operator()(Scope*, const std::vector<Object*>&);
	Object* operator[](const std::vector<Object*>&);
	bool isTrue();
	std::string toStr();

private:
	ObjectType currentType = ObjectType::UNDEFINED;
	bool lval = false;
	ArrayContainer& getArrayContainer();
};
