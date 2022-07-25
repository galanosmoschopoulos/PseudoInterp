#include "scope.h"

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
		throw std::runtime_error("Scope level cannot be negative.");
	}
	std::vector<ObjKey> toBeDeleted;
	for (auto itr = scopeMap.rbegin(); itr != scopeMap.rend(); ++itr)
	{
		if (itr->first.scopeLevel == scopeLevel)
		{
			toBeDeleted.push_back(itr->first);
		}
	}
	for (const auto& x : toBeDeleted)
	{
		scopeMap.erase(x);
	}
	scopeLevel--;
}

void Scope::addObj(const Object& obj, const std::string& id)
{
	scopeMap[ObjKey(scopeLevel, funcLevel, id)] = new Object(obj);
	scopeMap[ObjKey(scopeLevel, funcLevel, id)]->setLval(true);
}

Object* Scope::getObj(const std::string& id)
{
	for (auto itr = scopeMap.rbegin(); itr != scopeMap.rend(); ++itr)
	{
		if (itr->first.ID == id)
		{
			return itr->second;
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
	for (auto itr = scopeMap.begin(); itr != scopeMap.end(); ++itr)
	{
		if (itr->first.funcLevel <= maxFuncLevel)
			newMap.insert(*itr);
		else
			break;
	}
	newScope->setFuncLevel(newMap.rbegin()->first.funcLevel);
	newScope->setScopeLevel(newMap.rbegin()->first.scopeLevel);
	return newScope;
}

void Scope::printScope() const
{
	for (std::pair<ObjKey, Object*> x : scopeMap)
	{
		std::cout << "[ID: " << x.first.ID << ", Func Level: " << x.first.funcLevel << ", Scope Level: " << x.first.scopeLevel << "]\n";
	}
}
