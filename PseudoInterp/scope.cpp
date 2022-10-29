#include "scope.h"
#include "errors.h"
#include <iostream>
#include <ranges>

ObjKey::ObjKey() = default;

ObjKey::ObjKey(const int scopeLevel, const int funcLevel, std::string ID) : scopeLevel(scopeLevel),
                                                                            funcLevel(funcLevel), ID(std::move(ID))
{
}

Scope::Scope() = default;

ObjMap& Scope::getMap()
{
	return scopeMap;
}

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
	scopeLevel--;
}

void Scope::addObj(const Object& obj, const std::string& id, bool isConst)
{
	Object* objPtr;
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = objPtr = new Object(obj);
	objPtr->setLval(true);
	objPtr->setConst(isConst);
}

void Scope::addObj(Object* obj, const std::string& id, bool isConst)
{
	// Objects are sorted by the first element of the key (highest to lowest scope level)
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = obj;
	obj->setLval(true);
	obj->setConst(isConst); // To prevent user from reassigning built in functions like output()
}

Object* Scope::getObj(const std::string& id)
{
	// Iterate in reverse, so that Objects with higher scopelevel are chosen first
	for (auto& itr : std::ranges::reverse_view(scopeMap))
	{
		if (itr.first.ID == id)
		{
			return itr.second;
		}
	}
	return nullptr;
}

bool Scope::checkObj(const std::string& id)
{
	return getObj(id);
}

Scope* Scope::getRestricted(const int maxFuncLevel)
{
	// Creates a new scope, containing all variables of the existing scope that have a func level less or equal to maxFuncLevel
	const auto newScope = new Scope;
	ObjMap& newMap = newScope->getMap();
	for (auto& itr : scopeMap)
	{
		if (itr.first.funcLevel <= maxFuncLevel)
			newMap.insert(itr);
		else
			break;
	}
	newScope->setFuncLevel(newMap.rbegin()->first.funcLevel);
	newScope->setScopeLevel(newMap.rbegin()->first.scopeLevel);
	return newScope;
}

static bool str_to_int(const std::string& s, int& i)
{
	size_t pos;
	try // If user's input can be interpreted as a number, store it as a number
	{
		i = std::stoi(s, &pos);
	}
	catch (std::invalid_argument&)
	{
		return false;
	}
	catch (std::out_of_range&)
	{
		return false;
	}
	if (pos != s.length()) return false;
	return true;
}

#include <type_traits>
template<typename T>
static bool str_to_numerical(const std::string& s, T& i)
{
	size_t pos;
	try // If user's input can be interpreted as a number, store it as a number
	{
		if constexpr(std::is_same_v<T, int>)
		{
			i = std::stoi(s, &pos);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			i = std::stof(s, &pos);
		}
	}
	catch (std::invalid_argument&)
	{
		return false;
	}
	catch (std::out_of_range&)
	{
		return false;
	}
	if (pos != s.length()) return false;
	return true;
}

void Scope::enableExternalFunctions() // Adds external functions to the scope. These are pre-existing objects that can be called to construct data structures, or to use output(), input(), etc
{
	addObj(Object([](const std::vector<Object*>& argVec)
	{
		if (argVec.empty()) throw ArgumentError("At least 1 array size parameter expected.");
		size_t currSize = 0;
		std::vector<size_t> dimVec(argVec.size());
		for (size_t i = 0; i != dimVec.size(); i++) {
			if (const int* size = std::get_if<int>(&argVec[0]->data))
			{
				if (*size > 0) dimVec[i] = static_cast<size_t>(*size);
				else throw ValueError("Array size parameter must be a positive integer.");
			}
			else throw TypeError("Array size parameter must be an integer.");
		}

		return new Object(std::make_shared<ArrayContainer>(dimVec));
	}), "Array",true);

	addObj(Object([](const std::vector<Object*>& argVec)
	{
		return new Object(std::make_shared<StackContainer>(argVec));
	}), "Stack", true);
	addObj(Object([](const std::vector<Object*>& argVec)
	{
		return new Object(std::make_shared<QueueContainer>(argVec));
	}), "Queue", true);
	addObj(Object([](const std::vector<Object*>& argVec)
	{
		return new Object(std::make_shared<CollectionContainer>(argVec));
	}), "Collection", true);
	addObj(Object([](const std::vector<Object*>& argVec)
	{
		return new Object(std::make_shared<StringContainer>(argVec));
	}), "String", true);
	addObj(Object([](const std::vector<Object*>& argVec)
	{
		for (Object* obj : argVec)
		{
			std::cout << obj->toStr() << ' ';
		}
		std::cout << '\n';
		return nullptr;
	}), "output", true);
	addObj(Object([](const std::vector<Object*>& argVec)
	{
		if (argVec.size() > 1) throw ArgumentError("At most one argument expected.");
		std::string inputStr;
		std::getline(std::cin, inputStr);
		Object* inputObj = nullptr;
		if (int inputInt = 0;  str_to_numerical(inputStr, inputInt))
			inputObj = new Object(inputInt);
		else if (float inputFloat = 0.0f; str_to_numerical(inputStr, inputFloat))
			inputObj = new Object(inputFloat);
		else
			inputObj = new Object(std::make_shared<StringContainer>(inputStr));
		if (argVec.size() == 1) // If you pass an argument to input(), then the input value is put in that argument
			checkLval(*argVec[0] = *inputObj);
		return inputObj;
	}), "input", true);
}

