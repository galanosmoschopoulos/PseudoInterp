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
	for (auto& itr : std::ranges::reverse_view(scopeMap))
	{
		if (itr.first.scopeLevel == scopeLevel)
		{
			toBeDeleted.push_back(itr.first);
		}
	}
	for (const auto& x : toBeDeleted)
	{
		scopeMap.erase(x);
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
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = obj;
	obj->setLval(true);
	obj->setConst(isConst);
}

Object* Scope::getObj(const std::string& id)
{
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

void Scope::enableExternalFunctions()
{
	addObj(Object([](const std::vector<Object*>& argVec) { return new Object(ArrayContainer(argVec)); }), "Array", true);
	addObj(Object([](const std::vector<Object*>& argVec) { return new Object(std::make_shared<StackContainer>(argVec)); }), "Stack", true);
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
			bool isNum = true;
			int inputNum = 0;
			size_t pos = 0;
			try
			{
				inputNum = std::stoi(inputStr, &pos);
			}
			catch (std::invalid_argument&)
			{
				isNum = false;
			}
			catch (std::out_of_range&)
			{
				isNum = false;
			}
			if (pos != inputStr.length()) isNum = false;

			if (isNum) inputObj = new Object(inputNum);
			else inputObj = new Object(StringContainer(inputStr));
			if (argVec.size() == 1)
				checkLval(*argVec[0] = *inputObj);
			return inputObj;
		}), "input", true);
}

void Scope::printScope() const
{
	for (std::pair<ObjKey, Object*> x : scopeMap)
	{
		std::cout << "[ID: " << x.first.ID << ", Func Level: " << x.first.funcLevel << ", Scope Level: " << x.first.
			scopeLevel << "]\n";
	}
}
