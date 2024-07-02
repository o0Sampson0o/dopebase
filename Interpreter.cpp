#include "Interpreter.h"
#include "Environment.h"
#include "DerivedRuntimeValue.h"
#include "settings.h"
#include <iostream>
#include <map>
#include <print>
#include <fstream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

static unsigned int determineSize(const TableColType const* tableColType) {
	if (tableColType->typeName == "int") return sizeof(int);
	if (tableColType->typeName == "bool") return sizeof(bool);
	if (tableColType->typeName == "float") return sizeof(float);
	if (tableColType->typeName == "double") return sizeof(double);
	if (tableColType->typeName == "char"    ||
		tableColType->typeName == "varchar" ||
		tableColType->typeName == "bianry"    ) return stoi(tableColType->attr);
}

_Check_return_ Query* Interpreter::parse(std::string& sourceCode) {
	return parser.generateAST(sourceCode);
}

_Check_return_ RuntimeValue* Interpreter::eval(Node* astNode, Environment* env) {
	switch (astNode->type) {
	case NodeType::QUERY:
		return evalQuery(reinterpret_cast<Query*>(astNode), env);
		break;
	case NodeType::NUMERIC_LITERAL:
		return new NumericValue(reinterpret_cast<NumericLiteral*>(astNode)->val);
	case NodeType::DB_CREATOR:
		return evalDbCreator(reinterpret_cast<DbCreator*>(astNode), env);
		break;
	case NodeType::TABLE_CREATOR:
		return evalTableCreator(reinterpret_cast<TableCreator*>(astNode), env);
		break;
	case NodeType::USE_DB:
		return evalUseDb(reinterpret_cast<Use*>(astNode), env);
		break;
	case NodeType::INSERT:
		return evalInsert(reinterpret_cast<Insert*>(astNode), env);
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
		throw "Not Implemented Syntax";
		break;
	}

	return nullptr;
}

_Check_return_ RuntimeValue* Interpreter::evalQuery(Query* program, Environment* env)
{
	RuntimeValue* lastestValue = new Undefined();
	
	for (Statement* statement : program->body) {
		RuntimeValue* temp = eval(statement, env);
		lastestValue = temp->type == RuntimeValueType::UNDEFINED ? lastestValue : temp;
	}
	return lastestValue;
}

_Check_return_ RuntimeValue* Interpreter::evalDbCreator(DbCreator* dbCreator, Environment* env)
{	
	std::string dbPath = std::format(R"({}\{}\)", dopebase::root, dbCreator->dbName);

	if (fs::exists(dbPath))
		throw std::format(R"(database '{}' already existed)", dbCreator->dbName);

	fs::create_directory(dbPath);
	std::println(R"(database '{}' created)", dbCreator->dbName);
	return new Undefined();
}

_Check_return_ RuntimeValue* Interpreter::evalTableCreator(TableCreator* tableCreator, Environment* env)
{	
	std::string currentDb = tableCreator->dbName.empty() ? m_currentDb : tableCreator->dbName;
	std::string tablePath = std::format(R"({}\{}\{}.dpdb)", dopebase::root, currentDb, tableCreator->tableName);

	if (currentDb.empty())
		throw "No databse selected";

	if (fs::exists(tablePath))
		throw std::format(R"(table '{}.{}' already existed)", currentDb, tableCreator->tableName);

	std::ofstream tableFile(tablePath);
	std::stringstream ss;

	for (const TableColDclr* const tableColDclr : tableCreator->cols) {
		unsigned int colSize = determineSize(tableColDclr->colType);
		std::print(ss, "{} {} ", tableColDclr->colName, tableColDclr->colType->typeName);
	
		for (int i = 0; i < 4; i++) {
			std::print(ss, "{}", ((char*)(&colSize))[i]);
		}
		std::print(ss, " ");
	}

	unsigned int length = ss.str().length() + 5;
	
	length = (length & ~0b111111111) + (0b1000000000 * !!(length & 0b111111111));

	for (int i = 0; i < 4; i++) {
		std::print(tableFile, "{}", ((char*)(&length))[i]);
	}
	std::print(tableFile, " ");
	std::print(tableFile, "{}", ss.str());
	for (unsigned int i = ss.str().length() + 5; i < length; i++) {
		std::print(tableFile, "{}", '\0');
	}
	tableFile.close();
	std::println(R"(database '{}.{}' created)", currentDb, tableCreator->tableName);

	return new Undefined();
}

_Check_return_ RuntimeValue* Interpreter::evalUseDb(Use* use, Environment* env)
{
	std::string dbPath = std::format(R"({}\{}\)", dopebase::root, use->dbName);
	m_currentDb = use->dbName;
	
	if (!fs::exists(dbPath))
		throw std::format(R"(Database '{}' does not exist)", use->dbName);

	std::println(R"(Switched to database '{}')", use->dbName);
	return new Undefined();
}


_Check_return_ RuntimeValue* Interpreter::evalInsert(Insert* insert, Environment* env)
{
	std::string currentDb = insert->dbName != "" ? insert->dbName : m_currentDb;
	std::string dbPath = std::format(R"({}\{}\)", dopebase::root, currentDb);
	std::string tablePath = std::format(R"({}{}.dpdb)", dbPath, insert->tableName);

	std::ifstream tableFile(tablePath);
	std::ofstream tableFile(tablePath);

	if (currentDb.empty())
		throw "No databse selected";
	
	if (!fs::exists(dbPath))
		throw std::format(R"(database '{}' does not exist)", insert->dbName);
	
	if (!fs::exists(tablePath))
		throw std::format(R"(table '{}.{}' does not exist)", currentDb, insert->tableName);

	std::println("tried to insert smtg");

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
	else throw std::format("WTF binary op never seen before {}", binExpr->op);
}
