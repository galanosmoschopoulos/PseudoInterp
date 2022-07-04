#pragma once
#include <string>
#include <vector>
#include <array>

using std::array;
using std::string;
using std::vector;

enum class TokenType {
    L_PAREN,
    R_PAREN,
    L_SQ_BRACKET,
    R_SQ_BRACKET,
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
    ID,
    WHSPACE,
    OUTPUT,
    EOFILE,
    UNKNOWN
};

class TokenDescriptor {
public:
    TokenDescriptor();
    TokenDescriptor(const string& tokStr, TokenType tokType);
    string getLexeme();
    int getLen();
    TokenType getType();
protected:
    string lexeme;
    TokenType type;

};

class Token : public TokenDescriptor {
public:
    Token() {}
    Token(const string& tokStr, TokenType tokType, int tokPos);
    int getPos();
private:
    int pos = 0;
};

class Lexer {
public:
    Lexer();
    Lexer(const string& str);
    void setInput(const string& input);
    Token getCurrToken();
    void scanToken();
    void lexInput();
private:
    vector<Token> tokenList;
    int tokenListIndex = 0;
    string str;
    vector<TokenDescriptor> fixedTokenList = {
            TokenDescriptor("+", TokenType::PLUS),
            TokenDescriptor("-", TokenType::MINUS),
            TokenDescriptor("*", TokenType::STAR),
            TokenDescriptor("/", TokenType::FORW_SLASH),
            TokenDescriptor("(", TokenType::L_PAREN),
            TokenDescriptor(")", TokenType::R_PAREN),
            TokenDescriptor("%", TokenType::PERCENT),
            TokenDescriptor("||", TokenType::DOUBLE_VERT_SLASH),
            TokenDescriptor("&&", TokenType::DOUBLE_AMP),
            TokenDescriptor("=", TokenType::EQ),
            TokenDescriptor("==", TokenType::DOUBLE_EQ),
            TokenDescriptor("<<", TokenType::LSHFT),
            TokenDescriptor(">>", TokenType::RSHFT),
            TokenDescriptor("!=", TokenType::NOT_EQ),
            TokenDescriptor("<=", TokenType::LESS_EQ),
            TokenDescriptor(">=", TokenType::GRE_EQ),
            TokenDescriptor("<", TokenType::LESS),
            TokenDescriptor(">", TokenType::GRE),
            TokenDescriptor("output", TokenType::OUTPUT),

    };
};

