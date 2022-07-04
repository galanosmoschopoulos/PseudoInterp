#include "objects.h"
#include "parser.h"
#include "operators.h"
#include "AST.h"
#include <stdexcept>

ASTNode::ASTNode() = default;
ASTNode::~ASTNode() = default;
void ASTNode::setForceRval(bool isIt) { forceRval = isIt; }
Object* ASTNode::eval(Scope*, bool lSide) { return nullptr; }
void ASTNode::cleanTmps(std::initializer_list<Object*> tmpList)
{
	for(const Object* oPtr : tmpList)
	{
		if(!oPtr->isLval()) // If they're not lValues, kill them
		{
			delete oPtr;
		}
	}
}

BinaryNode::BinaryNode() = default;
BinaryNode::BinaryNode(ASTNode *l, ASTNode *r, OperatorType opType) : left(l), right(r), opType(opType) {}
ASTNode* BinaryNode::getLeft() const { return left; }
ASTNode* BinaryNode::getRight() const { return right; }
void BinaryNode::setLeft(ASTNode *l) { left = l; }
void BinaryNode::setRight(ASTNode *r) { right = r; }
Object* BinaryNode::eval(Scope *scope, bool lSide)
{
	Object* oLeft = left->eval(scope, (opType == OperatorType::ASSIGNMENT)?(true):(false));
	Object* oRight = right->eval(scope, lSide);
	Object* result = nullptr;
	switch (opType)
	{
	case OperatorType::ADDITION:
		result = addition(oLeft, oRight);
		break;
	case OperatorType::ASSIGNMENT:
		result = assign(oLeft, oRight);
		break;
	default:
		throw std::runtime_error("Incompatible operator in binary AST node.");
	}
	cleanTmps({ oLeft, oRight }); // Clean the temporary objects
	return result;
}
Object* BinaryNode::addition(const Object* leftObj, const Object* rightObj) const
{
	if(leftObj->getType() == ObjectType::INT && rightObj->getType() == ObjectType::INT)
    {
        return new Object(leftObj->getInt() + rightObj->getInt());
    }
    else if(leftObj->getType() == ObjectType::INT && rightObj->getType() == ObjectType::STR)
    {
        return new Object(std::to_string(leftObj->getInt()) + rightObj->getStr());
    }
	else if(leftObj->getType() == ObjectType::STR && rightObj->getType() == ObjectType::INT)
    {
        return new Object(leftObj->getStr() + std::to_string(rightObj->getInt()));
    }
    else if(leftObj->getType() == ObjectType::STR && rightObj->getType() == ObjectType::STR)
    {
        return new Object(leftObj->getStr() + rightObj->getStr());
    }
    else
    {
        throw std::runtime_error("Operands of incompatible types in addition operator");
    }
}
Object* BinaryNode::assign(Object* leftObj, const Object* rightObj)
{
	if (!leftObj->isLval())
	{
        throw std::runtime_error("Assignment: left operand is not a modifiable lvalue.");
	}
    return new Object(*leftObj = *rightObj); // essentially returns rightObj
    // In C, the expression a = b returns b
}


CodeBlock::CodeBlock() = default;
void CodeBlock::eval(Scope* scope)
{
	scope->incLevel();
	for (auto st : statementVec) {
		st->eval(scope);
	}
	scope->decrLevel();
}
void CodeBlock::addStatement(ASTNode* statementRoot) {
	statementVec.push_back(statementRoot);
}


LiteralNode::LiteralNode() = default;
Object* LiteralNode::eval(Scope* scope, bool lSide) { return literal; }


IDNode::IDNode() = default;
IDNode::IDNode(const string &id) : id(id) {}
Object* IDNode::eval(Scope *scope, bool lSide) 
{
	Object* obj = nullptr;
	if (!scope->checkObj(id) && lSide)
		scope->addObj(Object(), id);
	obj = scope->getObj(id);
	if (forceRval)
		obj->setLval(false);
	return obj;
}


UnaryNode::UnaryNode() = default;
UnaryNode::UnaryNode(ASTNode* operand, OperatorType opType) : operand(operand), opType(opType) {}
Object* UnaryNode::eval(Scope* scope, bool lSide)
{
	Object* obj = operand->eval(scope);
	Object* result = nullptr;
	switch (opType) {
	case OperatorType::OUTPUT:
		result = outputOp(obj);
		break;
	}
	cleanTmps({obj});
	return result;
}
Object* UnaryNode::outputOp(Object* obj)
{
	output(*obj);
	return nullptr;
}

