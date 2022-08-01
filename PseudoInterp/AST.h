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
	virtual Object* eval(Scope*, bool isInFunction);
protected:
	size_t pos = 0;
};

class IfStatement final : public Statement
{
public:
	IfStatement();
	~IfStatement() override;
	explicit IfStatement(size_t);
	IfStatement(ASTNode*, CodeBlock*, size_t);
	Object* eval(Scope*, bool isInFunction) override;
	void addCase(ASTNode*, CodeBlock*);
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
	ASTNode* lowerNode = nullptr;
	ASTNode* upperNode = nullptr;
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
	ASTNode* funcID = nullptr;
	std::vector<ASTNode*> funcParams{};
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

class nAryNode final : public ASTNode
{
public:
	nAryNode();
	~nAryNode() override;
	nAryNode(ASTNode*, OperatorType, std::vector<ASTNode*>, size_t);
	Object* eval(Scope* scope, bool lSide = false) override;
private:
	OperatorType opType = OperatorType::UNKNOWN;
	ASTNode* mainOperand = nullptr;
	std::vector<ASTNode*> nOperands{};
};

class BinaryNode final : public ASTNode
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


class LiteralNode final : public ASTNode
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
	Object* literal = nullptr;
};

class IDNode final : public ASTNode
{
public:
	IDNode();
	~IDNode() override;
	IDNode(std::string, size_t);
	Object* eval(Scope*, bool lSide = false) override;
	std::string getID();
private:
	std::string id;
};

class UnaryNode final : public ASTNode
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
