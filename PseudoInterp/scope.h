/* scope.h */

#pragma once
#include <string>
#include <map>

class Object;


class Scope
{
public:
	Scope();

	class ObjKey
		// It's defined so that a smaller scopelevel corresponds to smaller ObjKey
	{
	public:
		ObjKey();
		ObjKey(int, int, std::string);
		// A key involves the scope and function levels, as well as the ID
		int scopeLevel = 0;
		int funcLevel = 0;
		std::string ID;

		bool operator<(const ObjKey& rhs) const // Overload < operator
		{
			// Lexicographical (dictionary-like) comparison w.r.t scopeLevel and ID
			return std::tie(scopeLevel, ID) < std::tie(rhs.scopeLevel, rhs.ID);
		}
	};

	using ObjMap = std::map<ObjKey, Object*>;
	// That way the map is constructed with increasing scopelevel objects

	ObjMap& getMap(); // 
	[[nodiscard]] int getLevel() const;
	[[nodiscard]] int getFuncLevel() const;
	// Functions used to increase/decrease levels
	void incLevel();
	void incFuncLevel();
	void decrLevel();
	void decrFuncLevel();
	void setScopeLevel(int); // Set specific levels
	void setFuncLevel(int);
	// Existing (user's) objects must be added by passing pointers to scope
	void addObj(const Object& obj, const std::string& id, bool isConst = false);
	// Hardcoded objects (ExternalFunctions) are passed as arguments
	void addObj(Object*, const std::string& id, bool isConst = false);
	Object* getObj(const std::string& id); // Get pointer of object with said ID
	[[nodiscard]] bool checkObj(const std::string& id); // Does this var exist?
	Scope* getRestricted(int);
	void enableExternalFunctions(); // Load hardcoded functions (i.e. output)
private:
	ObjMap scopeMap{};
	// Scope level increases when we enter a nested scope (i.e. in a code block).
	// When a variable at a higher scope level has the same identifier as one in a lower,
	// the one in the higher will be chosen if the name is mentioned
	int scopeLevel = 0;
	// Func level increases by one each time we enter a function scope. Each function has
	// access to variables outside each scope. These variables must have been
	// Defined at a func level lower or equal to the func level of the function definition,
	// and before the function is called.
	// I.e. if a function B is defined within func A, and is called after having defined
	// vars a, b, c in A, then func B has access to a, b, c, and all the vars defined
	// outside of A.
	int funcLevel = 0;
};

#include "object.h"
