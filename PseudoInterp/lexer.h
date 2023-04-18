/* lexer.h */

#pragma once
#include <string>
#include <vector>

class Lexer
{
public:
	Lexer();

	enum class TokenType
	{
		// Enumeration of all token types, used by the parser to connect
		// to corresponding operators
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
		DIV,
		DIV_EQ,
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
		WHILE,
		IF,
		THEN,
		ELIF,
		ELSE,
		AND,
		OR,
		NOT,
		MOD,
		FOR,
		FROM,
		TO,
		EOFILE,
		STRING_LIT,
		TRUE_LIT,
		FALSE_LIT,
		COMMENT,
		UNKNOWN
	};


	class TokenDescriptor
	{
		// Connects a lexeme (string) to a token type
	public:
		TokenDescriptor();
		TokenDescriptor(std::string, TokenType);
		TokenDescriptor(std::string, TokenType, bool wordToken);
		std::string getLexeme(); // Returns the actual token text = lexeme
		[[nodiscard]] size_t getLen() const;
		[[nodiscard]] TokenType getType() const;
		[[nodiscard]] TokenType getOppositeType() const; // I.e. if '(' return ')'
		bool isWordToken();
	protected:
		std::string lexeme;
		TokenType type = TokenType::UNKNOWN;
		bool wordToken = false; // A word token is expected to end with a
		// white space, in contrast to a symbol token (i.e. '+')
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


	explicit Lexer(std::string);
	void setInput(const std::string&);
	Token getCurrToken();
	Token lookForw(size_t); // Look forward in the token list
	void scanToken(int n = 1); // Proceed to next token
	void lexInput(); // Run elxer

private:
	std::string lexChar(size_t& i) const; // Used to parse ASCII sequences
	std::vector<Token> tokenList; // Contains a series of tokens
	size_t tokenListIndex = 0; // This increments
	std::string str; // The code to be lexed
	std::vector<TokenDescriptor> fixedTokenList = {
		// A list of keywords, relating lexeme to token type
		TokenDescriptor("loop while", TokenType::WHILE, true),
		//TokenDescriptor("while", TokenType::WHILE, true),
		TokenDescriptor("if", TokenType::IF, true),
		TokenDescriptor("then", TokenType::THEN, true),
		TokenDescriptor("else if", TokenType::ELIF, true),
		TokenDescriptor("else", TokenType::ELSE, true),
		TokenDescriptor("//", TokenType::COMMENT),
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
		//TokenDescriptor("div=", TokenType::DIV_EQ),
		TokenDescriptor("div", TokenType::DIV, true),
		TokenDescriptor("mod", TokenType::MOD, true),
		TokenDescriptor("and", TokenType::AND, true),
		TokenDescriptor("or", TokenType::OR, true),
		TokenDescriptor("not", TokenType::NOT, true),
		TokenDescriptor("loop for", TokenType::FOR, true),
		//TokenDescriptor("for", TokenType::FOR, true),
		TokenDescriptor("from", TokenType::FROM, true),
		TokenDescriptor("to", TokenType::TO, true),
		TokenDescriptor("true", TokenType::TRUE_LIT, true),
		TokenDescriptor("false", TokenType::FALSE_LIT, true),
		TokenDescriptor("return", TokenType::RETURN_TOK, true),
		TokenDescriptor("method", TokenType::FUNCTION_DEF, true),
	};
};
