#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include "object.h"

class Object;

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
	ObjMap& getMap();
	[[nodiscard]] int getLevel() const;
	[[nodiscard]] int getFuncLevel() const;
	void incLevel();
	void incFuncLevel();
	void decrLevel();
	void decrFuncLevel();
	void setScopeLevel(int);
	void setFuncLevel(int);
	void addObj(const Object& obj, const std::string& id);
	void addObj(Object*, const std::string& id);
	Object* getObj(const std::string& id);
	[[nodiscard]] bool checkObj(const std::string& id);
	Scope* getRestricted(int);

	void printScope() const;
private:
	ObjMap scopeMap{};
	int scopeLevel = 0;
	int funcLevel = 0;
	bool derivativeScope = false;
};

