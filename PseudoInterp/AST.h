#pragma once
#include "objects.h"
#include "parser.h"
#include <stdexcept>
#include <vector>
#include <initializer_list>

static void cleanTmps(std::initializer_list<Object*>);
inline static Object& checkLval(Object& obj);

class CodeBlock;
class Statement;
class ASTNode;

class CodeBlock
{
public:
    CodeBlock();
    ~CodeBlock();
    void eval(Scope*);
    void addStatement(Statement*);
private:
    std::vector<Statement*> statementVec;
};

class Statement
{
public:
    Statement();
    virtual ~Statement();
    virtual void eval(Scope*);
};

class IfStatement final : public Statement
{
public:
    IfStatement();
    ~IfStatement();
    IfStatement(ASTNode*, CodeBlock*);
    void eval(Scope*) override;
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
    void eval(Scope*) override;
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
    void eval(Scope*) override;
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
    void eval(Scope*) override;
private:
    ASTNode* exprRoot = nullptr;
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

class BinaryNode final : public ASTNode
{
public:
    BinaryNode();
    ~BinaryNode();
    BinaryNode(ASTNode*, ASTNode*, OperatorType);
    ASTNode* getLeft() const;
    ASTNode* getRight() const;
    void setLeft(ASTNode*);
    void setRight(ASTNode*);
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
    LiteralNode(auto val) : literal(Object(val)) { }
    Object* eval(Scope*, bool lSide = false) override;
private:
    Object literal;
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

