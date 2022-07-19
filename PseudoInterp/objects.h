#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <map>
#include "AST.h"

class Object;
class Function;
class Scope;
class ArrayContainer;
class VariantValueType;
using vecOfPtrs = std::vector<std::shared_ptr<Object>>;
using VariantType = std::variant<int, std::string, bool, ArrayContainer, Function>;

void output(const Object& obj);

class ASTNode;
class CodeBlock;

enum class ObjectType
{
	UNDEFINED,
	INT,
	STR,
	ARR,
	FUNC
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
	explicit ArrayContainer(size_t size);
	[[nodiscard]] vecOfPtrs* getVecPtr() const;
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
	std::vector<ASTNode*> paramVec;
	int definedFuncLevel = 0;
};

template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};

template <class... Ts>
explicit overload(Ts ...) -> overload<Ts...>;

class Object
{
public:
	Object();
	explicit Object(std::string val);
	explicit Object(int val);
	explicit Object(const Function&);
	Object(ObjectType type, int val);
	[[nodiscard]] ObjectType getType() const;
	Object* getArray(size_t pos);
	void setArray(const Object& obj, size_t pos);
	void initArray(size_t size);
	void setVal(auto val);
	void setType(ObjectType type);
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

	bool isTrue();

private:
	ObjectType currentType = ObjectType::UNDEFINED;
	bool lval = false;
	ArrayContainer& getArrayContainer();
};

class ObjKey
{
public:
	ObjKey();
	ObjKey(const int, const int, std::string);
	int scopeLevel = 0;
	int funcLevel = 0;
	std::string ID;

	bool operator<(const ObjKey& rhs) const
	{
		return std::tie(scopeLevel, ID) < std::tie(rhs.scopeLevel, rhs.ID);
	}
};

using ObjMap = std::map<ObjKey, Object*>;

class Scope
{
public:
	Scope();
	const ObjMap& getMap();
	[[nodiscard]] int getLevel() const;
	[[nodiscard]] int getFuncLevel() const;
	void incLevel();
	void incFuncLevel();
	void decrLevel();
	void decrFuncLevel();
	void addObj(const Object& obj, const std::string& id);
	Object* getObj(const std::string& id);
	[[nodiscard]] bool checkObj(const std::string& id) const;

	void printScope() const;
private:
	ObjMap scopeMap;
	int scopeLevel = 0;
	int funcLevel = 0;
};
