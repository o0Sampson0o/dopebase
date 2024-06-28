#pragma once

enum class NodeType {
	QUERY, // Statement ;
	CREATE_DB,
	CREATE_TABLE,
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
	DbCreator() : Statement(Type::CREATE_DB) {}
	~DbCreator() = default;

	std::string dbName;
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