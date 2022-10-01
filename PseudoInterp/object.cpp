#include "object.h"
#include "errors.h"
#include <iostream>

template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};

ArrayContainer::ArrayContainer() { addMethods(); }

ArrayContainer::ArrayContainer(const ArrayContainer& ac)
{
	copyArrays(array, ac.array);
	addMethods();
}

ArrayContainer& ArrayContainer::operator=(const ArrayContainer& ac2)
{
	copyArrays(array,ac2.array);
	return *this;
}

ArrayContainer::ArrayContainer(const std::vector<Object*>& dimVec)
{
	if (dimVec.empty()) throw ArgumentError("At least 1 array size parameter expected.");
	size_t currSize = 0;
	if (const int* size = std::get_if<int>(&dimVec[0]->data))
	{
		if (*size > 0) currSize = static_cast<size_t>(*size);
		else throw ValueError("Array size parameter must be a positive integer.");
	}
	else throw TypeError("Array size parameter must be an integer.");
	for (size_t i = 0; i != currSize; i++)
	{
		Object* objPtr = nullptr;
		if(dimVec.size() == 1)
			objPtr = new Object();
		else
			objPtr = new Object(std::make_shared<ArrayContainer>(std::vector(dimVec.begin() + 1, dimVec.end())));
		objPtr->setLval(true);
		array.emplace_back(objPtr);
	}
}

Object* ArrayContainer::size(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(static_cast<int>(array.size()));
}

void ArrayContainer::copyArrays(ArrayType& a1, const ArrayType& a2) const
{
	a1.clear();
	for(auto& element : a2)
	{
		a1.emplace_back(std::make_unique<Object>(*element));
		a1.back()->setLval(true);
	}
}

Scope& ArrayContainer::getMethodScope()
{
	return methodScope;
}

void ArrayContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return size(args); }), "size", true);
}

Object* ArrayContainer::getArray(const std::vector<Object*>& idxVec) const
{
	if (idxVec.empty())
		throw ArgumentError("At least 1 array subscript expected.");
	size_t currIdx = 0;
	if (const int* idx = std::get_if<int>(&idxVec[0]->data))
	{
		if (*idx >= 0) currIdx = static_cast<size_t>(*idx);
		else throw ValueError("Array subscript must be a non-negative integer.");
	}
	else throw TypeError("Array subscript must be an integer.");

	if (currIdx >= array.size())
		throw RangeError("Array subscript out of range.");
	if (idxVec.size() == 1)
		return array[currIdx].get(); // Convert std::unique_ptr to raw pointer

	return (*array[currIdx])[std::vector<Object*>(idxVec.begin() + 1, idxVec.end())];
}

StringContainer::StringContainer() { addMethods(); }

StringContainer::StringContainer(const StringContainer& sc2)
{
	copyStrings(string, sc2.string);
	addMethods();
}
StringContainer& StringContainer::operator=(const StringContainer& sc2)
{
	copyStrings(string, sc2.string);
	return *this;
}
StringContainer::StringContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("Stack constructor does not take any arguments!");
	addMethods();
}

StringContainer::StringContainer(const std::string& str)
{
	addMethods();
	for (size_t i = 0; i != str.length(); i++)
	{
		const auto objPtr = new Object(str[i]);
		objPtr->setLval(true);
		objPtr->setPersistentType(true);
		string.emplace_back(objPtr);
	}
}

void StringContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return length(args); }), "length", true);
}

Object* StringContainer::getChar(const std::vector<Object*>& idxVec)
{
	if (idxVec.size() != 1)
		throw ArgumentError("Exactly 1 string subscript expected.");
	size_t currIdx = 0;
	if (const int* idx = std::get_if<int>(&idxVec[0]->data))
	{
		if (*idx >= 0) currIdx = static_cast<size_t>(*idx);
		else throw ValueError("String subscript must be a non-negative integer.");
	}
	else throw TypeError("String subscript must be an integer.");
	if (currIdx >= string.size())
		throw RangeError("String subscript out of range.");
	return string[currIdx].get(); // Convert std::unique_ptr to raw pointer
}

std::string StringContainer::getStr() const
{
	std::string str;
	for (const auto& charObj : string)
	{
		str.push_back(std::get<char>(charObj->data));
	}
	return str;
}

Object* StringContainer::length(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(static_cast<int>(string.size()));
}

void StringContainer::copyStrings(StringType& s1, const StringType& s2) const
{
	s1.clear();
	for(auto& element : s2)
	{
		s1.emplace_back(std::make_unique<Object>(*element));
		s1.back()->setLval(true);
		s1.back()->setPersistentType(true);
	}
}

Scope& StringContainer::getMethodScope()
{
	return methodScope;
}


StackContainer::StackContainer() { addMethods(); }
StackContainer::StackContainer(const StackContainer& sc2)
{
	auto& sc2_nonconst = const_cast<StackContainer&>(sc2);
	copyStacks(stack, sc2_nonconst.stack);
	addMethods();
}
StackContainer& StackContainer::operator=(const StackContainer& sc2)
{
	auto& sc2_nonconst = const_cast<StackContainer&>(sc2);
	copyStacks(stack, sc2_nonconst.stack);
	return *this;
}
StackContainer::StackContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("Stack constructor does not take any arguments!");
	addMethods();
}
Object* StackContainer::push(const std::vector<Object*>& argVec)
{
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	stack.emplace(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}
Object* StackContainer::pop(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	if (stack.empty()) throw CustomError("Called pop() on empty stack.");
	const auto poppedObj = new Object(*stack.top());
	stack.pop();
	return poppedObj;
}
Object* StackContainer::isEmpty(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(stack.empty());
}
void StackContainer::copyStacks(StackType& stack1, const StackType& stack2) const
{
	auto& stack2_nonconst = const_cast<StackType&>(stack2);
	StackType tmpStack;
	stack1 = StackType();
	while (!stack2_nonconst.empty())
	{
		stack1.emplace(std::make_unique<Object>(*stack2_nonconst.top()));
		tmpStack.emplace(std::make_unique<Object>(*stack2_nonconst.top()));
		stack2_nonconst.pop();
	}
	while (!tmpStack.empty())
	{
		stack2_nonconst.emplace(std::make_unique<Object>(*tmpStack.top()));
		tmpStack.pop();
	}
}
Scope& StackContainer::getMethodScope()
{
	return methodScope;
}
void StackContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return push(args); }), "push", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return pop(args); }), "pop", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return isEmpty(args); }), "isEmpty", true);
}


QueueContainer::QueueContainer() { addMethods(); };
QueueContainer::QueueContainer(const QueueContainer& qc2)
{
	auto& qc2_nonconst = const_cast<QueueContainer&>(qc2);
	copyQueues(queue, qc2_nonconst.queue);
	addMethods();
}
QueueContainer& QueueContainer::operator=(const QueueContainer& qc2)
{
	auto& qc2_nonconst = const_cast<QueueContainer&>(qc2);
	copyQueues(queue, qc2_nonconst.queue);
	return *this;
}

QueueContainer::QueueContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("Queue constructor does not take any arguments!");
	addMethods();
}
void QueueContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return enqueue(args); }), "enqueue", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return dequeue(args); }), "dequeue", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return isEmpty(args); }), "isEmpty", true);
}
Object* QueueContainer::enqueue(const std::vector<Object*>& argVec)
{
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	queue.emplace(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}
Object* QueueContainer::dequeue(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	if (queue.empty()) throw CustomError("Called dequeue() on empty queue.");
	const auto poppedObj = new Object(*queue.front());
	queue.pop();
	return poppedObj;
}
Object* QueueContainer::isEmpty(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(queue.empty());
}
void QueueContainer::copyQueues(QueueType& queue1, const QueueType& queue2) const
{
	auto& queue2_nonconst = const_cast<QueueType&>(queue2);
	QueueType tmpQueue;
	queue1 = QueueType();
	while (!queue2_nonconst.empty())
	{
		queue1.emplace(std::make_unique<Object>(*queue2_nonconst.front()));
		tmpQueue.emplace(std::make_unique<Object>(*queue2_nonconst.front()));
		queue2_nonconst.pop();
	}
	while (!tmpQueue.empty())
	{
		queue2_nonconst.emplace(std::make_unique<Object>(*tmpQueue.front()));
		tmpQueue.pop();
	}
}
Scope& QueueContainer::getMethodScope()
{
	return methodScope;
}


CollectionContainer::CollectionContainer()
{
	addMethods();
}
CollectionContainer::CollectionContainer(const CollectionContainer& c)
{
	auto& c_nonconst = const_cast<CollectionContainer&>(c);
	copyCollections(collection, c_nonconst.collection);
	addMethods();
	index = c.index;
}
CollectionContainer& CollectionContainer::operator=(const CollectionContainer& c)
{
	auto& c_nonconst = const_cast<CollectionContainer&>(c);
	copyCollections(collection, c_nonconst.collection);
	index = c.index;
	return *this;
}
CollectionContainer::CollectionContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("Collection constructor does not take any arguments!");
	addMethods();
}
void CollectionContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return addItem(args); }), "addItem", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return hasNext(args); }), "hasNext", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return getNext(args); }), "getNext", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return resetNext(args); }), "resetNext", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args) { return isEmpty(args); }), "isEmpty", true);
}
Object* CollectionContainer::addItem(const std::vector<Object*>& argVec)
{
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	collection.emplace_back(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}

Object* CollectionContainer::getNext(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	if (index >= static_cast<int>(collection.size()) - 1) throw CustomError("Collection is empty.");
	++index;
	return new Object(*collection[index]);
}
Object* CollectionContainer::resetNext(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	index = -1;
	return nullptr;
}
Object* CollectionContainer::hasNext(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(index < static_cast<int>(collection.size()) - 1);
}
Object* CollectionContainer::isEmpty(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(collection.empty());
}
void CollectionContainer::copyCollections(CollectionType& c1, const CollectionType& c2) const
{
	c1.clear();
	for(auto& element : c2)
	{
		c1.emplace_back(std::make_unique<Object>(*element));
	}
	
}
Scope& CollectionContainer::getMethodScope()
{
	return methodScope;
}


Function::Function(CodeBlock* block, std::vector<ASTNode*> params, const int level) : block(block),
                                                                                      paramVec(std::move(params)), definedFuncLevel(level)
{
};

Function::Function() = default;

Object* Function::eval(Scope* scope, const std::vector<Object*>& argVec) const
{
	if (argVec.size() != paramVec.size())
		throw ArgumentError("Number of arguments not equal to number of declared parameters.");

	Scope* newScope = scope->getRestricted(definedFuncLevel);

	newScope->incLevel();
	newScope->incFuncLevel();

	Object* funcResult = nullptr;
	for (size_t i = 0; i != argVec.size(); i++)
	{
		*paramVec[i]->eval(newScope, true) = *argVec[i];
	}
	funcResult = block->eval(newScope, true);

	if (funcResult == nullptr) funcResult = new Object;

	newScope->decrLevel();
	newScope->decrFuncLevel();
	delete newScope;
	return funcResult;
}

Object::Object() = default;

Object::Object(const Object& obj2)
{
	*this = obj2;
}

bool Object::isLval() const
{
	return lval;
}

void Object::setLval(const bool isIt)
{
	lval = isIt;
}

bool Object::isTrue()
{
	bool result = false;
	std::visit(overload{
		           [&result](bool& val) { result = val; },
		           [&result](char& val) { result = static_cast<bool>(val); },
		           [&result](int& val) { result = static_cast<bool>(val); },
		           [&result](float& val) { result = static_cast<bool>(val); },
		           [](auto&)
		           {
		           }
	           }, data);
	return result;
}

std::string Object::toStr()
{
	std::string result;
	std::visit(overload{
		           [&result](bool& val) { result = (val) ? ("true") : ("false"); },
		           [&result](char& val) { result = std::string(1, val); },
		           [&result](int& val) { result = std::to_string(val); },
		           [&result](float& val)
		           {
					   auto tmpS = std::to_string(val);
					   tmpS.erase(tmpS.find_last_not_of("0") + 1);
					   result = tmpS;
		           },
		           [&result](std::shared_ptr<StringContainer> sc) { result = sc->getStr(); },
		           [](auto&) { throw TypeError("Object does not have a string representation"); }
	           }, data);
	return result;
}

bool Object::isPersistentType() const
{
	return persistentType;
}

bool Object::isConst() const
{
	return constness;
}

void Object::setConst(bool isIt)
{
	constness = isIt;
}

void Object::setPersistentType(const bool isIt)
{
	persistentType = isIt;
}
