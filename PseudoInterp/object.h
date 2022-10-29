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
class VariantValueType;
using ExternalFunction = std::function<Object*(const std::vector<Object*>&)>;
using VariantType = std::variant<int, std::shared_ptr<StringContainer>, bool, float, char,
								 std::shared_ptr<ArrayContainer>,std::shared_ptr<StackContainer>,
								 std::shared_ptr<QueueContainer>, std::shared_ptr<CollectionContainer>,
								 Function, ExternalFunction>;

class ASTNode;
class CodeBlock;

class ArrayContainer
{
public:
	using ArrayType = std::vector<std::unique_ptr<Object>>;
	ArrayContainer();
	ArrayContainer(const ArrayContainer&);
	ArrayContainer& operator=(const ArrayContainer&);
	explicit ArrayContainer(const std::vector<Object*>&);
	ArrayContainer(const std::vector<size_t>&);
	void addMethods();
	[[nodiscard]] Object* getArray(const std::vector<Object*>&) const;
	[[nodiscard]] Object* size(const std::vector<Object*>&) const;
	void copyArrays(ArrayType& a1, const ArrayType& a2) const;
	Scope& getMethodScope();
private:
	//bool isTempArray = false;
	ArrayType array;
	Scope methodScope;
};

class StringContainer
{
public:
	using StringType = std::vector<std::unique_ptr<Object>>;
	StringContainer();
	StringContainer(const StringContainer&);
	StringContainer& operator=(const StringContainer&);
	explicit StringContainer(const std::vector<Object*>&);
	explicit StringContainer(const std::string&);
	void addMethods();
	[[nodiscard]] Object* getChar(const std::vector<Object*>&);
	[[nodiscard]] std::string getStr() const;
	Object* length(const std::vector<Object*>&);
	void copyStrings(StringType&, const StringType&) const;
	Scope& getMethodScope();
private:
	Scope methodScope;
	StringType string;
};

class StackContainer
{
public:
	using StackType = std::stack<std::unique_ptr<Object>>;
	StackContainer();
	StackContainer(const StackContainer& sc2);
	StackContainer& operator=(const StackContainer& sc2);
	explicit StackContainer(const std::vector<Object*>&);
	void addMethods();
	[[nodiscard]] Object* push(const std::vector<Object*>&);
	[[nodiscard]] Object* pop(const std::vector<Object*>&);
	[[nodiscard]] Object* isEmpty(const std::vector<Object*>& argVec) const;
	void copyStacks(StackType& stack1, const StackType& stack2) const;
	Scope& getMethodScope();
private:
	StackType stack;
	Scope methodScope;
};
class QueueContainer
{
public:
	using QueueType = std::queue<std::unique_ptr<Object>>;
	QueueContainer();
	QueueContainer(const QueueContainer&);
	QueueContainer& operator=(const QueueContainer&);
	explicit QueueContainer(const std::vector<Object*>&);
	void addMethods();
	[[nodiscard]] Object* enqueue(const std::vector<Object*>&);
	[[nodiscard]] Object* dequeue(const std::vector<Object*>&);
	[[nodiscard]] Object* isEmpty(const std::vector<Object*>& argVec) const;
	void copyQueues(QueueType&, const QueueType&) const;
	Scope& getMethodScope();
private:
	QueueType queue;
	Scope methodScope;
};
class CollectionContainer
{
public:
	using CollectionType = std::vector<std::unique_ptr<Object>>;
	CollectionContainer();
	CollectionContainer(const CollectionContainer&);
	CollectionContainer& operator=(const CollectionContainer&);
	explicit CollectionContainer(const std::vector<Object*>&);
	void addMethods();
	[[nodiscard]] Object* addItem(const std::vector<Object*>&);
	[[nodiscard]] Object* getNext(const std::vector<Object*>&);
	[[nodiscard]] Object* resetNext(const std::vector<Object*>& argVec);
	[[nodiscard]] Object* hasNext(const std::vector<Object*>& argVec) const;
	[[nodiscard]] Object* isEmpty(const std::vector<Object*>& argVec) const;
	void copyCollections(CollectionType&, const CollectionType&) const;
	Scope& getMethodScope();
private:
	int index = -1;
	CollectionType collection;
	Scope methodScope;
};

class Function
{
public:
	Function();
	Function(CodeBlock*, std::vector<ASTNode*>, int);
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
	explicit Object(const auto& val) { data = val; }
	[[nodiscard]] bool isLval() const;
	void setLval(bool isIt);
	VariantType data;

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
	bool isTrue();
	std::string toStr();
	[[nodiscard]] bool isPersistentType() const;
	[[nodiscard]] bool isConst() const;
	void setConst(bool isIt);
	void setPersistentType(bool);
private:
	bool lval = false;
	bool persistentType = false;
	bool constness = false;
};
