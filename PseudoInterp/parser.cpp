#include "parser.h"
#include "objects.h"
#include "lexer.h"
#include "AST.h"
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>

Parser::Parser() = default;
CodeBlock* Parser::getAST(const std::string &inputStr) { // Returns full AST based on inputStr
	lexer.setInput(inputStr); // Convert str to tokens
	lexer.lexInput();
	CodeBlock* mainBlock = parseBlock(); // Consider the whole program to be in a block
	if (lexer.getCurrToken().getType() != TokenType::EOFILE) { // If there are unparsed characters in the end, something's wrong
		throw std::runtime_error(getParsingError());
	}
	return mainBlock;
}

// Note: (this->*(currGroup + 1)->parserFunc)(currGroup + 1) calls the appropriate parser function for operators of the next (higher) precedence level.
// Note: in grammaer definitions, T refers to an expression of higher precedence than E


// A block is a sequence of statements, that have an equal number of leading tabs (identation).
// To parse a block, the current token must be at the beginning of the line (not at the beginning of the statement).
CodeBlock* Parser::parseBlock() {
	blockLevel++; // New block => level up
	CodeBlock* currBlock = new CodeBlock();
	while (lexer.getCurrToken().getType() != TokenType::EOFILE) {
		int nTabs; // Holds the number of tabs until the statement
		if (lessTabs(nTabs)) break; // If less than expected, we're out of the block
		
		while (lexer.getCurrToken().getType() == TokenType::TAB) lexer.scanToken(); // Skip all the tabs
		
		Statement* currStatement = nullptr;
		switch (lexer.getCurrToken().getType()) { // Parse depending on the statement
		case TokenType::WHILE:
			currStatement = parseWhile();
			break;
		case TokenType::IF:
			currStatement = parseIf();
			break;
		case TokenType::FOR:
			currStatement = parseFor();
			break;
		default:
			currStatement = parseExpr();
			break;
		}
		currBlock->addStatement(currStatement); // Add statement to sequence
	}
	blockLevel--; // Block has ended => level down
	return currBlock;
}

bool Parser::lessTabs(int& i) {
	for (i = 0; lexer.lookForw(i).getType() == TokenType::TAB; i++);
	if (i < blockLevel) { // Less tabs than the current block level
		return true;
	}
	else if (i > blockLevel) { // Excessive tabs yield identation error
		throw std::runtime_error(getParsingError("identation error"));
	}
	return false;
}

Statement* Parser::parseIf() {
	IfStatement* statement = new IfStatement();
	TokenType currToken;
	while ((currToken = lexer.getCurrToken().getType()) == TokenType::IF || currToken == TokenType::ELIF || currToken == TokenType::ELSE) { // To parse the whole if-elif-else chain
		lexer.scanToken();
		ASTNode* condition = nullptr;

		if (currToken != TokenType::ELSE) { // Else doesn't have a condition
			condition = (this->*precedenceTab[0].parserFunc)(precedenceTab);
		}
		else {
			condition = new LiteralNode(1); // Always true
		}

		if (lexer.getCurrToken().getType() == TokenType::NEWLINE) { // If it doesn't end in newline, something's wrong
			lexer.scanToken();
		}
		else throw std::runtime_error(getParsingError());

		CodeBlock* block = parseBlock(); // Parse block and add it
		statement->addCase(condition, block);

		if (currToken == TokenType::ELSE) break; // If we're on an else, we have finished

		int nTabs = 0;
		if (lessTabs(nTabs)) return statement; // Count the tabs. If less than expected, we are done
		
		TokenType nextTok;
		if ((nextTok = lexer.lookForw(nTabs).getType()) == TokenType::ELIF || nextTok == TokenType::ELSE) { // If the next statement after the tabs is elif or else
			while (lexer.getCurrToken().getType() == TokenType::TAB) lexer.scanToken(); // Skip tabs, and continue the loop
		}
		else break;
	}
	return statement;
}

Statement* Parser::parseExpr() {
	Statement* exprStatement = new ExprStatement((this->*precedenceTab[0].parserFunc)(precedenceTab));
	if (lexer.getCurrToken().getType() == TokenType::NEWLINE) { // If it doesn't end in newline, something's wrong
		lexer.scanToken();
	}
	else throw std::runtime_error(getParsingError());
	return exprStatement;
}

Statement* Parser:: parseWhile() {
	lexer.scanToken();
	ASTNode* condition = (this->*precedenceTab[0].parserFunc)(precedenceTab); // Get the condition expression
	if (lexer.getCurrToken().getType() == TokenType::NEWLINE) {
		lexer.scanToken();
	}
	else throw std::runtime_error(getParsingError());
	CodeBlock* block = parseBlock(); // Get the block
	return new WhileStatement(condition, block);

}

Statement* Parser::parseFor() {
	lexer.scanToken();
	ASTNode* counterNode = (this->*precedenceTab[0].parserFunc)(precedenceTab); // This is the dummy counter variable of the loop

	if (lexer.getCurrToken().getType() == TokenType::FROM) { // Check if the limits are separated by 'from' and 'to'
		lexer.scanToken();
	}
	else throw std::runtime_error("For loop limit separators syntax error");

	ASTNode* lowerNode = (this->*precedenceTab[0].parserFunc)(precedenceTab);

	if (lexer.getCurrToken().getType() == TokenType::TO) {
		lexer.scanToken();
	}
	else throw std::runtime_error("For loop limit separators syntax error");

	ASTNode* upperNode = (this->*precedenceTab[0].parserFunc)(precedenceTab);

	if (lexer.getCurrToken().getType() == TokenType::NEWLINE) {
		lexer.scanToken();
	}
	else throw std::runtime_error(getParsingError());

	CodeBlock* block = parseBlock();
	return new ForStatement(counterNode, lowerNode, upperNode, block);
}

ASTNode* Parser::parseUnary(precedenceGroup* currGroup) { // Parses right associative unary operators (E -> T, E -> [op]E)
	TokenType currToken = lexer.getCurrToken().getType();
	if (currGroup->findOp.contains(currToken)) { // If current token is in the token group we are examining
		lexer.scanToken(); // Proceed to next token
		ASTNode* child = parseUnary(currGroup); // E -> [op]E
		return new UnaryNode(child, currGroup->findOp[currToken]); // Create unary node, assign the corresponding operator
	}
	return (this->*(currGroup + 1)->parserFunc)(currGroup + 1); // E -> T
}
ASTNode* Parser::parseBinLeft(precedenceGroup* currGroup) { // Parses binary left associative operators. ( E -> E + T, hence E -> T {[op]T} )
	ASTNode* nodeA = (this->*(currGroup+1)->parserFunc)(currGroup + 1); // Get left operand
	while (1) { // As long as we have repeated terms (i.e. 5 + 2 + 3 + 8), continue parsing
		TokenType currToken = lexer.getCurrToken().getType();
		if (currGroup->findOp.contains(currToken)) {
			lexer.scanToken();
			ASTNode* nodeB = (this->*(currGroup+1)->parserFunc)(currGroup + 1); // Get left operand
			ASTNode* tmpNode = new BinaryNode(nodeA, nodeB, currGroup->findOp[currToken]); // Use temporary node to fix left recursion problem
			nodeA = tmpNode;
		}
		else
			return nodeA;
	}
}
ASTNode* Parser::parseBinRight(precedenceGroup* currGroup) { // Parses binary right associative operators. ( E -> T + E )
	ASTNode* nodeA = (this->*(currGroup+1)->parserFunc)(currGroup + 1); // Get left operand
	TokenType currToken = lexer.getCurrToken().getType();
	if (currGroup->findOp.contains(currToken)) { // If current token is in the group we're examining
		lexer.scanToken();
		ASTNode* nodeB = (this->*(currGroup)->parserFunc)(currGroup); // E -> T + E (call E again)
		return new BinaryNode(nodeA, nodeB, currGroup->findOp[currToken]);
	}
	return nodeA;
}


ASTNode* Parser::parseUnaryPostfix(precedenceGroup* currGroup) { // Parses unary postfix operation with production { E -> E[op], E -> T }, hence E -> T{[op]}
	ASTNode* node = (this->*(currGroup+1)->parserFunc)(currGroup + 1);
	while (1) {
	TokenType currToken = lexer.getCurrToken().getType();
		if (currGroup->findOp.contains(currToken)) {
			lexer.scanToken();
			node = new UnaryNode(node, currGroup->findOp[currToken]);
		}
		else
			return node;
	}
}

ASTNode* Parser::parsePrimary(precedenceGroup*) { // Parses literals, identifiers, and parentheses
	ASTNode* node;
	switch (lexer.getCurrToken().getType()) {
	case TokenType::INT_LIT: // If number literal, convert lexeme to int
		node = new LiteralNode(std::stoi(lexer.getCurrToken().getLexeme()));
		lexer.scanToken();
		break;
	case TokenType::L_PAREN:
		lexer.scanToken();
		node = (this->*precedenceTab[0].parserFunc)(precedenceTab); // Go back to lowest precedence
		if (node) {
			node->setForceRval(true); // (myVar) = 5 should not be a valid syntax
		}
		if (lexer.getCurrToken().getType() == TokenType::R_PAREN) {
			lexer.scanToken();
		}
		else {// If it doesn't end with a matching parenthesis, error
			node = nullptr;
			throw std::runtime_error(getParsingError("no matching parenthesis"));
		}
		break;
	case TokenType::ID: // For object identifiers
		node = new IDNode(lexer.getCurrToken().getLexeme());
		lexer.scanToken();
		break;
	default:
		node = nullptr;
		break;
	}
	if (!node) {
		throw std::runtime_error(getParsingError());
	}
	return node;
}
std::string Parser::getParsingError(const std::string &customMessage) {
	/*
	std::stringstream ss;
	int lineNum = 0, realLineNum = 0, linePos = 0;
	std::tie(lineNum, realLineNum, linePos) = lexer.posToLine(lexer.getCurrToken().getPos());
	ss << "Parsing error in line " << realLineNum + 1 << ": " << customMessage << '\n';
	ss << lexer.getInputLine(lineNum) << '\n';
	for (int i = 0; i != linePos; i++, ss << ' ');
	ss << "^\n";
	return ss.str();*/
	return "parsing error " + customMessage;
}
