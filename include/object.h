/* object.h */

#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <variant>
#include <stack>
#include <queue>
#include "AST.h"
#include "scope.h"

class CollectionContainer;
class QueueContainer;
class Object;
class Function;
class Scope;
class ArrayContainer;
class StackContainer;
class StringContainer;
using ExternalFunction = std::function<Object*(const std::vector<Object*>&)>;
// Tagged union used to hold all possible types of object
using VariantType = std::variant<
	int, std::shared_ptr<StringContainer>, bool, float, char,
	std::shared_ptr<ArrayContainer>, std::shared_ptr<StackContainer>,
	std::shared_ptr<QueueContainer>, std::shared_ptr<CollectionContainer>,
	Function, ExternalFunction>;
// Shared ptr used for automatic memory deallocation at deletion

class ASTNode;
class CodeBlock;

class ArrayContainer
{
public:
	// Container types internally are containers i.e. vector with smart vectors
	// pointing to objects. Smart vectors ensure memory dealocation for the
	// complex recursive std::variant type
	using ArrayType = std::vector<std::unique_ptr<Object>>;
	ArrayContainer();
	// Different assignment & copy contructors
	ArrayContainer(const ArrayContainer&);
	ArrayContainer& operator=(const ArrayContainer&);
	explicit ArrayContainer(const std::vector<Object*>&);
	ArrayContainer(const std::vector<size_t>&);
	// Get the object in a specific index
	[[nodiscard]] Object* getArray(const std::vector<Object*>&) const;
	[[nodiscard]] Object* size(const std::vector<Object*>&) const; // Get # of elements
	void copyArrays(ArrayType& a1, const ArrayType& a2) const;
	Scope& getMethodScope(); // Get a scope with all the hardcoded methods of
	// the array. 
private:
	void addMethods(); // Adds hardcoded methods to methodScope
	ArrayType array;
	Scope methodScope;
};

class StringContainer
{ // Similar to ArrayContainer, but only 1D and can contain only char types
public:
	using StringType = std::vector<std::unique_ptr<Object>>; // Like ArrayContainer
	StringContainer();
	StringContainer(const StringContainer&);
	StringContainer& operator=(const StringContainer&);
	explicit StringContainer(const std::vector<Object*>&);
	explicit StringContainer(const std::string&);
	[[nodiscard]] Object* getChar(const std::vector<Object*>&);
	[[nodiscard]] std::string getStr() const;
	Object* length(const std::vector<Object*>&);
	void copyStrings(StringType&, const StringType&) const;
	Scope& getMethodScope();
private:
	void addMethods();
	Scope methodScope;
	StringType string;
};

class StackContainer
{
public:
	using StackType = std::stack<std::unique_ptr<Object>>;
	// std::stack is the underlying implementation
	// contructors are similar in all containers
	StackContainer();
	StackContainer(const StackContainer& sc2);
	StackContainer& operator=(const StackContainer& sc2);
	explicit StackContainer(const std::vector<Object*>&);
	// Stack's methods are push, pop and isEmpty
	[[nodiscard]] Object* push(const std::vector<Object*>&);
	[[nodiscard]] Object* pop(const std::vector<Object*>&);
	[[nodiscard]] Object* isEmpty(const std::vector<Object*>& argVec) const;
	void copyStacks(StackType& stack1, const StackType& stack2) const;
	Scope& getMethodScope();
private:
	void addMethods();
	StackType stack;
	Scope methodScope;
};

class QueueContainer
{
public:
	using QueueType = std::queue<std::unique_ptr<Object>>;
	// std::queue is the underlying implementation
	QueueContainer();
	QueueContainer(const QueueContainer&);
	QueueContainer& operator=(const QueueContainer&);
	explicit QueueContainer(const std::vector<Object*>&);
	[[nodiscard]] Object* enqueue(const std::vector<Object*>&);
	[[nodiscard]] Object* dequeue(const std::vector<Object*>&);
	[[nodiscard]] Object* isEmpty(const std::vector<Object*>& argVec) const;
	void copyQueues(QueueType&, const QueueType&) const;
	Scope& getMethodScope();
private:
	void addMethods();
	QueueType queue;
	Scope methodScope;
};

class CollectionContainer
{
public:
	using CollectionType = std::vector<std::unique_ptr<Object>>;
	// The Collection is essentially a linked list. This is simulated with an
	// array and an index used to hold current node.
	CollectionContainer();
	CollectionContainer(const CollectionContainer&);
	CollectionContainer& operator=(const CollectionContainer&);
	explicit CollectionContainer(const std::vector<Object*>&);
	[[nodiscard]] Object* addItem(const std::vector<Object*>&);
	[[nodiscard]] Object* getNext(const std::vector<Object*>&);
	[[nodiscard]] Object* resetNext(const std::vector<Object*>& argVec);
	[[nodiscard]] Object* hasNext(const std::vector<Object*>& argVec) const;
	[[nodiscard]] Object* isEmpty(const std::vector<Object*>& argVec) const;
	void copyCollections(CollectionType&, const CollectionType&) const;
	Scope& getMethodScope();
private:
	void addMethods();
	int index = -1; // Index to simulate linked list
	CollectionType collection;
	Scope methodScope;
};

class Function // A user defined function
{
public:
	Function();
	Function(CodeBlock*, std::vector<ASTNode*>, int);
	// argVec contains the passed arguments - objects
	Object* eval(Scope* scope, const std::vector<Object*>& argVec) const;
private:
	CodeBlock* block = nullptr;
	std::vector<ASTNode*> paramVec{};
	int definedFuncLevel = 0;
};

class Object
{
public:
	Object();
	Object(const Object& obj2);
	explicit Object(const auto& val) { data = val; } // Init. directly with value
	[[nodiscard]] bool isLval() const;
	void setLval(bool isIt);
	VariantType data; // The tagged union

	// All operators are overloaded and defined in operators.cpp
	Object& operator=(const Object&);
	Object& operator+=(Object&);
	friend Object operator+(Object, Object&);
	Object& operator-=(Object&);
	friend Object operator-(Object, Object&);
	Object& operator*=(Object&);
	friend Object operator*(Object, Object&);
	Object& operator/=(Object&);
	friend Object operator/(Object, Object&);
	Object& operator%=(Object&);
	friend Object operator%(Object, Object&);
	Object& operatorDivEq(Object&); // There no 'div' operator in C++    :'(
	friend Object operatorDiv(Object, Object&);
	Object& operator++();
	Object& operator--();
	Object operator++(int);
	Object operator--(int);
	Object operator-() const;
	Object operator+() const;
	Object operator!();
	friend Object operator<(Object&, Object&);
	friend Object operator>(Object&, Object&);
	friend Object operator<=(Object&, Object&);
	friend Object operator>=(Object&, Object&);
	friend Object operator==(Object&, Object&);
	friend Object operator!=(Object&, Object&);
	friend Object operator||(Object&, Object&);
	friend Object operator&&(Object&, Object&);
	Object* operator()(Scope*, const std::vector<Object*>&);
	Object* operator[](const std::vector<Object*>&);
	bool isTrue(); // If the object can be evaluated as true or false
	std::string toStr(); // Get string representation if possible
	[[nodiscard]] bool isPersistentType() const; // Can the type be changed?
	[[nodiscard]] bool isConst() const;
	void setConst(bool isIt);
	void setPersistentType(bool);
private:
	bool lval = false;
	bool persistentType = false; /* I.e. the objects of a string contaner are
							always chars*/
	bool constness = false;
};
