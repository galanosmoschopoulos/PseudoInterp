#include "parser.h"
#include "objects.h"
#include "operators.h"
#include "lexer.h"
#include "AST.h"
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

Parser::Parser() = default;
CodeBlock* Parser::getAST(const std::string &inputStr) { // Returns full AST based on inputStr
	lexer.setInput(inputStr); // Convert str to tokens
	lexer.lexInput();
	CodeBlock* mainBlock = parseBlock(); // Consider the whole program to be in a block
	if (lexer.getCurrToken().getType() != TokenType::EOFILE) { // If there are unparsed characters in the end, something's wrong
		throw std::runtime_error("Parsing error: illegal expression.");
	}
	return mainBlock;
}

// Note: (this->*(currGroup + 1)->parserFunc)(currGroup + 1) calls the appropriate parser function for operators of the next (higher) precedence level.
// Note: in grammaer definitions, T refers to an expression of higher precedence than E

CodeBlock* Parser::parseBlock() { // Parses blocks
	blockLevel++; // New block => level up
	CodeBlock* currBlock = new CodeBlock();
	while (lexer.getCurrToken().getType() != TokenType::EOFILE) {
		int i;
		for (i = 0; lexer.lookForw(i).getType() == TokenType::TAB; i++);
		if (i < blockLevel) { // If we meet less tabs than the current block level, it means this block has ended
			break;
		}
		else if (i > blockLevel) { // Excessive tabs yield identation error
			throw std::runtime_error("Identation error");
		}
		while (lexer.getCurrToken().getType() == TokenType::TAB) lexer.scanToken(); // Skip all the tabs

		ASTNode* currStatement = (this->*precedenceTab[0].parserFunc)(precedenceTab); // Parse current statement until newline
		currBlock->addStatement(currStatement);

		if (lexer.getCurrToken().getType() == TokenType::NEWLINE) { // If we it doesn't end in newline, something's wrong
			lexer.scanToken();
		}
		else throw std::runtime_error("Illegal expression");
	}
	blockLevel--; // Block has ended => level down
	return currBlock;
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
		else node = nullptr; // If it doesn't end with a matching parenthesis, error
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
		throw std::runtime_error("Parsing error: illegal expression.");
	}
	return node;
}

