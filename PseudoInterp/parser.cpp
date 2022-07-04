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
ASTNode* Parser::getAST(const std::string &inputStr) {
	lexer.setInput(inputStr);
	lexer.lexInput();
	ASTNode* root = (this->*precedenceTab[0].parserFunc)(precedenceTab);
	if (lexer.getCurrToken().getType() != TokenType::EOFILE) {
		throw std::runtime_error("Parsing error: illegal expression.");
	}
	return root;
}
ASTNode* Parser::parseUnary(precedenceGroup* currGroup) {
	TokenType currToken = lexer.getCurrToken().getType();
	if (currGroup->findOp.contains(currToken)) {
		lexer.scanToken();
		ASTNode* child = parseUnary(currGroup);
		return new UnaryNode(child, currGroup->findOp[currToken]);
	}
	return (this->*(currGroup + 1)->parserFunc)(currGroup + 1);
}
ASTNode* Parser::parseBinLeft(precedenceGroup* currGroup) {
	ASTNode* nodeA = (this->*(currGroup+1)->parserFunc)(currGroup + 1);
	while (1) {
		TokenType currToken = lexer.getCurrToken().getType();
		if (currGroup->findOp.contains(currToken)) {
			lexer.scanToken();
			ASTNode* nodeB = (this->*(currGroup+1)->parserFunc)(currGroup + 1);
			ASTNode* tmpNode = new BinaryNode(nodeA, nodeB, currGroup->findOp[currToken]);
			nodeA = tmpNode;
		}
		else
			return nodeA;
	}
}
ASTNode* Parser::parseBinRight(precedenceGroup* currGroup) {
	ASTNode* nodeA = (this->*(currGroup+1)->parserFunc)(currGroup + 1);
	TokenType currToken = lexer.getCurrToken().getType();
	if (currGroup->findOp.contains(currToken)) {
		lexer.scanToken();
		ASTNode* nodeB = (this->*(currGroup)->parserFunc)(currGroup);
		return new BinaryNode(nodeA, nodeB, currGroup->findOp[currToken]);
	}
	return nodeA;
}
ASTNode* Parser::parsePrimary(precedenceGroup* currGroup) {
	ASTNode* node;
	switch (lexer.getCurrToken().getType()) {
	case TokenType::INT_LIT:
		node = new LiteralNode(std::stoi(lexer.getCurrToken().getLexeme()));
		lexer.scanToken();
		break;
	case TokenType::L_PAREN:
		lexer.scanToken();
		node = (this->*precedenceTab[0].parserFunc)(precedenceTab);
		if (node) {
			node->setForceRval(true); // (myVar) = 5 should not be a valid syntax
		}
		if (lexer.getCurrToken().getType() == TokenType::R_PAREN) {
			lexer.scanToken();
		}
		else node = nullptr;
		break;
	case TokenType::ID:
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

