#include "Lexer.h"
#include <sstream>
#include <string>
#include <ranges>
#include <algorithm>

namespace views = std::ranges::views;

bool Lexer::isalphaNum_(const char ch) {
	return ::isalnum(ch) || ch == '_';
}

bool Lexer::isalpha_(const char ch) {
	return ::isalpha(ch) || ch == '_';
}

const std::array<std::string, 10> Lexer::keywords = {
	"create",
	"update",
	"delete",
	"database",
	"table",
	"function", // function ;
};

const std::array<std::string, 4> Lexer::literals = {
	"null",
	"undefined",
	"true",
	"false"
};

const std::array<std::string, 14> Lexer::punctuators = {
	";",
	"\"", "\'","`",
	"=",
	"{","}","[","]","(",")",
	".",
	",",
	":"
};

const std::array<std::string, 16> Lexer::binaryOps = {
	"**",
	"*", "/", "%",
	"+", "-",
	">>", "<<", "&", "|", "^",
	">", "<", "==", "===",
	"\\"
};

std::vector<Token>* Lexer::tokenize(std::string& sourceCode) {
	Mode mode = Mode::SPACE;
	std::vector<Token>* p_tks = new std::vector<Token>();

	std::string tkVal = "";

	for (size_t i = 0; i < sourceCode.length(); i++) {
		const char& currentChar = sourceCode.at(i);
		const char& nextChar = (i == sourceCode.length() - 1) ? '\n' : sourceCode.at(i + 1);

		mode = changeMode(currentChar, nextChar, mode);

		if (mode != Mode::SPACE) {
			tkVal += currentChar;
		}

		if (checkIsDeli(currentChar, nextChar, mode, tkVal)) {
			if (!(mode == Mode::SPACE && isspace(currentChar)) &&
				  mode != Mode::LINE_COMMENT_CLOSE && mode != Mode::BLOCK_COMMENT_CLOSE
				) {
				TokenType tkType = getTokenType(tkVal, mode);
				if (tkType == TokenType::KEYWORD) {
					std::transform(tkVal.begin(), tkVal.end(), tkVal.begin(), ::tolower);
				}
				p_tks->emplace_back(tkType, tkVal);
			}
			tkVal = "";
		}
	}

	p_tks->emplace_back(TokenType::END_OF_FILE, "endOfFile");
	return p_tks;
}

TokenType Lexer::getLiteralType(const std::string& value) {
	if (value == "undefined") return TokenType::UNDEFINED_LITERAL;
	if (value == "null") return TokenType::NULL_LITERAL;
	if (value == "true") return TokenType::BOOLEAN_LITERAL;
	if (value == "false") return TokenType::BOOLEAN_LITERAL;
	else throw "DAMIT";
}

bool Lexer::checkIsDeli(const char currentChar, const char nextChar, const LexerMode mode, const std::string& partialToken) {
	switch (mode) {
	case LexerMode::ALPHA:
		if (isalphaNum_(nextChar)) return false;
		return true;
	case LexerMode::NUMBER:
		return !isdigit(nextChar);
	case LexerMode::SYMBOL: {
		if (isspace(nextChar)) return true;
		auto lengthGe = [&partialToken](const std::string& t)
			{return t.length() >= partialToken.length(); };
		auto isPrefixOf = [&partialToken](const std::string& t)
			{return std::mismatch(partialToken.begin(), partialToken.end(), t.begin()).first == partialToken.end(); };
		auto possibleSymPunc =
			Lexer::punctuators
			| views::filter(lengthGe)
			| views::filter(isPrefixOf);

		auto sameLengthPunc =
			possibleSymPunc
			| views::filter([&partialToken](const std::string& punc)
				{return punc.length() == partialToken.length(); });

		auto possibleSymBinaryOps =
			Lexer::binaryOps
			| views::filter(lengthGe)
			| views::filter(isPrefixOf);

		auto sameLengthPuncBinaryOps = possibleSymBinaryOps
			| views::filter([&partialToken](const std::string& ops)
				{return ops.length() == partialToken.length(); });

		if (possibleSymPunc.empty() && possibleSymBinaryOps.empty()) return false;
		if (!sameLengthPunc.empty()) return true;
		if (!sameLengthPuncBinaryOps.empty()) return true;
		return false;
	} return false;
	case LexerMode::SPACE:
		return true;
	case LexerMode::STRING_SINGLE_QUOTE_OPEN:
	case LexerMode::STRING_DOUBLE_QUOTE_OPEN:
	case LexerMode::STRING_TICK_OPEN:
		return false;
	case LexerMode::STRING_SINGLE_QUOTE_CLOSE:
	case LexerMode::STRING_DOUBLE_QUOTE_CLOSE:
	case LexerMode::STRING_TICK_CLOSE:
		return true;
	case LexerMode::LINE_COMMENT_OPEN:
		return currentChar == '\n';
	case LexerMode::LINE_COMMENT_CLOSE:
		return true;
	case LexerMode::BLOCK_COMMENT_OPEN:
		return currentChar == '*' && nextChar == '/';
	case LexerMode::BLOCK_COMMENT_CLOSE:
		return true;
	default:
		throw "Technically imposibble to get here";
		return true;
	}
}

LexerMode Lexer::changeMode(const char currentChar, const char nextChar, const LexerMode mode) {
	if (currentChar == '/' && nextChar == '/') return LexerMode::LINE_COMMENT_OPEN;
	if (currentChar == '/' && nextChar == '*') return LexerMode::BLOCK_COMMENT_OPEN;

	if (mode != LexerMode::LINE_COMMENT_OPEN && mode != LexerMode::BLOCK_COMMENT_OPEN) {
		if (mode != LexerMode::STRING_SINGLE_QUOTE_OPEN &&
			currentChar == '\''
			) return LexerMode::STRING_SINGLE_QUOTE_OPEN;
		if (mode != LexerMode::STRING_DOUBLE_QUOTE_OPEN &&
			currentChar == '\"'
			) return LexerMode::STRING_DOUBLE_QUOTE_OPEN;
		if (mode != LexerMode::STRING_TICK_OPEN &&
			currentChar == '`'
			) return LexerMode::STRING_TICK_OPEN;

		if (mode != LexerMode::STRING_SINGLE_QUOTE_OPEN &&
			mode != LexerMode::STRING_DOUBLE_QUOTE_OPEN &&
			mode != LexerMode::STRING_TICK_OPEN &&
			!isdigit(currentChar) &&
			!isalpha_(currentChar) &&
			!isspace(currentChar)
			) return LexerMode::SYMBOL;
	}

	switch (mode) {
	case LexerMode::NUMBER:
		return isspace(currentChar) ? LexerMode::SPACE : isdigit(currentChar) ? mode : LexerMode::ALPHA;
	case LexerMode::ALPHA:
		return isalnum(currentChar) ? mode : LexerMode::SPACE;
	case LexerMode::SYMBOL:
		return
			isdigit(currentChar) ? LexerMode::NUMBER :
			isalpha_(currentChar) ? LexerMode::ALPHA :
			isspace(currentChar) ? LexerMode::SPACE : mode;
	case LexerMode::SPACE:
	case LexerMode::STRING_SINGLE_QUOTE_CLOSE:
	case LexerMode::STRING_DOUBLE_QUOTE_CLOSE:
	case LexerMode::STRING_TICK_CLOSE:
	case LexerMode::LINE_COMMENT_CLOSE:
	case LexerMode::BLOCK_COMMENT_CLOSE:
		return
			isdigit(currentChar) ? LexerMode::NUMBER :
			isalpha_(currentChar) ? LexerMode::ALPHA : LexerMode::SPACE;
	case LexerMode::STRING_SINGLE_QUOTE_OPEN:
		return currentChar == '\'' ? LexerMode::STRING_SINGLE_QUOTE_CLOSE : mode;
	case LexerMode::STRING_DOUBLE_QUOTE_OPEN:
		return currentChar == '\"' ? LexerMode::STRING_SINGLE_QUOTE_CLOSE : mode;
	case LexerMode::STRING_TICK_OPEN:
		return currentChar == '`' ? LexerMode::STRING_SINGLE_QUOTE_CLOSE : mode;
	case LexerMode::LINE_COMMENT_OPEN:
		return currentChar == '\n' ? LexerMode::LINE_COMMENT_CLOSE : mode;
	case LexerMode::BLOCK_COMMENT_OPEN:
		return currentChar == '*' && nextChar == '/' ? LexerMode::BLOCK_COMMENT_CLOSE : mode;
	default:
		return mode;
	}
}

TokenType Lexer::getTokenType(const std::string& val, const LexerMode mode) {
	std::string loweredVal = val;
	std::transform(val.begin(), val.end(), loweredVal.begin(), tolower);

	switch (mode) {
	case LexerMode::ALPHA:
		if (std::find(Lexer::literals.begin(), Lexer::literals.end(), val) != Lexer::literals.end()) return getLiteralType(val);
		if (std::find(Lexer::keywords.begin(), Lexer::keywords.end(), loweredVal) != Lexer::keywords.end()) return TokenType::KEYWORD;
		return TokenType::IDENTIFIER;
	case LexerMode::NUMBER:
		return TokenType::NUMERIC_LITERAL;
	case LexerMode::SYMBOL:
		if (std::find(Lexer::punctuators.begin(), Lexer::punctuators.end(), val) != Lexer::punctuators.end()) return TokenType::PUNCTUATOR;
		if (std::find(Lexer::binaryOps.begin(), Lexer::binaryOps.end(), val) != Lexer::binaryOps.end()) return TokenType::BINARY_OPERATOR;
		return TokenType::IDENTIFIER;
	case LexerMode::STRING_SINGLE_QUOTE_CLOSE:
	case LexerMode::STRING_DOUBLE_QUOTE_CLOSE:
	case LexerMode::STRING_TICK_CLOSE:
		return TokenType::STRING_LITERAL;
	default:
		/* Token should not be pushed in these modes ;
			STRING_SINGLE_QUOTE_OPEN,
			STRING_DOUBLE_QUOTE_OPEN,
			STRING_TICK_OPEN,
			LINE_COMMENT_OPEN,
			LINE_COMMENT_CLOSE,
			BLOCK_COMMENT_OPEN,
			BLOCK_COMMENT_CLOSE,
		*/
		throw "Syntax Error, Unknown Error";
		return TokenType::IDENTIFIER;
	}

}