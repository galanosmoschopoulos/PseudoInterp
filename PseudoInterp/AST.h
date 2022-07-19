#pragma once
#include "objects.h"
#include "parser.h"
#include <stdexcept>
#include <vector>
#include <initializer_list>

class CodeBlock;
class Statement;
class ASTNode;
class Object;
class Scope;
enum class OperatorType;

static void cleanTmps(std::initializer_list<Object*>);
inline static Object& checkLval(const Object& obj);

class CodeBlock
{
public:
    CodeBlock();
    ~CodeBlock();
    Object* eval(Scope*, bool isInFunction);
    void addStatement(Statement*);
private:
    std::vector<Statement*> statementVec;
};

class Statement
{
public:
    Statement();
    virtual ~Statement();
    virtual Object* eval(Scope*, bool isInFunction);
};

class IfStatement final : public Statement
{
public:
    IfStatement();
    ~IfStatement();
    IfStatement(ASTNode*, CodeBlock*);
    Object* eval(Scope*, bool isInFunction) override;
    void addCase(ASTNode*, CodeBlock*);
private:
    std::vector<std::pair<ASTNode*, CodeBlock*>> cases;
};

class WhileStatement final : public Statement
{
public:
    WhileStatement();
    ~WhileStatement();
    WhileStatement(ASTNode*, CodeBlock*);
    Object* eval(Scope*, bool isInFunction) override;
private:
    ASTNode* condition = nullptr;
    CodeBlock* block = nullptr;
};

class ForStatement final : public Statement
{
public:
    ForStatement();
    ~ForStatement();
    ForStatement(ASTNode*, ASTNode*, ASTNode*, CodeBlock*);
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
    ~ExprStatement();
    ExprStatement(ASTNode*);
    Object* eval(Scope*, bool isInFunction) override;
private:
    ASTNode* exprRoot = nullptr;
};

class ReturnStatement final : public Statement
{
public:
    ReturnStatement();
    ~ReturnStatement();
    ReturnStatement(ASTNode*);
    Object* eval(Scope*, bool isInFunction) override;
private:
    ASTNode* returnRoot = nullptr;
};

class FunctionDefStatement final : public Statement
{
public:
    FunctionDefStatement();
    ~FunctionDefStatement();
    FunctionDefStatement(ASTNode*, const std::vector<ASTNode*>&, CodeBlock*);
    Object* eval(Scope*, bool) override;
private:
    ASTNode* funcID = nullptr;
    std::vector<ASTNode*> funcParams;
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
};

class nAryNode final : public ASTNode
{
public:
    nAryNode();
    ~nAryNode();
    nAryNode(ASTNode*, OperatorType, std::vector<ASTNode*>&);
    Object* eval(Scope* scope, bool lSide = false) override;
private:
    OperatorType opType = OperatorType::UNKNOWN;
    ASTNode* mainOperand = nullptr;
    std::vector<ASTNode*> nOperands;
};

class BinaryNode final : public ASTNode
{
public:
    BinaryNode();
    ~BinaryNode();
    BinaryNode(ASTNode*, ASTNode*, OperatorType);
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
    ~LiteralNode();
    LiteralNode(auto val) : literal(new Object(val)) { }
    Object* eval(Scope*, bool lSide = false) override;
private:
    Object *literal = nullptr;
};

class IDNode final : public ASTNode
{
public:
    IDNode();
    ~IDNode();
    IDNode(const std::string&);
    Object* eval(Scope*, bool lSide = false);
private:
    std::string id = "";
};

class UnaryNode final : public ASTNode
{
public:
    UnaryNode();
    ~UnaryNode();
    UnaryNode(ASTNode*, OperatorType);
    Object* eval(Scope*, bool lSide = false) override;
private:
    Object& outputOp(Object& obj);
    OperatorType opType = OperatorType::UNKNOWN;
    ASTNode* operand = nullptr;
};

