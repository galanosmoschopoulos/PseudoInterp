#include "objects.h"
#include "parser.h"
#include "AST.h"
#include <stdexcept>

inline static Object& checkLval(Object& obj) {
    if (!obj.isLval()) // We have to ensure that lhs is an lVal. I.e. (x + 5) = 2 is invalid, but x = 2 is valid
    {
        throw std::runtime_error("Assignment: left operand is not a modifiable lvalue.");
    }
    return obj;
}

static void cleanTmps(std::initializer_list<Object*> tmpList) // Cleans temporary objects created in the evaluation of an expression (i.e. in a = 5 + 3*2, the object with value 3*2 is temporary)
{
	for(const Object* oPtr : tmpList)
	{
		if(!oPtr->isLval()) // If they're not lValues (i.e. variables in the scope), delete them
		{
			delete oPtr;
		}
	}
}



/*--------------------------------------------------------------------------------------------------------------------------------*/
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
void CodeBlock::addStatement(Statement* st) {
	statementVec.push_back(st);
}
/*--------------------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------------------*/
Statement::Statement() = default;
Statement::~Statement() = default;
void Statement::eval(Scope*) {}

WhileStatement::WhileStatement() = default;
WhileStatement::~WhileStatement() {
	delete condition;
	delete block;
}
WhileStatement::WhileStatement(ASTNode* condition, CodeBlock* block) : condition(condition), block(block) {}
void WhileStatement::eval(Scope* scope) {
	Object* conditionObj = nullptr;

	while (( conditionObj = condition->eval(scope) )->isTrue()) { // As long as it is true
		block->eval(scope);
		cleanTmps({conditionObj}); // The result of the condition changes each time
	}
}

ForStatement::ForStatement() = default;
ForStatement::~ForStatement() {
	delete counterNode;
	delete upperNode;
	delete lowerNode;
	delete block;
}
ForStatement::ForStatement(ASTNode* counterNode, ASTNode* lowerNode, ASTNode* upperNode, CodeBlock* block) :
	counterNode(counterNode), lowerNode(lowerNode), upperNode(upperNode), block(block) {}
void ForStatement::eval(Scope* scope) {
	Object* lowerObj = lowerNode->eval(scope), *upperObj = upperNode->eval(scope);
	scope->incLevel(); // We want the counter variable to exist in an inner scope (only available to the block)
	Object* counterObj = counterNode->eval(scope, true); // lSide = true, to allow initialization of the variable instead of searching for it. It is like doing var = 0.
	if (!counterObj->isLval()) throw std::runtime_error("Counter variable in for loop not an lval.");
	if ((* lowerObj > *upperObj).isTrue()) throw std::runtime_error("For loop limits error."); // Verift that lower <= upper
	*counterObj = *lowerObj;
	while ((*counterObj <= *upperObj).isTrue()) {
		block->eval(scope);
		lowerObj = lowerNode->eval(scope), upperObj = upperNode->eval(scope); // Re-evaluate the limits (something may have changed)
		++(*counterObj); // Increase the counter
	}
	cleanTmps({counterObj, lowerObj, upperObj});
	scope->decrLevel();
}

IfStatement::IfStatement() = default;
IfStatement::~IfStatement() {
	for (const auto& casePair : cases) {
		delete casePair.first;
		delete casePair.second;
	}
}
IfStatement::IfStatement(ASTNode* condition, CodeBlock* block) {
	addCase(condition, block);
}
void IfStatement::addCase(ASTNode* condition, CodeBlock* block) {
	cases.push_back(std::make_pair(condition, block));
}
void IfStatement::eval(Scope* scope) {
	for (const auto& casePair : cases) { // For each "case"
		Object* caseObj = casePair.first->eval(scope); // Evaluate the condition
		if (caseObj->isTrue()) {
			casePair.second->eval(scope); // Run the block
			return;
		}
		cleanTmps({ caseObj });
	}
}

ExprStatement::ExprStatement() = default;
ExprStatement::~ExprStatement() {
	delete exprRoot;
}
ExprStatement::ExprStatement(ASTNode* expr) : exprRoot(expr) {}
void ExprStatement::eval(Scope* scope) {
	cleanTmps({ exprRoot->eval(scope) }); // The result of an expression is a pointer to the root, but in an expresion statement it is discarded.
}
/*--------------------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------------------*/
ASTNode::ASTNode() = default;
ASTNode::~ASTNode() = default;
void ASTNode::setForceRval(bool isIt) { forceRval = isIt; }
Object* ASTNode::eval(Scope*, bool) { return nullptr; }

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
	Object* oLeft = left->eval(scope, (opType == OperatorType::ASSIGNMENT)?(true):(false)); // If we have the assignment operator, the left node should be passed with lSide=true. This allows it to be initialized if needed.
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
/*--------------------------------------------------------------------------------------------------------------------------------*/

