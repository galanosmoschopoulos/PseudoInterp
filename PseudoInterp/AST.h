#pragma once
#include "objects.h"
#include "parser.h"
#include <stdexcept>
#include <vector>
#include <initializer_list>

static Object& checkLval(Object& obj);
class ASTNode
{
public:
    ASTNode();
    virtual ~ASTNode();
    virtual Object* eval(Scope*, bool lSide = false);
    void setForceRval(bool);
protected:
    bool forceRval = false;
    static void cleanTmps(std::initializer_list<Object*>);
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

class CodeBlock
{
public:
    CodeBlock();
    ~CodeBlock();
    void eval(Scope*);
    void addStatement(ASTNode*);
private:
    std::vector<ASTNode*> statementVec;
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

