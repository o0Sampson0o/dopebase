#include "Interpreter.h"
#include "Environment.h"
#include "DerivedRuntimeValue.h"
#include "settings.h"
#include <iostream>
#include <map>
#include <print>
#include <fstream>
#include <filesystem>

_Check_return_ Query* Interpreter::parse(std::string& sourceCode)
{
	return parser.generateAST(sourceCode);
}

_Check_return_ RuntimeValue* Interpreter::eval(Node* astNode, Environment* env)
{
	switch (astNode->type)
	{
	case NodeType::QUERY:
		return evalQuery(reinterpret_cast<Query*>(astNode), env);
		break;
	case NodeType::NUMERIC_LITERAL:
		return new NumericValue(reinterpret_cast<NumericLiteral*>(astNode)->val);
	case NodeType::CREATE_DB:
		return evalDbCreator(reinterpret_cast<DbCreator*>(astNode), env);
		break;
	case NodeType::STRING_LITERAL:
		return new StringValue(reinterpret_cast<StringLiteral*>(astNode)->val);
	case NodeType::NULL_LITERAL:
		return new Null();
		break;
	case NodeType::UNDEFINED_LITERAL:
		return new Undefined();
		break;
	case NodeType::IDENTIFIER:
		return evalIdentifier(reinterpret_cast<Identifier*>(astNode), env);
		break;
	case NodeType::BINARY_EXPR:
		return evalBinExpr(reinterpret_cast<BinExpr*>(astNode), env);
		break;
	default:
		std::println("Not Implemented Syntax");
		break;
	}

	return nullptr;
}

_Check_return_ RuntimeValue* Interpreter::evalQuery(Query* program, Environment* env)
{
	RuntimeValue* lastestValue = new Undefined();
	for (Statement* statement : program->body)
	{
		RuntimeValue* temp = eval(statement, env);
		lastestValue = temp->type == RuntimeValueType::UNDEFINED ? lastestValue : temp;
	}
	return lastestValue;
}

_Check_return_ RuntimeValue* Interpreter::evalDbCreator(DbCreator* dbCreator, Environment* env)
{
	namespace fs = std::filesystem;
	
	std::string databasePath = std::format("{}\\{}.dpdb", dopebase::root, dbCreator->dbName);

	if (fs::exists(databasePath)) {
		std::println("database \'{}\' already existed", dbCreator->dbName);
	}
	else {
		std::ofstream database(databasePath);
		database.close();
		std::println("database \'{}\' created", dbCreator->dbName);
	}

	return new Undefined();
}

_Check_return_ RuntimeValue* Interpreter::evalIdentifier(Identifier* identifier, Environment* env)
{
	Environment* resolved;
	if ((resolved = env->resolve(identifier->symbol)) != nullptr)
	{
		return resolved->lookUp(identifier->symbol);
	}
	return new Undefined();
}

_Check_return_ RuntimeValue* Interpreter::evalAssign(AssignExpr* expr, Environment* env)
{
	RuntimeValue* result = eval(expr->value, env);

	env->assign(reinterpret_cast<Identifier*>(expr->assignee)->symbol, result);
	return result;
}

_Check_return_ RuntimeValue* Interpreter::evalBinExpr(BinExpr* binExpr, Environment* env)
{
	RuntimeValue* lhs = eval(binExpr->left, env);
	RuntimeValue* rhs = eval(binExpr->right, env);

	if (binExpr->op == "+") return *lhs + *rhs;
	else if (binExpr->op == "-") return *lhs - *rhs;
	else if (binExpr->op == "*") return *lhs * *rhs;
	else if (binExpr->op == "/") return *lhs / *rhs;
	else if (binExpr->op == "%") return *lhs % *rhs;
	else
	{
		println("WTF binary op never seen before {}", binExpr->op);
		return new Undefined();
	}
}
