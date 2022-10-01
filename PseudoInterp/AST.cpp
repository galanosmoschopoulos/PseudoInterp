#include "AST.h"
#include "errors.h"

template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};

Object& checkLval(const Object& obj)
{
	if (!obj.isLval()) // We have to ensure that lhs is an lVal. I.e. (x + 5) = 2 is invalid, but x = 2 is valid
	{
		throw CustomError("Left operand is not a modifiable lvalue.");
	}
	return const_cast<Object&>(obj);
}

// Cleans temporary objects created in the evaluation of an expression (i.e. in a = 5 + 3*2, the object with value 3*2 is temporary)
static void cleanTmps(const std::initializer_list<Object*> tmpList)
{
	for (const Object* oPtr : tmpList)
	{
		if (oPtr) // If the pointer is valid
		{
			if (!oPtr->isLval()) // If they're not lValues (i.e. variables in the scope), delete them
			{
				delete oPtr;
			}
		}
	}
}

CodeBlock::CodeBlock() = default;

CodeBlock::~CodeBlock()
{
	for (const Statement* st : statementVec)
	{
		delete st;
	}
}

Object* CodeBlock::eval(Scope* scope, const bool isInFunction) const
{
	Object* tmpObj = nullptr;
	scope->incLevel(); // Increase scope level
	for (Statement* st : statementVec)
	{
		// Execute all statements
		if ((tmpObj = st->eval(scope, isInFunction)) != nullptr) 
		{
			// If we get something that isn't nullptr, a return statement has been run
			break;
		}
	}
	scope->decrLevel(); // Decrease scope level
	return tmpObj;
}

void CodeBlock::addStatement(Statement* st)
{
	statementVec.push_back(st);
}

Statement::Statement() = default;
Statement::~Statement() = default;
Object* Statement::eval(Scope*, bool) { return nullptr; }

WhileStatement::WhileStatement() = default;

WhileStatement::~WhileStatement()
{
	delete condition;
	delete block;
}

WhileStatement::WhileStatement(ASTNode* condition, CodeBlock* block, size_t position) : condition(condition),
	block(block)
{
	pos = position;
}

Object* WhileStatement::eval(Scope* scope, const bool isInFunction)
{
	Object* conditionObj = nullptr;
	Object* tmpObj = nullptr;
	while ((conditionObj = condition->eval(scope))->isTrue())
	{
		// As long as it is true
		tmpObj = block->eval(scope, isInFunction);
		cleanTmps({conditionObj}); // The result of the condition changes each time
		if (tmpObj != nullptr) break;
	}
	return tmpObj;
}

ForStatement::ForStatement() = default;

ForStatement::~ForStatement()
{
	delete counterNode;
	delete upperNode;
	delete lowerNode;
	delete block;
}

ForStatement::ForStatement(ASTNode* counterNode, ASTNode* lowerNode, ASTNode* upperNode, CodeBlock* block,
                           size_t position) :
	counterNode(counterNode), lowerNode(lowerNode), upperNode(upperNode), block(block)
{
	pos = position;
}

Object* ForStatement::eval(Scope* scope, const bool isInFunction)
{
	Object *lowerObj = lowerNode->eval(scope), *upperObj = upperNode->eval(scope);
	scope->incLevel(); // We want the counter variable to exist in an inner scope (only available to the block)
	Object* counterObj = counterNode->eval(scope, true);
	// lSide = true, to allow initialization of the variable instead of searching for it. It is like doing var = 0.

	if ((*lowerObj > *upperObj).isTrue()) throw ValueError("Lower limit greater than upper limit.", pos);
	// Verify that lower <= upper

	*counterObj = *lowerObj;
	Object* tmpObj = nullptr;
	while ((*counterObj <= *upperObj).isTrue())
	{
		tmpObj = block->eval(scope, isInFunction);
		if (tmpObj != nullptr) break;
		cleanTmps({lowerObj, upperObj});
		lowerObj = lowerNode->eval(scope);
		upperObj = upperNode->eval(scope); // Re-evaluate the limits (something may have changed)
		++(*counterObj); // Increase the counter
	}
	cleanTmps({counterObj, lowerObj, upperObj});
	scope->decrLevel();
	return tmpObj;
}

IfStatement::IfStatement() = default;

IfStatement::~IfStatement()
{
	for (const auto& [casePtr, blockPtr] : cases)
	{
		delete casePtr;
		delete blockPtr;
	}
}

IfStatement::IfStatement(size_t position)
{
	pos = position;
}

IfStatement::IfStatement(ASTNode* condition, CodeBlock* block, size_t position)
{
	pos = position;
	addCase(condition, block);
}

void IfStatement::addCase(ASTNode* condition, CodeBlock* block)
{
	cases.emplace_back(condition, block);
}

Object* IfStatement::eval(Scope* scope, const bool isInFunction)
{
	for (const auto& [casePtr, blockPtr] : cases)
	{
		// For each "case"
		Object* caseObj = casePtr->eval(scope); // Evaluate the condition
		if (caseObj->isTrue())
		{
			cleanTmps({caseObj});
			return blockPtr->eval(scope, isInFunction); // Run the block
		}
		cleanTmps({caseObj});
	}
	return nullptr;
}

ExprStatement::ExprStatement() = default;

ExprStatement::~ExprStatement()
{
	delete exprRoot;
}

ExprStatement::ExprStatement(ASTNode* expr, size_t position) : exprRoot(expr)
{
	pos = position;
}

Object* ExprStatement::eval(Scope* scope, bool)
{
	cleanTmps({exprRoot->eval(scope)});
	// The result of an expression is a pointer to the root, but in an expression statement it is discarded.
	return nullptr;
}

ReturnStatement::ReturnStatement() = default;

ReturnStatement::~ReturnStatement()
{
	delete returnRoot;
}

ReturnStatement::ReturnStatement(ASTNode* expr, size_t position) : returnRoot(expr)
{
	pos = position;
}

Object* ReturnStatement::eval(Scope* scope, const bool isInFunction)
{
	if (!isInFunction)
	{
		throw CustomError("Return statements should only be inside functions.", pos);
	}
	Object* returnObj = returnRoot->eval(scope);
	const auto newObj= new Object(*returnObj); // Copies the return value to a knew object
	cleanTmps({returnObj});
	return newObj;
}

FunctionDefStatement::FunctionDefStatement() = default;

FunctionDefStatement::~FunctionDefStatement()
{
	delete block;
	delete funcID;
	for (const ASTNode* node : funcParams)
	{
		delete node;
	}
}

FunctionDefStatement::FunctionDefStatement(ASTNode* funcID, std::vector<ASTNode*> funcParams, CodeBlock* block,
                                           size_t position) :
	funcID(funcID), funcParams(
		std::move(funcParams)), block(block)
{
	pos = position;
}

Object* FunctionDefStatement::eval(Scope* scope, bool)
{
	*funcID->eval(scope, true) = Object(Function(block, funcParams, scope->getFuncLevel())); // Evaluate the ID node to create a Function object
	// Get the current func level so that the Function knows which variables it can access when it runs
	return nullptr;
}

ASTNode::ASTNode() = default;
ASTNode::~ASTNode() = default;
void ASTNode::setForceRval(bool isIt) { forceRval = isIt; }
Object* ASTNode::eval(Scope*, bool) { return nullptr; }

nAryNode::nAryNode() = default;

nAryNode::~nAryNode()
{
	delete mainOperand;
	for (const ASTNode* opPtr : nOperands) { delete opPtr; }
}

nAryNode::nAryNode(ASTNode* mainOperand, const OperatorType opType, std::vector<ASTNode*> nOperands, size_t position) :
	opType(opType),
	mainOperand(mainOperand), nOperands(
		std::move(nOperands))
{
	pos = position;
}

Object* nAryNode::eval(Scope* scope, bool)
{
	Object* result = nullptr;
	Object* mainObject = mainOperand->eval(scope);
	std::vector<Object*> nObjects;
	for (ASTNode* node : nOperands)
	{
		nObjects.push_back(node->eval(scope));
	}
	try
	{
		switch (opType)
		{
		case OperatorType::SUBSCRIPT:
			result = (*mainObject)[nObjects];
			break;
		case OperatorType::FUNCTION_CALL:
			result = (*mainObject)(scope, nObjects);
			break;
		default:
			break;
		}
	}
	catch (CustomError& ce)
	{
		/* If pos is set already, it means that this error was produced within a function.
		 * So if we change it it will point to the function call operator and not the actual location.*/
		if (!ce.isPosSet()) ce.setPos(pos);
		throw;
	}
	cleanTmps({mainObject});
	for (Object* objPtr : nObjects)
	{
		cleanTmps({objPtr});
	}
	return result;
}

BinaryNode::BinaryNode() = default;

BinaryNode::~BinaryNode()
{
	delete left;
	delete right;
}

BinaryNode::BinaryNode(ASTNode* l, ASTNode* r, OperatorType opType, size_t position) : opType(opType), left(l), right(r)
{
	pos = position;
}

Object* BinaryNode::eval(Scope* scope, const bool lSide)
{
	// Allow new variable initialization
	Object* oLeft = left->eval(scope, (opType == OperatorType::ASSIGNMENT) ? (true) : (false)); 
	/* If we have the assignment operator, the left node should be passed with lSide = true.
	 *This allows it to be initialized if needed. */
	Object* result = nullptr;
	if (! oLeft) { throw FatalError("", pos); }
	try
	{
		if (opType == OperatorType::MEMBER_ACCESS)
		{
			/*Member access refers to accessing methods in certain objects. An node containing the pointer name (right) searches in the
			 * method scope of the object. It retrieves an external function which executes the method.*/
			std::visit(overload{
				           [&result, this](std::shared_ptr<StackContainer>& sc){ result = right->eval(&sc->getMethodScope()); },
				           [&result, this](std::shared_ptr<QueueContainer>& qc){ result = right->eval(&qc->getMethodScope()); },
				           [&result, this](std::shared_ptr<ArrayContainer>& ac){ result = right->eval(&ac->getMethodScope()); },
				           [&result, this](std::shared_ptr<CollectionContainer>& cc){ result = right->eval(&cc->getMethodScope()); },
				           [&result, this](std::shared_ptr<StringContainer>& sc){ result = right->eval(&sc->getMethodScope()); },
						   [](auto&) {throw TypeError("Object does not contain methods."); }
			           }, oLeft->data);
		}
		else
		{
			Object* oRight = right->eval(scope, lSide); // Get the rhs object
			if (!oRight) { throw FatalError("", pos); }
			switch (opType) // Apply different operators
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
				result = &checkLval(*oLeft = *oRight); // The result of assignment is the lhs operand. If it isn't an lVal, then assignment is impossible.
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
				throw FatalError("", pos);
			}
			cleanTmps({oLeft, oRight}); // Clean the temporary objects
		}
	}
	catch (CustomError& ce)
	{
		if (!ce.isPosSet()) ce.setPos(pos);
		throw;
	}

	return result;
}

UnaryNode::UnaryNode() = default;
UnaryNode::~UnaryNode() { delete operand; }

UnaryNode::UnaryNode(ASTNode* operand, const OperatorType opType, size_t position) : opType(opType), operand(operand)
{
	pos = position;
}

Object* UnaryNode::eval(Scope* scope, bool)
{
	Object* obj = operand->eval(scope);
	Object* result = nullptr;
	if (!obj) { throw FatalError("", pos); }
	Object tmpObj;
	try
	{
		switch (opType)
		{
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
			result = &checkLval(++ *obj); // The prefix operators return an lVal
			break;
		case OperatorType::PRE_DECR:
			result = &checkLval(-- *obj);
			break;
		case OperatorType::POST_INCR:
			checkLval(*obj); // The postfix do not. Hence, we use a temporary object to act as an rval
			tmpObj = (*obj)++;
			result = new Object(tmpObj);
			break;
		case OperatorType::POST_DECR:
			checkLval(*obj);
			tmpObj = (*obj)--;
			result = new Object(tmpObj);
			break;
		default:
			throw FatalError("", pos);
		}
	}
	catch (CustomError& ce)
	{
		if (!ce.isPosSet()) ce.setPos(pos);
		throw;
	}

	cleanTmps({obj}); // Clean tmp object
	return result;
}

LiteralNode::LiteralNode() = default;

LiteralNode::~LiteralNode()
{
	delete literal;
}

Object* LiteralNode::eval(Scope*, bool) { return new Object(*literal); }

IDNode::IDNode() = default;
IDNode::~IDNode() = default;

IDNode::IDNode(std::string id, size_t position) : id(std::move(id))
{
	pos = position;
}

Object* IDNode::eval(Scope* scope, const bool lSide)
{
	Object* obj = nullptr;
	if (!scope->checkObj(id) && lSide)
		/* If object with set id doesn't exist, and is exactly in the left side (lSide) of an
		 * equality operator, create a new object with such id*/
		scope->addObj(Object(), id);
	obj = scope->getObj(id);
	if (!obj)
	{
		throw NameError("Object with identifier \'" + id + "\' does not exist in scope.", pos);
	}
	if (forceRval) // If it is forced to be an rval
		obj->setLval(false);
	return obj;
}

