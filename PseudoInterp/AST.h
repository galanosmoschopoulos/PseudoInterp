#pragma once
#include "objects.h"
#include "parser.h"
#include "operators.h"
#include <stdexcept>
#include <initializer_list>
class CodeBlock;
class ASTNode;
class BinaryNode;

class ASTNode
{
public:
    ASTNode();
    virtual ~ASTNode();
    virtual Object* eval(Scope*, bool lSide = false);
    void setForceRval(bool isIt);
protected:
    bool forceRval = false;
    static void cleanTmps(std::initializer_list<Object*> tmpList);
};

class BinaryNode final : public ASTNode
{
public:
    BinaryNode();
    BinaryNode(ASTNode* l, ASTNode* r, OperatorType opType);
    ASTNode* getLeft() const;
    ASTNode* getRight() const;
    void setLeft(ASTNode* l);
    void setRight(ASTNode* r);
    Object* eval(Scope* scope, bool lSide = false) override;
private:
    OperatorType opType;
    ASTNode* left = nullptr;
    ASTNode* right = nullptr;
	Object* addition(const Object*, const Object*) const;
	/*Object* subtraction(const Object*, const Object*) const;
	Object* multiplication(const Object*, const Object*) const;
	Object* division(const Object*, const Object*) const;
	Object* l_or(const Object*, const Object*) const;
	Object* l_and(const Object*, const Object*) const;*/
    Object* assign(Object*, const Object*);

};

class CodeBlock
{
public:
    CodeBlock();
    void eval(Scope* scope);
    void addStatement(ASTNode* statementRoot);
private:
    vector<ASTNode*> statementVec;
};

class LiteralNode final : public ASTNode
{
public:
    LiteralNode();
    LiteralNode(auto val) : literal(new Object(val)) { }
    Object* eval(Scope* scope, bool lSide = false) override;
private:
    Object* literal = nullptr;
};

class IDNode final : public ASTNode
{
public:
    IDNode();
    IDNode(const string& id);
    Object* eval(Scope* scope, bool lSide = false);
private:
    string id;
};

class UnaryNode final : public ASTNode
{
public:
    UnaryNode();
    UnaryNode(ASTNode* operand, OperatorType opType);
    Object* eval(Scope* scope, bool lSide = false) override;
private:
    Object* outputOp(Object* obj);
    OperatorType opType;
    ASTNode* operand = nullptr;
};

