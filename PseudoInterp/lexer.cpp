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

void Lexer::scanToken(const int n) { tokenListIndex+=n; } // Goes to next token
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
			subStrVec.push_back(subStr + '\n');
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
		std::string tmpLexeme;
		if(str[i] == '.')
		{
			tmpLexeme.push_back(str[i++]);
			while (isdigit(str[i]) && i < str.size()) // Store all continuous digits
				tmpLexeme.push_back(str[i++]);
			tokenList.emplace_back(tmpLexeme, TokenType::FLOAT_LIT, i - tmpLexeme.size()); // Add a num literal token
		}

		bool foundFixedToken = false;
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
			TokenType tType = TokenType::INT_LIT;
			while (isdigit(str[i]) && i < str.size()) // Store all continuous digits
				tmpLexeme.push_back(str[i++]);
			if (str[i] == '.')
			{
				tType = TokenType::FLOAT_LIT;
				tmpLexeme.push_back(str[i++]);
			}
			while (isdigit(str[i]) && i < str.size()) // Store all continuous digits
				tmpLexeme.push_back(str[i++]);

			tokenList.emplace_back(tmpLexeme, tType, i - tmpLexeme.size()); // Add a num literal token
		}
		else if (isalpha(str[i]) || str[i] == '_') // If it starts with letter or _
		{
			while (isalnum(str[i]) || str[i] == '_') // If it continues with alphanumeric or _
				tmpLexeme.push_back(str[i++]);
			tokenList.emplace_back(tmpLexeme, TokenType::ID, i - tmpLexeme.size()); // Add identifier token
		}
		else if (str[i] == '\'')
		{
			i++;
			tokenList.emplace_back(std::string(1, str[i]), TokenType::CHAR_LIT, i - tmpLexeme.size());
			if (str[++i] != '\'') throw std::runtime_error("Lexing error: char literal not defined correctly.");
			i++;
		}
		else if (str[i] == '\"')
		{
			i++;
			while (str[i] != '\"')
			{
				if (str[i] == '\\')
				{
#define IS_OCTAL(N)((N) >= '0' && (N) <= '7')
#define IS_HEX(N)(((N) >= '0' && (N) <= '9') || ((N) >= 'A' && (N) <= 'F') || ((N) >= 'a' && (N) <= 'f'))
					i++;
					if (IS_OCTAL(str[i]))
					{
						int charNum = 0;
						while (IS_OCTAL(str[i]))
						{
							charNum = 8 * charNum + str[i++] - '0';
						}
						tmpLexeme.push_back(static_cast<char>(charNum));
					}
					else if (std::tolower(str[i]) == 'x')
					{
						i++;
						int charNum = 0;
						while (IS_HEX(str[i]))
						{
							charNum = 16 * charNum + str[i++] - '0';
						}
						tmpLexeme.push_back(static_cast<char>(charNum));
					}
					else {
						switch (str[i])
						{
						case 'n':
							tmpLexeme.push_back('\n');
							break;
						case 't':
							tmpLexeme.push_back('\t');
							break;
						case 'a':
							tmpLexeme.push_back('\a');
							break;
						case 'b':
							tmpLexeme.push_back('\b');
							break;
						case 'f':
							tmpLexeme.push_back('\f');
							break;
						case 'r':
							tmpLexeme.push_back('\r');
							break;
						case 'v':
							tmpLexeme.push_back('\v');
							break;
						case '\\':
							tmpLexeme.push_back('\\');
							break;
						case '\?':
							tmpLexeme.push_back('\?');
							break;
						case '\'':
							tmpLexeme.push_back('\'');
							break;
						case '\"':
							tmpLexeme.push_back('\"');
							break;
						default:
							throw std::runtime_error("Lexing error: unknown ASCII escape sequence.");
							break;
						}
						i++;
					}
				}
				else
					tmpLexeme.push_back(str[i++]);
			}
			i++;
			tokenList.emplace_back(tmpLexeme, TokenType::STRING_LIT, i - tmpLexeme.size()); // Add identifier token
		}

		else if (isspace(str[i]))
		{
			for (; isspace(str[i]) && i < str.size(); i++); // Whitespaces are ignored
		}
		else
		{
			tokenList.emplace_back(std::string(1, str[i]), TokenType::UNKNOWN, i);
			++i;
			// If it doesn't match the above, there's a problem
		}
	}
}

std::string Lexer::getErrorLine(size_t errPos, int offset) const
{
	errPos += offset;
	std::stringstream ss;
	size_t currLen = 0, nlines = 0, posInLine = 0;
	for(const auto& str : subStrVec)
	{
		if (currLen + str.size() <= errPos)
		{
			currLen += str.size();
			nlines++;
		}
		else
		{
			posInLine =  errPos - currLen;
			break;
		}
	}
	if (nlines >= subStrVec.size()) {
		nlines = subStrVec.size() - 1;
		posInLine = subStrVec.rbegin()->size() - 1;
	}
	else if (posInLine >= subStrVec[nlines].size()) posInLine = subStrVec[nlines].size() - 1;

	ss << "Line: " << nlines + sumOfDeletedStrs[nlines] + 1 << '\n';
	std::string s = subStrVec[nlines];
	std::ranges::replace(s.begin(), s.end(), '\t', ' ');
	ss << s;
	for (size_t i = 0; i < posInLine; ++i) ss << ' ';
	ss << "^";
	return ss.str();
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
	default: return TokenType::NEWLINE;
	}
}

Token::Token(const std::string& tokStr, const TokenType tokType, const size_t tokPos) : pos(tokPos)
{
	lexeme = tokStr;
	type = tokType;
}

size_t Token::getPos() const { return pos; }
