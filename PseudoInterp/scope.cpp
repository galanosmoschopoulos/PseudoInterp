/* scope.cpp */

#include "scope.h"
#include "errors.h"
#include <iostream>
#include <ranges>

/*Initially some getters, setters and constrctors */
Scope::ObjKey::ObjKey() = default;

Scope::ObjKey::ObjKey(const int scopeLevel, const int funcLevel,
                      std::string ID) : scopeLevel(scopeLevel),
                                        funcLevel(funcLevel), ID(std::move(ID))
{
}

Scope::Scope() = default;

Scope::ObjMap& Scope::getMap()
{
	return scopeMap;
}

/* Used to increase/decrease levels*/
int Scope::getLevel() const { return scopeLevel; }
int Scope::getFuncLevel() const { return funcLevel; }
void Scope::incLevel() { scopeLevel++; }
void Scope::incFuncLevel() { funcLevel++; }
void Scope::decrFuncLevel() { funcLevel--; }

void Scope::setScopeLevel(const int level)
{
	scopeLevel = level;
}

void Scope::setFuncLevel(const int level)
{
	funcLevel = level;
}

void Scope::decrLevel()
{
	if (scopeLevel == 0)
	{
		throw FatalError("");
	}
	std::vector<ObjKey> toBeDeleted;
	// Iterated in reverse since highest scopelevel is at the beginning.
	// Keys of objects with current (highest) scopelevel are stored
	for (auto& itr : std::ranges::reverse_view(scopeMap))
	{
		if (itr.first.scopeLevel == scopeLevel)
		{
			delete itr.second; // Delete the object
			toBeDeleted.push_back(itr.first);
		}
	}
	// Objects are deleted
	for (const auto& x : toBeDeleted)
	{
		scopeMap.erase(x); // Remove keys of deleted objects
	}
	scopeLevel--; // The level counter is decremented
}

void Scope::addObj(const Object& obj, const std::string& id, bool isConst)
{
	Object* objPtr; // This will be the ponter of the new object
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = objPtr = new Object(obj);
	objPtr->setLval(true); // It is an lval since it exists in a scope
	objPtr->setConst(isConst);
}

void Scope::addObj(Object* obj, const std::string& id, bool isConst)
{
	// Objects are sorted by the first element of the key (highest to lowest scope level)
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = obj;
	obj->setLval(true);
	obj->setConst(isConst);
	// To prevent user from reassigning built in functions like output()
}

Object* Scope::getObj(const std::string& id)
{
	// Iterate in reverse, so that Objects with higher scopelevel are chosen first
	for (auto& itr : std::ranges::reverse_view(scopeMap))
	{
		if (itr.first.ID == id) // For the first object with matching ID
		{
			return itr.second; // Return it!
		}
	}
	return nullptr;
}

bool Scope::checkObj(const std::string& id)
{
	return getObj(id); // If getObj returns nullptr, the object doesn't exist
}

Scope* Scope::getRestricted(const int maxFuncLevel)
{
	// Creates a new scope, containing all variables of the existing scope that have a
	// func level less or equal to maxFuncLevel
	const auto newScope = new Scope;
	ObjMap& newMap = newScope->getMap(); // The map of the new scope
	for (auto& itr : scopeMap)
	{
		if (itr.first.funcLevel <= maxFuncLevel) 
			// If current obj's level is low enough, include it
			newMap.insert(itr);
		else // When level max level is surpassed, there's no point in continuing
			break;
	}
	newScope->setFuncLevel(newMap.rbegin()->first.funcLevel);
	newScope->setScopeLevel(newMap.rbegin()->first.scopeLevel);
	return newScope;
}

#include <type_traits>

template <typename T> /* checks if a string represents a number
					  if yes, converts that string to the number type*/
static bool str_to_numerical(const std::string& s, T& i)
{
	size_t pos;
	try // If user's input can be interpreted as a number, store it as a number
	{
		if constexpr (std::is_same_v<T, int>) // If expected type is int
		{
			i = std::stoi(s, &pos); // Call appropriate conversion function
		}
		else if constexpr (std::is_same_v<T, float>) // If expected type is float
		{
			i = std::stof(s, &pos);
		}
	}
	catch (std::invalid_argument&) // If number cannot be parsed
	{
		return false;
	}
	catch (std::out_of_range&) // If out of range error in parsing
	{
		return false;
	}
	if (pos != s.length()) return false; // Didn't manage to parse to whole thing
	return true;
}

void Scope::enableExternalFunctions()
// Adds external functions to the scope. These are pre-existing objects that can be
// called to construct data structures, or to use output(), input(), etc
{
	// The object's values are lambdas
	addObj(Object([](const std::vector<Object*>& argVec)  // Array constructor
	{
			// Some argument checking
		if (argVec.empty()) throw ArgumentError(
			"At least 1 array size parameter expected.");
		size_t currSize = 0;
		std::vector<size_t> dimVec(argVec.size()); // Will hold array dimensions
		for (size_t i = 0; i != dimVec.size(); i++)
		{
			// If the argument is an int
			if (const int* size = std::get_if<int>(&argVec[0]->data))
			{
				if (*size > 0) dimVec[i] = static_cast<size_t>(*size);
				// Dimension must be positive
				else throw ValueError(
					"Array size parameter must be a positive integer.");
			}
			else throw TypeError("Array size parameter must be an integer.");
		}

		return new Object(std::make_shared<ArrayContainer>(dimVec));
	}), "Array", true);

	addObj(Object([](const std::vector<Object*>& argVec) // Stack constructor
	{
			// Simply return an empty stack object
		return new Object(std::make_shared<StackContainer>(argVec)); 
	}), "Stack", true);
	addObj(Object([](const std::vector<Object*>& argVec) // Queue constructor
	{
		return new Object(std::make_shared<QueueContainer>(argVec));
	}), "Queue", true);
	addObj(Object([](const std::vector<Object*>& argVec) // Collection constructor
	{
		return new Object(std::make_shared<CollectionContainer>(argVec));
	}), "Collection", true);
	addObj(Object([](const std::vector<Object*>& argVec) // String constructor
	{
		return new Object(std::make_shared<StringContainer>(argVec));
	}), "String", true);
	addObj(Object([](const std::vector<Object*>& argVec) // output() function
	{
		for (Object* obj : argVec) // Get string representation of arguments
		{
			std::cout << obj->toStr() << ' '; // Print them separated with ' '
		}
		std::cout << '\n'; // newline at the end
		return new Object(0); // 0 signifies no error
	}), "output", true);
	addObj(Object([](const std::vector<Object*>& argVec) // input() function
	{
		// input(a) and a = input() are equivalent.
		// Hard coded functions have the luxury to support pass by reference!
		if (argVec.size() > 1) throw ArgumentError(
			"At most one argument expected.");
		std::string inputStr; // Holds user's input
		std::getline(std::cin, inputStr); // Get from standard input
		Object* inputObj = nullptr; // The resulting object
		if (int inputInt = 0; str_to_numerical(inputStr, inputInt))
			// If the input string is a valid integer i.e. "103"
			inputObj = new Object(inputInt); // Create int object
		else if (float inputFloat = 0.0f;
			str_to_numerical(inputStr, inputFloat)) // If it's a valid float
			inputObj = new Object(inputFloat); // Create float object
		else // Else just create a string object loaded with the input string
			inputObj = new Object(std::make_shared<StringContainer>(inputStr));
		if (argVec.size() == 1)
			// If you pass an argument to input(), then the input value is put in that
			// argument
			checkLval(*argVec[0] = *inputObj); // It must be an lvalue!
		return inputObj;
	}), "input", true);
}
