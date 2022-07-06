#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <map>

class Object;
class ArrayContainer;
class VariantValueType;
using vecOfPtrs = std::vector<std::shared_ptr<Object>>;
using VariantType = std::variant<int, std::string, bool, ArrayContainer>;

void output(const Object& obj);

enum class ObjectType
{
	UNDEFINED,
	INT,
	STR,
	ARR
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
	ArrayContainer(size_t size);
	[[nodiscard]] vecOfPtrs* getVecPtr() const;
private:
	vecOfPtrs* vecPtr = nullptr;
};

class Object
{
public:
	Object();
	explicit Object(std::string val);
	explicit Object(int val);
	Object(ObjectType type, int val);
	Object(ObjectType type, int size, ObjectType contained);
	[[nodiscard]] ObjectType getType() const;
	Object* getArray(size_t pos);
	void setArray(const Object& obj, size_t pos);
	void initArray(size_t size);
	void setVal(auto val);
	//void traceType();
	void printTypeTrace() const;
	void setType(ObjectType type);
	bool isLval() const;
	void setLval(bool isIt);
	int getInt() const;
	std::string getStr() const;
	VariantType data;

	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...)->overload<Ts...>;

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
	Object& operator++(int);
	Object& operator--(int);
	Object operator-();
	Object operator+();
	Object operator!();
	friend Object operator<(Object&, Object&);
	friend Object operator>(Object&, Object&);
	friend Object operator<=(Object&, Object&);
	friend Object operator>=(Object&, Object&);
	friend Object operator==(Object&, Object&);
	friend Object operator!=(Object&, Object&);
	friend Object operator||(Object&, Object&);
	friend Object operator&&(Object&, Object&);
	friend Object operator,(Object&, Object&);

	private:
	ObjectType currentType = ObjectType::UNDEFINED;
	std::vector<ObjectType> typeSeq;
	bool lval = false;
	//void traceTypeRecursive(Object &obj);
	ArrayContainer& getArrayContainer();
};

using ObjMap = std::map<std::pair<int, std::string>, Object>;
class Scope
{
public:
	Scope();
	const ObjMap& getMap();
	int getLevel();
	void incLevel();
	void decrLevel();
	void addObj(const Object& obj, const std::string& id);
	Object* getObj(const std::string& id);
	bool checkObj(const std::string& id) const;
	void printScope();
private:
	ObjMap scopeMap;
	int level = 0;
};
