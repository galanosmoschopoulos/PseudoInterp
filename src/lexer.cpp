/* lexer.cpp */

#include "lexer.h"
#include "errors.h"
#include <string>
#include <cctype>
#include <sstream>
#include <algorithm>

Lexer::Lexer() = default;

Lexer::Lexer(std::string strIn) : str(std::move(strIn))
{
}

void Lexer::setInput(const std::string& strIn)
{
	str = strIn;
}

void Lexer::scanToken(const int n) { tokenListIndex += n; }
// Goes to next token
Lexer::Token Lexer::lookForw(const size_t i)
{
	return tokenList[tokenListIndex + i];
} // Returns the ith next token
Lexer::Token Lexer::getCurrToken() { return lookForw(0); }
// Returns current token


void Lexer::lexInput()
{
	tokenList.erase(tokenList.begin(), tokenList.end()); // Resets tokenList
	tokenListIndex = 0;

	for (size_t i = 0; ;)
	{
		if (i == str.size())
		// If it has finished, write End Of File token to list
		{
			tokenList.emplace_back("", TokenType::EOFILE, i);
			break;
		}
		std::string tmpLexeme;
		bool foundFixedToken = false;
		for (TokenDescriptor& td : fixedTokenList)
		// Check every fixed token (i.e. keywords)
		{
			size_t p = 0;
			// Count the matching characters
			for (; p < td.getLen() && (td.getLexeme()[p] == str[i + p]) && (i +
				       p < str.size()); p++);
			if (td.getLen() == p) // If all match
			{
				// A keyword (i.e. 'while') token must end in ' ' or newline, to separate
				// it from the next token.If not, then it is a variable identifier.
				// I.e. 'for i' and 'fori' are different.
				if (td.isWordToken() && str[i + p] != ' ' && str[i + p] != '\n')
					break;

				foundFixedToken = true;
				if (td.getType() == TokenType::COMMENT)
				// If comment, skip until the end of line
				{
					for (; str[i] != '\n' && i < str.size(); i++);
					break;
				}
				tokenList.emplace_back(td.getLexeme(), td.getType(), i);
				//Add the new token
				i += static_cast<int>(td.getLen());
				// Increase the input string index
				break;
			}
		}
		if (foundFixedToken) continue;
		if (isdigit(str[i])) // If it starts with a digit
		{
			auto tType = TokenType::INT_LIT;
			while (isdigit(str[i]) && i < str.size())
				// Store all continuous digits
				tmpLexeme.push_back(str[i++]);
			if (str[i] == '.') // If we meet a decimal separator, it's a float
			{
				tType = TokenType::FLOAT_LIT;
				tmpLexeme.push_back(str[i++]);
			}
			while (isdigit(str[i]) && i < str.size())
				// Store all continuous digits
				tmpLexeme.push_back(str[i++]);

			tokenList.emplace_back(tmpLexeme, tType, i - tmpLexeme.size());
			// Add token a numerical literal token
		}
		else if (isalpha(str[i]) || str[i] == '_')
		// If it starts with letter or _
		{
			while (isalnum(str[i]) || str[i] == '_')
				// If it continues with alphanumeric or _
				tmpLexeme.push_back(str[i++]);
			tokenList.emplace_back(tmpLexeme, TokenType::ID,
			                       i - tmpLexeme.size());
			// Add identifier token
		}
		else if (str[i] == '\'') // If we meet quotation mark
		{
			i++;
			// Use lexChar() to parse a character or an ASCII escape sequence
			tokenList.emplace_back(lexChar(i), TokenType::CHAR_LIT,
			                       i - tmpLexeme.size());
			if (str[i] != '\'') throw LexingError(
				"Lexing error: char literal not defined correctly.", i);
			i++;
		}
		else if (str[i] == '\"') // Parse string literals
		{
			i++;
			while (str[i] != '\"') // Until we meet the closing double quote
			{
				tmpLexeme += lexChar(i);
			}
			i++;
			tokenList.emplace_back(tmpLexeme, TokenType::STRING_LIT,
			                       i - tmpLexeme.size());
			// Add string literal token
		}

		else if (isspace(str[i]))
		{
			for (; isspace(str[i]) && i < str.size(); i++);
			// Whitespaces are ignored
		}
		else
		{
			tokenList.emplace_back(std::string(1, str[i]), TokenType::UNKNOWN,
			                       i);
			++i;
			// If it doesn't match the above, there's a problem
		}
	}
}

std::string Lexer::lexChar(size_t& i) const
{
	std::string tmpChar; // Store the char to be parsed
	if (str[i] == '\\') // This is how ACII sequences start
	{
		// These macros check if an ASCII character is a valid HEX/OCTAL digit
#define IS_OCTAL(N)((N) >= '0' && (N) <= '7')
#define IS_HEX(N)(((N) >= '0' && (N) <= '9') || \
				  ((N) >= 'A' && (N) <= 'F') || \
				  ((N) >= 'a' && (N) <= 'f'))
		i++;
		if (IS_OCTAL(str[i]))
		// Parse the \ooo escape sequence, where ooo is an octal number
		{
			char charNum = 0;
			while (IS_OCTAL(str[i]))
			{
				// Convert octal to decimal. Note: 'k' - '0' returns integer k
				charNum = 8 * charNum + str[i++] - '0'; 
			}
			tmpChar.push_back(charNum);
		}
		else if (std::tolower(str[i]) == 'x')
		// Parse the \xhhh escape sequence, where hhh is a hex number
		{
			i++;
			char charNum = 0;
			while (IS_HEX(str[i]))
			{
				charNum = 16 * charNum + str[i++] - '0'; // Conversion to decimal
			}
			tmpChar.push_back(charNum);
		}
		else
		{
			switch (str[i]) // Account for all the other escape sequences
			{
			case 'n':
				tmpChar.push_back('\n');
				break;
			case 't':
				tmpChar.push_back('\t');
				break;
			case 'a':
				tmpChar.push_back('\a');
				break;
			case 'b':
				tmpChar.push_back('\b');
				break;
			case 'f':
				tmpChar.push_back('\f');
				break;
			case 'r':
				tmpChar.push_back('\r');
				break;
			case 'v':
				tmpChar.push_back('\v');
				break;
			case '\\':
				tmpChar.push_back('\\');
				break;
			case '\?':
				tmpChar.push_back('\?');
				break;
			case '\'':
				tmpChar.push_back('\'');
				break;
			case '\"':
				tmpChar.push_back('\"');
				break;
			default:
				throw LexingError( // Account for ill formed sequence
					"Lexing error: unknown ASCII escape sequence.", i);
				break;
			}
			i++;
		}
	}
	else
		tmpChar.push_back(str[i++]);
	// If not an escape sequence, just push the character
	return tmpChar;
}

/* Multiple constructors, getters and setters */
Lexer::TokenDescriptor::TokenDescriptor() = default;

Lexer::TokenDescriptor::TokenDescriptor(std::string tokStr, const TokenType
                                        tokType) : lexeme(std::move(tokStr)),
                                                   type(tokType)
{
}

Lexer::TokenDescriptor::TokenDescriptor(std::string tokStr, const TokenType
                                        tokType, bool wordToken) :
	lexeme(std::move(tokStr)), type(tokType), wordToken(wordToken)
{
}

std::string Lexer::TokenDescriptor::getLexeme() { return lexeme; }
size_t Lexer::TokenDescriptor::getLen() const { return lexeme.size(); }
Lexer::TokenType Lexer::TokenDescriptor::getType() const { return type; }

Lexer::TokenType Lexer::TokenDescriptor::getOppositeType() const
{ // I.e. the opposite of ( is ), the opposite of { is }
	switch (type)
	{
	case TokenType::L_PAREN: return TokenType::R_PAREN;
	case TokenType::R_PAREN: return TokenType::L_PAREN;
	case TokenType::L_SQ_BRACKET: return TokenType::R_SQ_BRACKET;
	case TokenType::R_SQ_BRACKET: return TokenType::L_SQ_BRACKET;
	default: return TokenType::NEWLINE;
	}
}

bool Lexer::TokenDescriptor::isWordToken()
{
	return wordToken;
}

Lexer::Token::Token(const std::string& tokStr, const TokenType tokType,
                    const size_t tokPos) : pos(tokPos)
{
	lexeme = tokStr;
	type = tokType;
}

size_t Lexer::Token::getPos() const { return pos; }
