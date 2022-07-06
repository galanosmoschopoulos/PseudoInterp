#include "objects.h"
#include "parser.h"
#include "AST.h"
#include <stdexcept>

static Object& checkLval(Object& obj) {
    if (!obj.isLval()) // We have to ensure that lhs is an lVal. I.e. (x + 5) = 2 is invalid, but x = 2 is valid
    {
        throw std::runtime_error("Assignment: left operand is not a modifiable lvalue.");
    }
    return obj;
}


CodeBlock::CodeBlock() = default;
CodeBlock::~CodeBlock() {
	for (auto st : statementVec) {
		delete st;
	}
}
void CodeBlock::eval(Scope* scope)
{
	scope->incLevel(); // Increase scope level
	for (auto st : statementVec) { // Execute all statements
		st->eval(scope);
	}
	scope->decrLevel(); // Decrease scope level
}
void CodeBlock::addStatement(ASTNode* statementRoot) {
	statementVec.push_back(statementRoot);
}



ASTNode::ASTNode() = default;
ASTNode::~ASTNode() = default;
void ASTNode::setForceRval(bool isIt) { forceRval = isIt; }
Object* ASTNode::eval(Scope*, bool) { return nullptr; }
void ASTNode::cleanTmps(std::initializer_list<Object*> tmpList) // Cleans temporary objects created in the evaluation of an expression (i.e. in a = 5 + 3*2, the object with value 3*2 is temporary)
{
	for(const Object* oPtr : tmpList)
	{
		if(!oPtr->isLval()) // If they're not lValues (i.e. variables in the scope), delete them
		{
			delete oPtr;
		}
	}
}




BinaryNode::BinaryNode() = default;
BinaryNode::~BinaryNode() {
	delete left;
	delete right;
}
BinaryNode::BinaryNode(ASTNode *l, ASTNode *r, OperatorType opType) : left(l), right(r), opType(opType) {}
ASTNode* BinaryNode::getLeft() const { return left; }
ASTNode* BinaryNode::getRight() const { return right; }
void BinaryNode::setLeft(ASTNode *l) { left = l; }
void BinaryNode::setRight(ASTNode *r) { right = r; }
Object* BinaryNode::eval(Scope *scope, bool lSide)
{
	Object* oLeft = left->eval(scope, (opType == OperatorType::ASSIGNMENT)?(true):(false)); // If we have the assignment operator, the left node should be passed with lSide=true
	Object* oRight = right->eval(scope, lSide);
	Object* result = nullptr;
	if (!oLeft || !oRight) { throw std::runtime_error("Null object pointer received."); }
	switch (opType)
	{
	case OperatorType::ADDITION:
		result = new Object(*oLeft + *oRight);
		break;
	case OperatorType::SUBTRACTION:
		result = new Object(*oLeft - *oRight);
		break;
	case OperatorType::MULTIPLICATION:
		result = new Object(*oLeft * *oRight);
		break;
	case OperatorType::DIVISION:
		result = new Object(*oLeft / *oRight);
		break;
	case OperatorType::MODULO:
		result = new Object(*oLeft % *oRight);
		break;
	case OperatorType::LESS:
		result = new Object(*oLeft < *oRight);
		break;
	case OperatorType::LESS_EQ:
		result = new Object(*oLeft <= *oRight);
		break;
	case OperatorType::GREATER:
		result = new Object(*oLeft > *oRight);
		break;
	case OperatorType::GRE_EQ:
		result = new Object(*oLeft >= *oRight);
		break;
	case OperatorType::EQUAL:
		result = new Object(*oLeft == *oRight);
		break;
	case OperatorType::NOT_EQUAL:
		result = new Object(*oLeft != *oRight);
		break;
	case OperatorType::OR:
		result = new Object(*oLeft || *oRight);
		break;
	case OperatorType::AND:
		result = new Object(*oLeft && *oRight);
		break;
	case OperatorType::ASSIGNMENT:
		result = &checkLval(*oLeft = *oRight);
		break;
	case OperatorType::ADDITION_ASSIGN:
		result = &checkLval(*oLeft += *oRight);
		break;
	case OperatorType::SUBTRACTION_ASSIGN:
		result = &checkLval(*oLeft -= *oRight);
		break;
	case OperatorType::MULTIPLICATION_ASSIGN:
		result = &checkLval(*oLeft *= *oRight);
		break;
	case OperatorType::DIVISION_ASSIGN:
		result = &checkLval(*oLeft /= *oRight);
		break;
	case OperatorType::MODULO_ASSIGN:
		result = &checkLval(*oLeft %= *oRight);
		break;
	case OperatorType::COMMA:
		result = (oRight->isLval()) ? (oRight) : (new Object(*oRight));
		break;
	default:
		throw std::runtime_error("Incompatible operator in binary AST node.");
		break;
	}
	cleanTmps({ oLeft, oRight }); // Clean the temporary objects
	return result;
}



UnaryNode::UnaryNode() = default;
UnaryNode::~UnaryNode() { delete operand; }
UnaryNode::UnaryNode(ASTNode* operand, OperatorType opType) : operand(operand), opType(opType) {}
Object* UnaryNode::eval(Scope* scope, bool)
{
	Object* obj = operand->eval(scope);
	Object* result = nullptr;
	if (!obj) { throw std::runtime_error("Null object pointer received."); }
	Object tmpObj;
	switch (opType) {
	case OperatorType::OUTPUT:
		result = new Object(outputOp(*obj));
		break;
	case OperatorType::NOT:
		result = new Object(!*obj);
		break;
	case OperatorType::UNARY_NEGATION:
		result = new Object(-*obj);
		break;
	case OperatorType::UNARY_PLUS:
		result = new Object(+*obj);
		break;
	case OperatorType::PRE_INCR:
		result = &checkLval(++*obj);
		break;
	case OperatorType::PRE_DECR:
		result = &checkLval(++*obj);
		break;
	case OperatorType::POST_INCR:
		checkLval(*obj);
		tmpObj = (*obj)++;
		result = new Object(tmpObj);
		break;
	case OperatorType::POST_DECR:
		checkLval(*obj);
		tmpObj = (*obj)--;
		result = new Object(tmpObj);
		break;

	default:
		throw std::runtime_error("Incompatible operator in unary AST node.");
		break;
	}
	cleanTmps({obj}); // Clean tmp object
	return result;
}
Object& UnaryNode::outputOp(Object &obj)
{
	output(obj);
	return obj;
}




LiteralNode::LiteralNode() = default;
LiteralNode::~LiteralNode() = default;
Object* LiteralNode::eval(Scope*, bool) { return new Object(literal); }


IDNode::IDNode() = default;
IDNode::~IDNode() = default;
IDNode::IDNode(const std::string &id) : id(id) {}
Object* IDNode::eval(Scope *scope, bool lSide) 
{
	Object* obj = nullptr;
	if (!scope->checkObj(id) && lSide) // If object with set id doesn't exist, and is exactly in the left side (lSide) of an equality operator, create a new object with such id
		scope->addObj(Object(), id);
	obj = scope->getObj(id);
	if (forceRval) // If it is forced to be an rval
		obj->setLval(false);
	return obj;
}

