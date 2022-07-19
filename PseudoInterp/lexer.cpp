#include "lexer.h"
#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
Lexer::Lexer() = default;

Lexer::Lexer(std::string strIn) : originalStr(std::move(strIn))
{
	str = preprocessStr(originalStr);
}

void Lexer::setInput(const std::string& strIn)
{
	originalStr = strIn;
	str = preprocessStr(originalStr);
}

void Lexer::scanToken() { tokenListIndex++; } // Goes to next token
Token Lexer::lookForw(const size_t i) { return tokenList[tokenListIndex + i]; } // Returns the ith next token
Token Lexer::getCurrToken() { return Lexer::lookForw(0); } // Returns current token

std::string Lexer::preprocessStr(const std::string& initStr)
// Removes blank lines and trailing spaces, adds newline to the end.
{
	subStrVec.erase(subStrVec.begin(), subStrVec.end());
	std::stringstream ss(initStr);
	std::string subStr, finalStr;
	const std::string whitespaces(" \t\f\v\n\r");
	int deletedLines = 0;
	while (std::getline(ss, subStr, '\n'))
	{
		if (std::ranges::any_of(subStr, ::isgraph))
		{
			// If the string contains graphic characters (i.e. not just spaces)
			subStr.erase(subStr.find_last_not_of(whitespaces) + 1);
			finalStr += subStr + "\n"; // Add to new string
			subStrVec.push_back(subStr);
			sumOfDeletedStrs.push_back(deletedLines);
		}
		else deletedLines++;
	}
	return finalStr;
}

void Lexer::lexInput()
{
	tokenList.erase(tokenList.begin(), tokenList.end()); // Resets tokenList
	tokenListIndex = 0;

	for (size_t i = 0; ;)
	{
		if (i == str.size()) // If it has finished, write EOFILE to list
		{
			tokenList.emplace_back("", TokenType::EOFILE, i);
			break;
		}
		bool foundFixedToken = false;
		std::string tmpLexeme;
		for (TokenDescriptor& td : fixedTokenList) // Check every fixed token (i.e. keywords)
		{
			size_t p = 0;
			for (; p < td.getLen() && (td.getLexeme()[p] == str[i + p]) && (i + p < str.size()); p++);
			// Count the matching characters
			if (td.getLen() == p) // If all match
			{
				foundFixedToken = true;
				tokenList.emplace_back(td.getLexeme(), td.getType(), i); // add token
				i += static_cast<int>(td.getLen());
				break;
			}
		}
		if (foundFixedToken) continue;
		if (isdigit(str[i])) // If it starts with a digit
		{
			while (isdigit(str[i]) && i < str.size()) // Store all continuous digits
				tmpLexeme.push_back(str[i++]);
			tokenList.emplace_back(tmpLexeme, TokenType::INT_LIT, i - tmpLexeme.size()); // Add a num literal token
		}
		else if (isalpha(str[i]) || str[i] == '_') // If it starts with letter or _
		{
			while (isalnum(str[i]) || str[i] == '_') // If it continues with alphanumeric or _
				tmpLexeme.push_back(str[i++]);
			tokenList.emplace_back(tmpLexeme, TokenType::ID, i - tmpLexeme.size()); // Add identifier token
		}
		else if (isspace(str[i]))
		{
			for (; isspace(str[i]) && i < str.size(); i++); // Whitespaces are ignored
		}
		else
		{
			tokenList.emplace_back(std::string(1, str[i]), TokenType::UNKNOWN, i);
			// If it doesn't match the above, there's a problem
		}
	}
}

TokenDescriptor::TokenDescriptor() = default;

TokenDescriptor::TokenDescriptor(std::string tokStr, const TokenType tokType) : lexeme(std::move(tokStr)), type(tokType)
{
}

std::string TokenDescriptor::getLexeme() { return lexeme; }
size_t TokenDescriptor::getLen() const { return lexeme.size(); }
TokenType TokenDescriptor::getType() const { return type; }

TokenType TokenDescriptor::getOppositeType() const
{
	switch (type)
	{
	case TokenType::L_PAREN: return TokenType::R_PAREN;
	case TokenType::R_PAREN: return TokenType::L_PAREN;
	case TokenType::L_SQ_BRACKET: return TokenType::R_SQ_BRACKET;
	case TokenType::R_SQ_BRACKET: return TokenType::L_SQ_BRACKET;
	default: return TokenType::UNKNOWN;
	}
}

Token::Token(const std::string& tokStr, const TokenType tokType, const size_t tokPos) : pos(tokPos)
{
	lexeme = tokStr;
	type = tokType;
}

size_t Token::getPos() const { return pos; }
