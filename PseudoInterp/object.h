#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <variant>
#include <stack>
#include "AST.h"

class Object;
class Function;
class Scope;
class ArrayContainer;
class StackContainer;
class StringContainer;
class VariantValueType;
using ExternalFunction = std::function<Object*(const std::vector<Object*>&)>;
using VariantType = std::variant<int, StringContainer, bool, float, char, ArrayContainer, StackContainer, Function,
                                 ExternalFunction>;

class ASTNode;
class CodeBlock;

enum class ObjectType
{
	UNDEFINED,
	INT,
	FLOAT,
	CHAR,
	STR,
	ARR,
	STACK,
	FUNC,
	BOOL,
	EXTERNAL_FUNCTION
};

class ArrayContainer
{
public:
	ArrayContainer();
	explicit ArrayContainer(const std::vector<Object*>&);
	Object* getArray(const std::vector<Object*>&) const;
private:
	std::vector<std::unique_ptr<Object>>* vecPtr = nullptr;
};

class StringContainer
{
public:
	StringContainer();
	explicit StringContainer(const std::string&);
	[[nodiscard]] Object* getChar(const std::vector<Object*>&) const;
	[[nodiscard]] std::string getStr() const;
private:
	std::vector<std::unique_ptr<Object>>* vecPtr = nullptr;
};

class StackContainer
{
public:
	explicit StackContainer(const std::vector<Object*>&);
	[[nodiscard]] Object* push(const std::vector<Object*>&) const;
	[[nodiscard]] Object* pop(const std::vector<Object*>&) const;
private:
	std::stack<std::unique_ptr<Object>>* stackPtr = nullptr;
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

class Object
{
public:
	Object();
	explicit Object(const auto& val) { data = val; }
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
	bool isPersistentType() const;
	void setPersistentType(bool);
private:
	//ObjectType currentType = ObjectType::UNDEFINED;
	bool lval = false;
	bool persistentType = false;
	ArrayContainer& getArrayContainer();
};
