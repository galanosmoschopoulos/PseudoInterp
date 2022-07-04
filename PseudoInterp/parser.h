#pragma once
#include "objects.h"
#include "operators.h"
#include "lexer.h"
#include "AST.h"
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

class CodeBlock;
class ASTNode;
class Parser;

#define MAX_GROUPS 15
#define MAX_OPS_PER_GROUP 20

using OT = OperatorType;
using TT = TokenType;

class Parser {
public:
	Parser();
	ASTNode* getAST(const std::string&);

private:
	Lexer lexer;

	struct precedenceGroup {
	public:
		/*TokenType tokenTypes[MAX_OPS_PER_GROUP];
		OperatorType opTypes[MAX_OPS_PER_GROUP];*/
		std::map<TokenType, OperatorType> findOp;
		ASTNode* (Parser::* parserFunc)(precedenceGroup*);
	};
	precedenceGroup precedenceTab[MAX_GROUPS] = {
		{  {{TT::OUTPUT, OT::OUTPUT}}, &Parser::parseUnary},

		{  {{TT::EQ, OT::ASSIGNMENT}}, &Parser::parseBinRight},

		{  {{TT::DOUBLE_VERT_SLASH, OT::OR}}, &Parser::parseBinLeft},

		{  {{TT::DOUBLE_AMP, OT::AND}}, &Parser::parseBinLeft},

		{  {{TT::DOUBLE_EQ, OT::EQUAL},
		   {TT::NOT_EQ, OT::NOT_EQUAL}}, &Parser::parseBinLeft},

		{  {{TT::LESS, OT::LESS},
			{TT::LESS_EQ, OT::LESS_EQ},
			{TT::GRE, OT::GREATER},
			{TT::GRE_EQ, OT::GRE_EQ}}, &Parser::parseBinLeft},

		{  {{TT::PLUS, OT::ADDITION},
		    {TT::MINUS, OT::SUBTRACTION}}, &Parser::parseBinLeft},

		{  {{TT::STAR, OT::MULTIPLICATION},
		    {TT::FORW_SLASH, OT::DIVISION},
		    {TT::PERCENT, OT::MODULO}}, &Parser::parseBinLeft },

		{   {{TT::EXMARK, OT::NOT}}, &Parser::parseUnary},

		{  {{TT::PLUS, OT::UNARY_PLUS},
			{TT::MINUS, OT::UNARY_NEGATION}}, &Parser::parseUnary},

		{{}, &Parser::parsePrimary}
	};

	ASTNode* parseUnary(precedenceGroup* currGroup);
	ASTNode* parseBinLeft(precedenceGroup* currGroup);
	ASTNode* parseBinRight(precedenceGroup* currGroup);
	ASTNode* parsePrimary(precedenceGroup* currGroup);
};

