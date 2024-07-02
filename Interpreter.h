#pragma once
#include "Parser.h"
#include "RuntimeValue.h"

class Environment;

class Interpreter
{
public:
	Interpreter() = default;
	~Interpreter() = default;

	_Check_return_ Query* parse(std::string& sourceCode);
	_Check_return_ RuntimeValue* eval(Node* astNode, Environment* env);
private:
	_Check_return_ RuntimeValue* evalQuery(Query* program, Environment* env);

	_Check_return_ RuntimeValue* evalDbCreator(DbCreator* dbCreator, Environment* env);
	_Check_return_ RuntimeValue* evalTableCreator(TableCreator* dbCreator, Environment* env);
	_Check_return_ RuntimeValue* evalUseDb(Use* use, Environment* env);
	_Check_return_ RuntimeValue* evalInsert(Insert* insert, Environment* env);
	_Check_return_ RuntimeValue* evalIdentifier(Identifier* dclr, Environment* env);
	_Check_return_ RuntimeValue* evalAssign(AssignExpr* dclr, Environment* env);
	_Check_return_ RuntimeValue* evalBinExpr(BinExpr* binExpr, Environment* env);
public:
private:
	Parser parser;
	std::string m_currentDb;
};