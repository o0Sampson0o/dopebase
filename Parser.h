#pragma once
#include "Lexer.h"
#include "AST.h"

class Parser
{
public:
	Query* generateAST(std::string& query);

	Token& peek();
	Token eat(TokenType tokenType = TokenType::NAT, const std::string& tokenValue = "", const std::string& errMsg = "");
	bool eof();

	_Check_return_ Statement* parseStmt();
	_Check_return_ Statement* parseCreation();
	_Check_return_ Statement* parseUseDb();
	_Check_return_ Statement* parseInsert();
	_Check_return_ Statement* parseDbCreation();
	_Check_return_ Statement* parseTableCreation();
	_Check_return_ TableColDclr* parseTableColDclr();
	_Check_return_ TableColType* parseTableColType();
;
	_Check_return_ Expr* parseExpr();
	_Check_return_ Expr* parseAddExpr();
	_Check_return_ Expr* parseMultExpr();
	_Check_return_ Expr* parsePrimaryExpr();
	_Check_return_ Expr* parseBinExpr();



private:
	size_t m_currentIndex = 0;
	std::vector<Token>* pm_tokens = nullptr;
};

