#include "lexer.h"
#include <string>
#include <cctype>
#include <iostream>

using std::isdigit;
using std::isspace;
using std::isalnum;
using std::isalpha;

Lexer::Lexer() = default;

Lexer::Lexer(const string& str) : str(str)
{
}

void Lexer::setInput(const string& input) { str = input; }
void Lexer::scanToken() { tokenListIndex++; }
Token Lexer::getCurrToken() { return tokenList[tokenListIndex]; }

void Lexer::lexInput()
{
	tokenList.erase(tokenList.begin(), tokenList.end());
	tokenListIndex = 0;
	for (int i = 0; ;)
	{
		if (i == str.size())
		{
			tokenList.push_back(Token("", TokenType::EOFILE, i));
			break;
		}
		bool foundFixedToken = false;
		string tmpLexeme;
		for (TokenDescriptor& td : fixedTokenList)
		{
			int charsMatch = 0;
			for (int p = 0; p < td.getLen() && (td.getLexeme()[p] == str[i + p]) && (i + p < str.size()); p++)
				charsMatch++;
			if (td.getLen() == charsMatch)
			{
				foundFixedToken = true;
				tokenList.push_back(Token(td.getLexeme(), td.getType(), i));
				i += td.getLen();
				break;
			}
		}
		if (foundFixedToken) continue;
		if (isdigit(str[i]))
		{
			while (isdigit(str[i]) && i < str.size())
				tmpLexeme.push_back(str[i++]);
			tokenList.push_back(Token(tmpLexeme, TokenType::INT_LIT, i - tmpLexeme.size()));
		}
		else if (isalpha(str[i]) || str[i] == '_')
		{
			while (isalnum(str[i]) || str[i] == '_')
				tmpLexeme.push_back(str[i++]);
			tokenList.push_back(Token(tmpLexeme, TokenType::ID, i - tmpLexeme.size()));
		}
		else if (isspace(str[i]))
		{
			int spaceCount = 0;
			while (isspace(str[i]) && i < str.size())
				spaceCount++, i++;
			//tokenList.push_back(Token(" ", TokenType::WHSPACE, i - spaceCount));
		}
		else
		{
			//tokenList.push_back(Token(string(1, str[i]), TokenType::UNKNOWN, i));
		}
	}
}

TokenDescriptor::TokenDescriptor() = default;

TokenDescriptor::TokenDescriptor(const string& tokStr, TokenType tokType) : lexeme(tokStr), type(tokType)
{
}

string TokenDescriptor::getLexeme() { return lexeme; }
int TokenDescriptor::getLen() { return lexeme.size(); }
TokenType TokenDescriptor::getType() { return type; }

Token::Token(const string& tokStr, TokenType tokType, int tokPos) : pos(tokPos)
{
	lexeme = tokStr;
	type = tokType;
}

int Token::getPos() { return pos; }
