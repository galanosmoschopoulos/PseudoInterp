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
	int scopeLevel;
	Object operator=(const Object &obj2);
private:
	ObjectType currentType = ObjectType::UNDEFINED;
	std::vector<ObjectType> typeSeq;
	bool lval = false;
	//void traceTypeRecursive(Object &obj);
	ArrayContainer& getArrayContainer();
};

/*
class ObjectKey {
public:
	ObjectKey() = default;
	ObjectKey(std::string id, int level) : id(id), level(level) {}
	std::string id = "";
	int level;
	bool operator==(const ObjectKey& key2) const{
		return level == key2.level && id == key2.id;
	}
	bool operator<(const ObjectKey& key2) const {
		return level < key2.level;
	}
};*/

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
