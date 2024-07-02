#include "Parser.h"
#include "Lexer.h"
#include <print>
#include <algorithm>

Query* Parser::generateAST(std::string& query)
{
	m_currentIndex = 0;
	pm_tokens = Lexer::tokenize(query);
	Query* p_query = new Query;
	while (!eof()) p_query->body.push_back(parseStmt());

	return p_query;
}

Token& Parser::peek()
{
	return pm_tokens->at(m_currentIndex);
}

Token Parser::eat(TokenType tokenType, const std::string& tokenValue, const std::string& errMsg)
{
	Token peeked = peek();
	if (tokenType != TokenType::NAT &&
		(peeked.type != tokenType || tokenValue != "" && peeked.val != tokenValue)
		)
		throw errMsg;
	return pm_tokens->at(m_currentIndex++);
}

bool Parser::eof()
{
	return peek().type == TokenType::END_OF_FILE;
}

_Check_return_ Statement* Parser::parseStmt()
{
	Token tk = peek();

	switch (tk.type)
	{
	case TokenType::KEYWORD:
		if (tk.val == "create")
			return parseCreation();
		else if (tk.val == "use")
			return parseUseDb();
		else if (tk.val == "insert")
			return parseInsert();
		else
			goto stmt_unexpected_token;
		break;
	stmt_unexpected_token:
	default:
		return parseExpr();
	}
}

_Check_return_ Statement* Parser::parseCreation()
{
	eat(); // eat the keyword "create" ;

	Token tk = peek();

	if (tk.val == "database") {
		return parseDbCreation();
	}
	else if (tk.val == "table") {
		return parseTableCreation();
	}

	return nullptr;
}

_Check_return_ Statement* Parser::parseUseDb()
{
	eat(); // eat the keyword "use";
	Use* use = new Use();
	use->dbName = eat(TokenType::IDENTIFIER, "", "syntax error, expected a database name").val;
	return use;
}

Statement* Parser::parseInsert()
{
	eat(); // eat the keyword "insert";
	eat(TokenType::KEYWORD, "into", "syntax error, expected 'INTO' after 'INSERT'");
	Insert* insert = new Insert();
	insert->tableName = eat(TokenType::IDENTIFIER).val;
	if (peek().val == ".") {
		eat(); // eat the "." ;
		insert->dbName = insert->tableName;
		insert->tableName = eat(TokenType::IDENTIFIER, "", "syntax error, expected a table name after database name").val;
	}

	eat(TokenType::PUNCTUATOR, "(", "syntax error, expected a '(' after INSERT INTO table");
	while (true) {
		insert->cols.push_back(eat().val);
		if (peek().val == ")") {
			break;
		}
		
		eat(TokenType::PUNCTUATOR, ",", "Syntax error, expected a ',' or a ')'");
	}

	eat(); // eat the ')' ;

	eat(TokenType::KEYWORD, "values", "insert query should have values, expected 'values'");

	eat(TokenType::PUNCTUATOR, "(", "syntax error, expected a '(' after INSERT INTO table");

	while (true) {
		insert->vals.push_back(parseExpr());
		if (peek().val == ")") {
			break;
		}

		eat(TokenType::PUNCTUATOR, ",", "Syntax error, expected a ',' or a ')'");
	}

	eat(); // eat the ')' ;

	return insert;
}

_Check_return_ Statement* Parser::parseDbCreation()
{
	eat(); // eat the keyword "database" ;
	DbCreator* dbCreator = new DbCreator();
	dbCreator->dbName = eat(TokenType::IDENTIFIER).val;
	return dbCreator;
}

_Check_return_ Statement* Parser::parseTableCreation() {
	eat(); // eat the keyword "table" ;
	TableCreator* tableCreator = new TableCreator();
	tableCreator->tableName = eat(TokenType::IDENTIFIER).val;
	if (peek().val == ".") {
		eat(); // eat the "." ;
		tableCreator->dbName = tableCreator->tableName;
		tableCreator->tableName = eat(TokenType::IDENTIFIER, "", "syntax error, expected a table name after database name").val;
	}

	eat(TokenType::PUNCTUATOR, "(", "Syntax error, expected \'(\' after table name");

	while (peek().val != ")") {
		tableCreator->cols.push_back(parseTableColDclr());
		if (peek().val == ",") {
			eat(TokenType::PUNCTUATOR, ",");
		}
	}

	eat(); // eat the ")" ;
	
	return tableCreator;
}

_Check_return_ TableColDclr* Parser::parseTableColDclr()
{
	TableColDclr* tableColDclr = new TableColDclr();
	tableColDclr->colName = eat(TokenType::IDENTIFIER).val;
	tableColDclr->colType = parseTableColType();
	return tableColDclr;
}

_Check_return_ TableColType* Parser::parseTableColType()
{
	TableColType* tableColType = new TableColType();
	tableColType->typeName = eat(TokenType::IDENTIFIER).val;
	if (tableColType->typeName == "varchar") {
		eat(TokenType::PUNCTUATOR, "(", "syntax error expected \'(\'");
		tableColType->attr = eat(TokenType::NUMERIC_LITERAL).val;
		eat(TokenType::PUNCTUATOR, ")", "syntax error expected \')\'");
	}
	return tableColType;
}

_Check_return_ Expr* Parser::parseExpr() {
	return parseBinExpr();
}

_Check_return_ Expr* Parser::parsePrimaryExpr() {
	TokenType tokenType = peek().type;
	std::string tokenValue = peek().val;

	switch (tokenType) {
	case TokenType::IDENTIFIER: {
		Identifier* p_identifier = new Identifier;
		p_identifier->symbol = eat().val;
		return p_identifier;
	} break;
	case TokenType::NUMERIC_LITERAL: {
		NumericLiteral* p_numericLiteral = new NumericLiteral;
		p_numericLiteral->val = stof(eat().val);
		return p_numericLiteral;
	} break;
	case TokenType::STRING_LITERAL: {
		StringLiteral* p_stringLiteral = new StringLiteral;
		std::string val = eat().val;
		p_stringLiteral->val = val.substr(1, val.length() - 2);
		return p_stringLiteral;
	} break;
	case TokenType::KEYWORD: {
		if (tokenValue == "null") {
			eat();
			return new NullLiteral;
		} else if (tokenValue == "undefined") {
			eat();
			return new UndefinedLiteral;
		} else {
			goto expr_unexpected_token;
		}
	} break;
	case TokenType::PUNCTUATOR: {
		if (tokenValue == "(") {
			eat(); // eat opening paren;
			Expr* exprInParan = parseExpr();

			eat(TokenType::PUNCTUATOR, ")", "Unexpected token found, expected \')\'"); // eat closing paren;
			return exprInParan;
		} else {
			goto expr_unexpected_token;
		}
	} break;
expr_unexpected_token:
	default:
		throw "Unexpected toke found";
	}
}

_Check_return_ Expr* Parser::parseAddExpr() {
	Expr* left = parseMultExpr();
	while (peek().val == "+" || peek().val == "-") {
		std::string op = eat().val;
		Expr* right = parseMultExpr();
		BinExpr* binExpr = new BinExpr;
		binExpr->left = left;
		binExpr->right = right;
		binExpr->op = op;
		left = binExpr;
	};
	return left;
}

_Check_return_ Expr* Parser::parseMultExpr() {
	Expr* left = parsePrimaryExpr();
	while (peek().val == "*" || peek().val == "/" || peek().val == "%") {
		std::string op = eat().val;
		Expr* right = parsePrimaryExpr();
		BinExpr* binExpr = new BinExpr;
		binExpr->left = left;
		binExpr->right = right;
		binExpr->op = op;
		left = binExpr;
	};
	return left;
}

_Check_return_ Expr* Parser::parseBinExpr() {
	return parseAddExpr();
}
