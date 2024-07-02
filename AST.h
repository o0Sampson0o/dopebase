#pragma once

enum class NodeType {
	QUERY, // Statement ;
	DB_CREATOR,
	TABLE_CREATOR,
	TABLE_COL_DCLR,
	TABLE_COL_TYPE,
	USE_DB,
	INSERT,
	ASSIGNMENT, // Expression ;
	NUMERIC_LITERAL,
	STRING_LITERAL,
	OBJECT_LITERAL,
	OBJECT_PROP,
	NULL_LITERAL,
	UNDEFINED_LITERAL,
	IDENTIFIER,
	BINARY_EXPR
};

struct Node {
	using Type = NodeType;

	Node(Type type) : type(type) {}
	virtual ~Node() = default;

	const Type type;
};

struct Statement : public Node {
	Statement(Type type) : Node(type) {}
	virtual ~Statement() = default;
};

struct Expr : public Statement {
	Expr(Type type) : Statement(type) {}
	virtual ~Expr() = default;
};

struct Query : Statement {
	Query() : Statement(Type::QUERY) {}
	~Query() = default;

	std::vector<Statement*> body;
};

struct DbCreator : Statement {
	DbCreator() : Statement(Type::DB_CREATOR) {}
	~DbCreator() = default;

	std::string dbName;
};

struct Use : Statement {
	Use() : Statement(Type::USE_DB) {}
	~Use() = default;

	std::string dbName;
};

struct TableColType : Statement {
	TableColType() : Statement(Type::TABLE_COL_TYPE) {}
	~TableColType() = default;

	std::string typeName;
	std::string attr;
};

struct TableColDclr : Statement {
	TableColDclr() : Statement(Type::TABLE_COL_DCLR), colType{ 0 } {}
	~TableColDclr() = default;

	std::string colName;
	TableColType* colType;
};

struct TableCreator : Statement {
	TableCreator() : Statement(Type::TABLE_CREATOR) {}
	~TableCreator() = default;

	std::string dbName;
	std::string tableName;
	std::vector<TableColDclr*> cols;
};

struct Insert : Statement {
	Insert() : Statement(Type::INSERT) {}
	~Insert() = default;

	std::string dbName;
	std::string tableName;
	std::vector<std::string> cols;
	std::vector<Expr*> vals;
};

// Expression ;
struct AssignExpr : public Expr
{
	AssignExpr() : Expr(Type::ASSIGNMENT) {}
	Expr* assignee = nullptr;
	Expr* value = nullptr;
};

struct BinExpr : public Expr
{
	BinExpr()
		: Expr(Type::BINARY_EXPR)
	{}

	~BinExpr() = default;

	Expr* left = nullptr;
	Expr* right = nullptr;
	std::string op;
};

struct Identifier : public Expr
{
	Identifier(std::string symbol = "") : Expr(Type::IDENTIFIER), symbol(symbol) {}
	std::string symbol;
};

// Literal Expression ;
struct NumericLiteral : public Expr
{
	NumericLiteral() : Expr(Type::NUMERIC_LITERAL) {}
	double val = 0.0;
};

struct StringLiteral : public Expr
{
	StringLiteral(const std::string& val = "") : Expr(Type::STRING_LITERAL), val(val) {}
	std::string val = "";
};

struct NullLiteral : public Expr
{
	NullLiteral() : Expr(Type::NULL_LITERAL) {}
};

struct UndefinedLiteral : public Expr
{
	UndefinedLiteral() : Expr(Type::UNDEFINED_LITERAL) {}
};