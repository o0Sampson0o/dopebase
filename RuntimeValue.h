#pragma once
#include <string>
#include <map>
#include <algorithm>

enum RuntimeValueType
{
	STRING,
	NUMERIC,
	UNDEFINED,
	NULLISH
};

struct NumericValue;
struct StringValue;

class RuntimeValue
{
	using Type = RuntimeValueType;
public:
	RuntimeValue(Type type);
	~RuntimeValue();

	_Check_return_ RuntimeValue* operator+ (const RuntimeValue& val);
	_Check_return_ NumericValue* operator- (const RuntimeValue& val);
	_Check_return_ NumericValue* operator* (const RuntimeValue& val);
	_Check_return_ NumericValue* operator/ (const RuntimeValue& val);
	_Check_return_ NumericValue* operator% (const RuntimeValue& val);

	_Check_return_ virtual StringValue* toString() const;

	_Check_return_ virtual NumericValue* toNumeric() const;

	const Type type;
};