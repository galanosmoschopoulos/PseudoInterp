#pragma once
#include "object.h"
#include "parser.h"
#include "scope.h"
#include <vector>

class CodeBlock;
class Statement;
class ASTNode;
class Object;
class Scope;
enum class OperatorType;

static void cleanTmps(std::initializer_list<Object*>);
Object& checkLval(const Object& obj);


/* Everything is inside a codeblock. A codeblock contains statements. Statements may be expressions(i.e. a = 1 + foo()), return statements, if / else if / else, while, for, and function definitions
Normally, neither codeblocks nor statements should return anything. However, if we have a return statement within a function, then execution must stop and the return value must be propagated to the
function call operator. Hence, a return statement may return an object. If a codeblock executes a statement that returns something other than null pointer, it knows that a return statement has been
called, it halts execution of subsequent statements and returns the same pointer. Statements that contain codeblocks (i.e. if statement) simply return whatever the codeblock returns.

An expression is a tree of nodes that represent operators, literals and identifiers (IDs). It is built through the parsing process. The tree is traversed with post-order traversal and evaluated.
I.e. for the expression a = 5 + foo(3). Everything is an ASTNode. And operators hold pointers to those nodes.
		   =
		  / \
		 /   \
		a     +
			 / \
			/   \
		   5     ()
				/  \
			  foo   3
*/

class CodeBlock
{
public:
	CodeBlock();
	~CodeBlock();
	Object* eval(Scope*, bool isInFunction) const;
	void addStatement(Statement*);
private:
	std::vector<Statement*> statementVec{};
};

class Statement
{
public:
	Statement();
	virtual ~Statement();
	virtual Object* eval(Scope*, bool isInFunction); // Is in function is used to determine whether a return statement is valid. (return is invalid outside a function)
protected:
	size_t pos = 0; // Holds the position of the statement in the source code
};

class IfStatement final : public Statement
{
public:
	IfStatement();
	~IfStatement() override;
	explicit IfStatement(size_t);
	IfStatement(ASTNode*, CodeBlock*, size_t);
	Object* eval(Scope*, bool isInFunction) override;
	void addCase(ASTNode*, CodeBlock*); // A 'case' is a branch in an if - elif - else chain. The minimum is 2 cases (an if and an else).
private:
	std::vector<std::pair<ASTNode*, CodeBlock*>> cases{};
};

class WhileStatement final : public Statement
{
public:
	WhileStatement();
	~WhileStatement() override;
	WhileStatement(ASTNode*, CodeBlock*, size_t);
	Object* eval(Scope*, bool isInFunction) override;
private:
	ASTNode* condition = nullptr;
	CodeBlock* block = nullptr;
};

class ForStatement final : public Statement
{
public:
	ForStatement();
	~ForStatement() override;
	ForStatement(ASTNode*, ASTNode*, ASTNode*, CodeBlock*, size_t);
	Object* eval(Scope*, bool isInFunction) override;
private:
	ASTNode* counterNode = nullptr;
	ASTNode* lowerNode = nullptr; // Refers to the lower limit of a for range
	ASTNode* upperNode = nullptr; // Upper limit
	CodeBlock* block = nullptr;
};

class ExprStatement final : public Statement
{
public:
	ExprStatement();
	~ExprStatement() override;
	ExprStatement(ASTNode*, size_t);
	Object* eval(Scope*, bool isInFunction) override;
private:
	ASTNode* exprRoot = nullptr;
};

class ReturnStatement final : public Statement
{
public:
	ReturnStatement();
	~ReturnStatement() override;
	ReturnStatement(ASTNode*, size_t);
	Object* eval(Scope*, bool isInFunction) override;
private:
	ASTNode* returnRoot = nullptr;
};

class FunctionDefStatement final : public Statement
{
public:
	FunctionDefStatement();
	~FunctionDefStatement() override;
	FunctionDefStatement(ASTNode*, std::vector<ASTNode*>, CodeBlock*, size_t);
	Object* eval(Scope*, bool) override;
private:
	ASTNode* funcID = nullptr; // An ID node used to name the function
	std::vector<ASTNode*> funcParams{}; // ID nodes - the parameters
	CodeBlock* block = nullptr;
};


class ASTNode
{
public:
	ASTNode();
	virtual ~ASTNode();
	virtual Object* eval(Scope*, bool lSide = false);
	void setForceRval(bool);
protected:
	bool forceRval = false;
	size_t pos = 0;
};

class nAryNode final : public ASTNode // For function call operator and subscript operator
{
public:
	nAryNode();
	~nAryNode() override;
	nAryNode(ASTNode*, OperatorType, std::vector<ASTNode*>, size_t);
	Object* eval(Scope* scope, bool lSide = false) override;
private:
	OperatorType opType = OperatorType::UNKNOWN;
	// I.e. in the expression foo(a, b), foo is the main operand and a, b go in nOperands
	ASTNode* mainOperand = nullptr;
	std::vector<ASTNode*> nOperands{};
};

class BinaryNode final : public ASTNode // For binary operators
{
public:
	BinaryNode();
	~BinaryNode() override;
	BinaryNode(ASTNode*, ASTNode*, OperatorType, size_t);
	Object* eval(Scope*, bool lSide = false) override;
private:
	OperatorType opType = OperatorType::UNKNOWN;
	ASTNode* left = nullptr;
	ASTNode* right = nullptr;
};


class LiteralNode final : public ASTNode // For literals
{
public:
	LiteralNode();
	~LiteralNode() override;

	explicit LiteralNode(auto val, const size_t position) : literal(new Object(val))
	{
		pos = position;
	}

	Object* eval(Scope*, bool lSide = false) override;
private:
	Object* literal = nullptr; // This object is set during the parsing
};

class IDNode final : public ASTNode // For identifiers
{
public:
	IDNode();
	~IDNode() override;
	IDNode(std::string, size_t);
	Object* eval(Scope*, bool lSide = false) override;
private:
	std::string id;
};

class UnaryNode final : public ASTNode // For unary operators
{
public:
	UnaryNode();
	~UnaryNode() override;
	UnaryNode(ASTNode*, OperatorType, size_t);
	Object* eval(Scope*, bool lSide = false) override;
private:
	OperatorType opType = OperatorType::UNKNOWN;
	ASTNode* operand = nullptr;
};
