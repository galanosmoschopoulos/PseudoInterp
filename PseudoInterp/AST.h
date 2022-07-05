#pragma once
#include "objects.h"
#include "parser.h"
#include "operators.h"
#include <stdexcept>
#include <vector>
#include <initializer_list>

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
	Object* addition(const Object*, const Object*) const;
    Object* assign(Object*, const Object*);

};

class CodeBlock
{
public:
    CodeBlock();
    void eval(Scope*);
    void addStatement(ASTNode*);
private:
    std::vector<ASTNode*> statementVec;
};

class LiteralNode final : public ASTNode
{
public:
    LiteralNode();
    LiteralNode(auto val) : literal(new Object(val)) { }
    Object* eval(Scope*, bool lSide = false) override;
private:
    Object* literal = nullptr;
};

class IDNode final : public ASTNode
{
public:
    IDNode();
    IDNode(const std::string&);
    Object* eval(Scope*, bool lSide = false);
private:
    std::string id = "";
};

class UnaryNode final : public ASTNode
{
public:
    UnaryNode();
    UnaryNode(ASTNode*, OperatorType);
    Object* eval(Scope*, bool lSide = false) override;
private:
    Object* outputOp(Object* obj);
    OperatorType opType = OperatorType::UNKNOWN;
    ASTNode* operand = nullptr;
};

