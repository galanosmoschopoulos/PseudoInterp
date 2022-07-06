#include "lexer.h"
#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
Lexer::Lexer() = default;
Lexer::Lexer(const std::string& strIn) : originalStr(strIn) {
	str = preprocessStr(originalStr);
}
void Lexer::setInput(const std::string& strIn) {
	originalStr = strIn;
	str = preprocessStr(originalStr);
}
void Lexer::scanToken() { tokenListIndex++; } // Goes to next token
Token Lexer::lookForw(int i) { return tokenList[tokenListIndex + i]; } // Returns the ith next token
Token Lexer::getCurrToken() { return Lexer::lookForw(0); } // Returns current token

std::string Lexer::preprocessStr(const std::string& initStr) // Removes blank lines and trailing spaces, adds newline to the end.
{
	subStrVec.erase(subStrVec.begin(), subStrVec.end());
	std::stringstream ss(initStr);
	std::string subStr, finalStr;
	std::string whitespaces(" \t\f\v\n\r");
	int deletedLines = 0;
	while (std::getline(ss, subStr, '\n')) {
		if (std::any_of(std::begin(subStr), std::end(subStr), ::isgraph)) { // If the string contains graphic characters (i.e. not just spaces)
			subStr.erase(subStr.find_last_not_of(whitespaces) + 1);
			finalStr += subStr + "\n"; // Add to new string
			subStrVec.push_back(subStr);
			sumOfDeletedStrs.push_back(deletedLines);
		}
		else deletedLines++;
	}
	return finalStr;
}
std::tuple<int, int, int> Lexer::posToLine(int pos) // Given a character index in a multi-line string, it returns the number of line the character is, and the position of the char in that line
{
	int lineNum = std::count(str.begin(), str.begin() + pos, '\n');
	int realLineNum = lineNum + sumOfDeletedStrs[lineNum];
	// Code missing
	return std::make_tuple(lineNum, realLineNum, 0);
}

std::string Lexer::getInputLine(int lineNum) {
	std::stringstream ss(str);
	std::string subStr;
	for (int i = 0; i <= lineNum; i++, std::getline(ss, subStr, '\n'));
	return subStr;
}
void Lexer::lexInput()
{
	tokenList.erase(tokenList.begin(), tokenList.end()); // Resets tokenList
	tokenListIndex = 0;

	for (int i = 0; ;)
	{
		if (i == str.size()) // If it has finished, write EOFILE to list
		{
			tokenList.push_back(Token("", TokenType::EOFILE, i));
			break;
		}
		bool foundFixedToken = false;
		std::string tmpLexeme;
		for (TokenDescriptor& td : fixedTokenList) // Check every fixed token (i.e. keywords)
		{
			int charsMatch = 0;
			for (int p = 0; p < td.getLen() && (td.getLexeme()[p] == str[i + p]) && (i + p < str.size()); p++) // Count the matching characters
				charsMatch++;
			if (td.getLen() == charsMatch) // If all match
			{
				foundFixedToken = true;
				tokenList.push_back(Token(td.getLexeme(), td.getType(), i)); // add token
				i += static_cast<int>(td.getLen());
				break;
			}
		}
		if (foundFixedToken) continue;
		if (isdigit(str[i])) // If it starts with a digit
		{
			while (isdigit(str[i]) && i < str.size()) // Store all continuous digits
				tmpLexeme.push_back(str[i++]);
			tokenList.push_back(Token(tmpLexeme, TokenType::INT_LIT, i - static_cast<int>(tmpLexeme.size()))); // Add a num literal token
		}
		else if (isalpha(str[i]) || str[i] == '_') // If it starts with letter or _
		{
			while (isalnum(str[i]) || str[i] == '_') // If it continues with alphanumeric or _
				tmpLexeme.push_back(str[i++]);
			tokenList.push_back(Token(tmpLexeme, TokenType::ID, i - static_cast<int>(tmpLexeme.size()))); // Add identifier token
		}
		else if (isspace(str[i]))
		{
			int spaceCount = 0;
			while (isspace(str[i]) && i < str.size()) // Count consequent spaces
				spaceCount++, i++;
			//Whitespaces are ignored
			//tokenList.push_back(Token(" ", TokenType::WHSPACE, i - spaceCount));
		}
		else
		{
			tokenList.push_back(Token(std::string(1, str[i]), TokenType::UNKNOWN, i)); // If it doesn't match the above, there's a problem
		}
	}
}

TokenDescriptor::TokenDescriptor() = default;
TokenDescriptor::TokenDescriptor(const std::string& tokStr, TokenType tokType) : lexeme(tokStr), type(tokType) { }
std::string TokenDescriptor::getLexeme() { return lexeme; }
size_t TokenDescriptor::getLen() { return lexeme.size(); }
TokenType TokenDescriptor::getType() { return type; }

Token::Token(const std::string& tokStr, TokenType tokType, int tokPos) : pos(tokPos)
{
	lexeme = tokStr;
	type = tokType;
}
int Token::getPos() { return pos; }
