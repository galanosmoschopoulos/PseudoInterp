#pragma once
#include <string>
#include <map>

class Object;

class ObjKey // It's defined so that a smaller scopelevel corresponds to smaller ObjKey
{
public:
	ObjKey();
	ObjKey(int, int, std::string);
	int scopeLevel = 0;
	int funcLevel = 0;
	std::string ID;

	bool operator<(const ObjKey& rhs) const
	{
		// Lexicographical (dictionary-like) comparison w.r.t scopeLevel and ID
		return std::tie(scopeLevel, ID) < std::tie(rhs.scopeLevel, rhs.ID);
	}
};

using ObjMap = std::map<ObjKey, Object*>; // That way the map is constructed with increasing scopelevel objects

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
	void addObj(const Object& obj, const std::string& id, bool isConst = false);
	void addObj(Object*, const std::string& id, bool isConst = false);
	Object* getObj(const std::string& id);
	[[nodiscard]] bool checkObj(const std::string& id);
	Scope* getRestricted(int);
	void enableExternalFunctions();
private:
	ObjMap scopeMap{};
	// Scope level increases when we enter a nested scope (i.e. in a code block).
	// When a variable at a higher scope level has the same identifier as one in a lower, the one in the higher will be chosen if the name is mentioned
	int scopeLevel = 0;
	// Func level increases by one each time we enter a function scope. Each function has access to variables outside each scope. These variables must have been
	// Defined at a func level lower or equal to the func level of the function definition, and before the function is called.
	// I.e. if a function B is defined within func A, and is called after having defined vars a, b, c in A, then func B has access to a, b, c, and all the vars defined outside of A.
	int funcLevel = 0;
};

#include "object.h"
