#pragma once
#include <iostream>
#include <string>
#include <vector>

enum class TokenType
{
	// Enumeration of all token types
	RETURN_TOK,
	FUNCTION_DEF,
	L_PAREN,
	R_PAREN,
	L_SQ_BRACKET,
	R_SQ_BRACKET,
	COMMA,
	DOT,
	TILDA,
	CIRCUMFLEX,
	DOUBLE_PLUS,
	DOUBLE_MINUS,
	PLUS,
	MINUS,
	STAR,
	FORW_SLASH,
	PERCENT,
	PLUS_EQ,
	MINUS_EQ,
	STAR_EQ,
	FORW_SLASH_EQ,
	PERCENT_EQ,
	DOUBLE_AMP,
	AMP,
	DOUBLE_VERT_SLASH,
	VERT_SLASH,
	EXMARK,
	EQ,
	DOUBLE_EQ,
	NOT_EQ,
	LESS_EQ,
	GRE_EQ,
	LESS,
	GRE,
	LSHFT,
	RSHFT,
	INT_LIT,
	FLOAT_LIT,
	CHAR_LIT,
	ID,
	WHSPACE,
	TAB,
	NEWLINE,
	OUTPUT,
	WHILE,
	IF,
	ELIF,
	ELSE,
	FOR,
	FROM,
	TO,
	EOFILE,
	STRING_LIT,
	TRUE_LIT,
	FALSE_LIT,
	UNKNOWN
};

class TokenDescriptor
{
	// Connects a lexeme (string) to a token type
public:
	TokenDescriptor();
	TokenDescriptor(std::string, TokenType);
	std::string getLexeme();
	[[nodiscard]] size_t getLen() const;
	[[nodiscard]] TokenType getType() const;
	[[nodiscard]] TokenType getOppositeType() const;
protected:
	std::string lexeme;
	TokenType type = TokenType::UNKNOWN;
};

class Token : public TokenDescriptor
{
	// Used for actual tokens found in the input string
public:
	Token() = default;
	Token(const std::string&, TokenType, size_t);
	[[nodiscard]] size_t getPos() const;
private:
	size_t pos = 0; // The position of the token in the input string
};

class Lexer
{
public:
	Lexer();
	explicit Lexer(std::string);
	void setInput(const std::string&);
	Token getCurrToken();
	Token lookForw(size_t);
	void scanToken(int n = 1);
	void lexInput();

	void printTokenList() const
	{
		for (auto t : tokenList)
		{
			std::cout << t.getPos() << ":\t" << t.getLexeme() << '\n';
		}
	}

private:
	std::vector<Token> tokenList;
	size_t tokenListIndex = 0;
	std::string str;
	std::vector<TokenDescriptor> fixedTokenList = {
		// A list of keywords, relating lexeme to token type
		TokenDescriptor("+=", TokenType::PLUS_EQ),
		TokenDescriptor("-=", TokenType::MINUS_EQ),
		TokenDescriptor("*=", TokenType::STAR_EQ),
		TokenDescriptor("/=", TokenType::FORW_SLASH_EQ),
		TokenDescriptor("%=", TokenType::PERCENT_EQ),
		TokenDescriptor("++", TokenType::DOUBLE_PLUS),
		TokenDescriptor("--", TokenType::DOUBLE_MINUS),
		TokenDescriptor("+", TokenType::PLUS),
		TokenDescriptor("-", TokenType::MINUS),
		TokenDescriptor("*", TokenType::STAR),
		TokenDescriptor("/", TokenType::FORW_SLASH),
		TokenDescriptor("%", TokenType::PERCENT),
		TokenDescriptor("(", TokenType::L_PAREN),
		TokenDescriptor(")", TokenType::R_PAREN),
		TokenDescriptor("[", TokenType::L_SQ_BRACKET),
		TokenDescriptor("]", TokenType::R_SQ_BRACKET),
		TokenDescriptor("||", TokenType::DOUBLE_VERT_SLASH),
		TokenDescriptor("&&", TokenType::DOUBLE_AMP),
		TokenDescriptor("==", TokenType::DOUBLE_EQ),
		TokenDescriptor("=", TokenType::EQ),
		TokenDescriptor("<<", TokenType::LSHFT),
		TokenDescriptor(">>", TokenType::RSHFT),
		TokenDescriptor("!=", TokenType::NOT_EQ),
		TokenDescriptor("!", TokenType::EXMARK),
		TokenDescriptor("<=", TokenType::LESS_EQ),
		TokenDescriptor(">=", TokenType::GRE_EQ),
		TokenDescriptor("<", TokenType::LESS),
		TokenDescriptor(">", TokenType::GRE),
		TokenDescriptor(",", TokenType::COMMA),
		TokenDescriptor(".", TokenType::DOT),
		TokenDescriptor("\t", TokenType::TAB),
		TokenDescriptor("\n", TokenType::NEWLINE),
		TokenDescriptor("while", TokenType::WHILE),
		TokenDescriptor("if", TokenType::IF),
		TokenDescriptor("elif", TokenType::ELIF),
		TokenDescriptor("else", TokenType::ELSE),
		TokenDescriptor("for", TokenType::FOR),
		TokenDescriptor("from", TokenType::FROM),
		TokenDescriptor("to", TokenType::TO),
		TokenDescriptor("true", TokenType::TRUE_LIT),
		TokenDescriptor("false", TokenType::FALSE_LIT),
		TokenDescriptor("return", TokenType::RETURN_TOK),
		TokenDescriptor("function", TokenType::FUNCTION_DEF)
	};
};
