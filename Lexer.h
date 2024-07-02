#pragma once
#include <string>
#include <vector>

enum class TokenType {
	NUMERIC_LITERAL,
	BOOLEAN_LITERAL,
	STRING_LITERAL,
	NULL_LITERAL,
	UNDEFINED_LITERAL,
	IDENTIFIER,
	KEYWORD,
	BINARY_OPERATOR,
	PUNCTUATOR,
	END_OF_FILE,
	NAT // No type checking ;
};

struct Token {
	using Type = TokenType;
	Type type;
	std::string val;
};

enum class LexerMode
{
	ALPHA,
	NUMBER,
	SYMBOL,
	SPACE,
	STRING_SINGLE_QUOTE_OPEN,
	STRING_SINGLE_QUOTE_CLOSE,
	STRING_DOUBLE_QUOTE_OPEN,
	STRING_DOUBLE_QUOTE_CLOSE,
	STRING_TICK_OPEN,
	STRING_TICK_CLOSE,
	LINE_COMMENT_OPEN,
	LINE_COMMENT_CLOSE,
	BLOCK_COMMENT_OPEN,
	BLOCK_COMMENT_CLOSE,
};

class Lexer {
public:
	using Mode = LexerMode;

	static std::vector<Token>* tokenize(std::string& sourceCode);

	static TokenType getTokenType(const std::string& value, const LexerMode mode);
	static Lexer::Mode changeMode(const char currentChar, const char nextChar, const LexerMode mode);
	static bool checkIsDeli(const char currentChar, const char nextChar, const LexerMode mode, const std::string& partialToken);
	static TokenType getLiteralType(const std::string& value);
	static bool isalpha_(const char ch);
	static bool isalphaNum_(const char ch);
private:
	Lexer() = default;
	~Lexer() = default;

	static Lexer* p_tokenizer;

	static const std::array<std::string, 11> keywords;
	static const std::array<std::string, 4> literals;
	static const std::array<std::string, 14> punctuators;
	static const std::array<std::string, 16> binaryOps;
};