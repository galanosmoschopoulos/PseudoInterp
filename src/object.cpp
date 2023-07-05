/* object.cpp */

#include "object.h"
#include "errors.h"
#include <iostream>

/* Overload structure to access std::variant */
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
	copyArrays(array, ac2.array);
	// In the assignment constructor, we needn't run addMethods as
	// the methods are already loaded in c2
	return *this;
}


ArrayContainer::ArrayContainer(const std::vector<Object*>& listVec)
{
	for (size_t i = 0; i != listVec.size(); i++)
	{ // Initialize array with given vector of objects
		array.emplace_back(std::make_unique<Object>(*listVec[i]));
		// Unique ptr used for auto mem management
	}
}

// I.e. if dimVec = {2, 3}, it's a 2D 2x3 array
ArrayContainer::ArrayContainer(const std::vector<size_t>& dimVec)
{
	for (size_t i = 0; i != dimVec[0]; i++) // Look at the first dimension
	{
		Object* objPtr = nullptr;
		if (dimVec.size() == 1) // If only one dimension
			objPtr = new Object(); // Initialize one object
		else // If more than one
			// Element is another array of dimensions dimArray[1:] (exclude 1st)
			objPtr = new Object(
				std::make_shared<ArrayContainer>(
					std::vector(dimVec.begin() + 1, dimVec.end())));
		objPtr->setLval(true);
		array.emplace_back(objPtr);
	}
}


Object* ArrayContainer::size(const std::vector<Object*>& argVec) const
{ // Get size of the array - no arguments expected
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(static_cast<int>(array.size()));
}

void ArrayContainer::copyArrays(ArrayType& a1, const ArrayType& a2) const
{
	a1.clear();
	for (auto& element : a2)
	{
		a1.emplace_back(std::make_unique<Object>(*element));
		// Create a copy of the object, don't merely copy the pointer
		a1.back()->setLval(true); 
	}
}

Scope& ArrayContainer::getMethodScope()
{
	return methodScope;
}


void ArrayContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return size(args);
	}), "size", true); // The only method is size()
}

Object* ArrayContainer::getArray(const std::vector<Object*>& idxVec) const
{
	if (idxVec.empty()) // Index must be provided
		throw ArgumentError("At least 1 array subscript expected.");
	size_t currIdx = 0;
	// Check the type and range of the index - must be positive int!
	if (const int* idx = std::get_if<int>(&idxVec[0]->data))
	{
		if (*idx >= 0) currIdx = static_cast<size_t>(*idx);
		else throw
			ValueError("Array subscript must be a non-negative integer.");
	}
	else throw TypeError("Array subscript must be an integer.");

	if (currIdx >= array.size())
		throw RangeError("Array subscript out of range.");
	if (idxVec.size() == 1) // If it is a 1D indexd
	{
		return array[currIdx].get(); // Convert std::unique_ptr to raw pointer
	}
	// If array is more than 1D, take the subsequent the indices and run getArray()
	// on the element of the current array, which is an array too
	return (*array[currIdx])[std::vector<Object*>(
		idxVec.begin() + 1, idxVec.end())];
}

StringContainer::StringContainer() { addMethods(); }

// Similar constructors are used for the string
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
{ // Check for approriate argument number
	if (!argVec.empty()) throw ArgumentError(
		"String constructor does not take any arguments!");
	addMethods();
}

StringContainer::StringContainer(const std::string& str)
{ // Initialize a StringCOntainer with a string
	addMethods();
	for (size_t i = 0; i != str.length(); i++)
	{
		const auto objPtr = new Object(str[i]);
		objPtr->setLval(true);
		objPtr->setPersistentType(true); // String can only contain chars
		string.emplace_back(objPtr);
	}
}

void StringContainer::addMethods()
{
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return length(args);
	}), "length", true); // Returns the length of the string
}

Object* StringContainer::getChar(const std::vector<Object*>& idxVec)
{
	if (idxVec.size() != 1)
		throw ArgumentError("Exactly 1 string subscript expected.");
	size_t currIdx = 0;
	if (const int* idx = std::get_if<int>(&idxVec[0]->data))
	{ // Index must be a positive integer
		if (*idx >= 0) currIdx = static_cast<size_t>(*idx);
		else throw ValueError(
			"String subscript must be a non-negative integer.");
	}
	else throw TypeError("String subscript must be an integer.");
	if (currIdx >= string.size()) // Check index range
		throw RangeError("String subscript out of range.");
	return string[currIdx].get(); // Convert std::unique_ptr to raw pointer
}

std::string StringContainer::getStr() const
{ // Get std::string from StringContainer
	std::string str;
	for (const auto& charObj : string)
	{
		// We know that StringContainer only has char type objects
		str.push_back(std::get<char>(charObj->data));
	}
	return str;
}

Object* StringContainer::length(const std::vector<Object*>& argVec)
{ // Simply get the length of the string
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(static_cast<int>(string.size()));
}

void StringContainer::copyStrings(StringType& s1, const StringType& s2) const
{ // Similar to ArrayContainer
	s1.clear();
	for (auto& element : s2)
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
 // Constructors are similar to all others
StackContainer::StackContainer(const StackContainer& sc2)
{
	copyStacks(stack, sc2.stack);
	addMethods();
}

StackContainer& StackContainer::operator=(const StackContainer& sc2)
{
	copyStacks(stack, sc2.stack);
	return *this;
}

StackContainer::StackContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError(
		"Stack constructor does not take any arguments!");
	addMethods();
}

Object* StackContainer::push(const std::vector<Object*>& argVec)
{ // push method
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	stack.emplace(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}

Object* StackContainer::pop(const std::vector<Object*>& argVec)
{ // pop method - returns top element
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	// Throw error if empty stack
	if (stack.empty()) throw CustomError("Called pop() on empty stack.");
	const auto poppedObj = new Object(*stack.top()); // Initializes new object
	stack.pop();
	return poppedObj;
}

Object* StackContainer::isEmpty(const std::vector<Object*>& argVec) const
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	return new Object(stack.empty());
}

void StackContainer::copyStacks(StackType& stack1,
                                const StackType& stack2) const
{
	// copyStacks requires non-const arguments, even though it doesn't change them
	auto& stack2_nonconst = const_cast<StackType&>(stack2);
	// Utilize tmp stack to refill initial stack after everything has been popped
	StackType tmpStack;
	stack1 = StackType(); // reset desitnation stack
	while (!stack2_nonconst.empty())
	{
		// Create a tmp stack with reversed elements
		tmpStack.emplace(std::make_unique<Object>(*stack2_nonconst.top()));
		stack2_nonconst.pop();
	}
	while (!tmpStack.empty())
	{ // Restore initial stack and copy to destination stack
		stack2_nonconst.emplace(std::make_unique<Object>(*tmpStack.top()));
		stack1.emplace(std::make_unique<Object>(*tmpStack.top()));
		tmpStack.pop();
	}
}

Scope& StackContainer::getMethodScope()
{
	return methodScope;
}

void StackContainer::addMethods()
{ // The methods in the method scope are wrappers for the actual hardcoded methods
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return push(args);
	}), "push", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return pop(args);
	}), "pop", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return isEmpty(args);
	}), "isEmpty", true);
}


QueueContainer::QueueContainer() { addMethods(); };
// The queue implementation is very similar to the stack one
QueueContainer::QueueContainer(const QueueContainer& qc2)
{
	copyQueues(queue, qc2.queue);
	addMethods();
}

QueueContainer& QueueContainer::operator=(const QueueContainer& qc2)
{
	copyQueues(queue, qc2.queue);
	return *this;
}

QueueContainer::QueueContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError(
		"Queue constructor does not take any arguments!");
	addMethods();
}

void QueueContainer::addMethods()
{ // Again, wrappers
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return enqueue(args);
	}), "enqueue", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return dequeue(args);
	}), "dequeue", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return isEmpty(args);
	}), "isEmpty", true);
}

Object* QueueContainer::enqueue(const std::vector<Object*>& argVec)
{
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	// .emplace used since smart pointer is enqueued
	queue.emplace(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}

Object* QueueContainer::dequeue(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	// Check if the queue is empty - if yes throw error
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

void QueueContainer::copyQueues(QueueType& queue1,
                                const QueueType& queue2) const
{
	auto& queue2_nonconst = const_cast<QueueType&>(queue2);
	QueueType tmpQueue;
	queue1 = QueueType();
	// A temporary queue is used to refill the initial queue, after everything
	// has been dequeued and added to the destination queue
	while (!queue2_nonconst.empty())
	{
		// Enqueue dequeued element in both the destination and tmp queue
		queue1.emplace(std::make_unique<Object>(*queue2_nonconst.front()));
		tmpQueue.emplace(std::make_unique<Object>(*queue2_nonconst.front()));
		queue2_nonconst.pop();
	}
	while (!tmpQueue.empty())
	{ // Refill the initial queue
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
// Similar constructors for CollectionContainer
CollectionContainer::CollectionContainer(const CollectionContainer& c)
{
	copyCollections(collection, c.collection);
	addMethods();
	index = c.index; // The current index must be copied (as the index)
	// simulates the current state/position of a linked list
}

CollectionContainer& CollectionContainer::operator=(
	const CollectionContainer& c)
{
	copyCollections(collection, c.collection);
	index = c.index;
	return *this;
}

CollectionContainer::CollectionContainer(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError(
		"Collection constructor does not take any arguments!");
	addMethods();
}

void CollectionContainer::addMethods()
{ // Wrappers for all the container methods
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return addItem(args);
	}), "addItem", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return hasNext(args);
	}), "hasNext", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return getNext(args);
	}), "getNext", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return resetNext(args);
	}), "resetNext", true);
	methodScope.addObj(Object([this](const std::vector<Object*>& args)
	{
		return isEmpty(args);
	}), "isEmpty", true);
}

Object* CollectionContainer::addItem(const std::vector<Object*>& argVec)
{ // Add something in the collection
	if (argVec.size() != 1) throw ArgumentError("Exactly 1 argument expected.");
	collection.emplace_back(std::make_unique<Object>(*argVec[0]));
	return nullptr;
}

Object* CollectionContainer::getNext(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	if (index >= static_cast<int>(collection.size()) - 1) throw CustomError(
		"Collection is empty.");
	++index; // Increase index to simulate traversing a linked list
	return new Object(*collection[index]);
}

Object* CollectionContainer::resetNext(const std::vector<Object*>& argVec)
{
	if (!argVec.empty()) throw ArgumentError("No arguments expected.");
	index = -1; // Index goes to the beginning
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

void CollectionContainer::copyCollections(CollectionType& c1,
                                          const CollectionType& c2) const
{
	c1.clear(); // Reset destination collection
	auto &c2_nonconst = const_cast<CollectionType&>(c2);
	for (auto& element : c2_nonconst) // Traverse all elements of current coll.
	{ // Add each to destination
		c1.emplace_back(std::make_unique<Object>(*element));
	}
}

Scope& CollectionContainer::getMethodScope()
{
	return methodScope;
}


Function::Function(CodeBlock* block, std::vector<ASTNode*> params,
                   const int level) : block(block),
                                      paramVec(std::move(params)),
                                      definedFuncLevel(level)
{
};

Function::Function() = default;

Object* Function::eval(Scope* scope, const std::vector<Object*>& argVec) const
{
	// Check if number of parameters passed is appropriate
	if (argVec.size() != paramVec.size())
		throw ArgumentError(
			"Number of arguments not equal to number of declared parameters.");

	// Get the scope that will be passed to the function body
	Scope* newScope = scope->getRestricted(definedFuncLevel);

	// Levels must be increased (both of them)
	newScope->incLevel();
	newScope->incFuncLevel();

	Object* funcResult = nullptr; // Return value
	for (size_t i = 0; i != argVec.size(); i++)
	{ /* Create variable argument objects in function's scope, initialize them
		 with the passed argument values */
		*paramVec[i]->eval(newScope, true) = *argVec[i];
	}
	funcResult = block->eval(newScope, true); // Run block

	if (funcResult == nullptr) funcResult = new Object; /* We must avoid
	returning null pointers since it will create fatal errors */

	newScope->decrLevel(); /* Restore levels */
	newScope->decrFuncLevel();
	delete newScope; /*Destroy function's scope */
	return funcResult;
}

Object::Object() = default;

Object::Object(const Object& obj2)
{
	*this = obj2; // Copy object to another
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
	bool result = false; // Default value if object cannot be cast to bol
	std::visit(overload{
		           [&result](bool& val) { result = val; },
				   // Note that a number equal to 0 is false, else true
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
		           [&result](bool& val)
		           { // Boolean states correspond to "true" and "false" strings
			           result = (val) ? ("true") : ("false");
		           },
				   // A string containing a single char
		           [&result](char& val) { result = std::string(1, val); },
		           [&result](int& val) { result = std::to_string(val); },
		           [&result](float& val)
		           {
			           auto tmpS = std::to_string(val);
					   // Get rid of the trailing zeroes due to float precision
			           tmpS.erase(tmpS.find_last_not_of("0") + 1);
			           result = tmpS;
		           },
		           [&result](std::shared_ptr<StringContainer> sc)
		           {
			           result = sc->getStr(); // Get string of StringContainer
		           },
		           [](auto&)
		           { // A queue, for example, cannot be represented as a string
			           throw TypeError(
				           "Object does not have a string representation");
		           }
	           }, data);

	return result;
}

/* Getters/Setters */
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
