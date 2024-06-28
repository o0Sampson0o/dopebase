#pragma once
#include "RuntimeValue.h"

struct NumericValue : public RuntimeValue
{
	NumericValue(double value = 0.0);
public:
	_Check_return_ StringValue* toString() const override;
	double value;
};

struct StringValue : public RuntimeValue
{
	StringValue(std::string value = "");
	std::string value;
public:
	_Check_return_ StringValue* toString() const override;
	_Check_return_ NumericValue* toNumeric() const override;
};

struct Undefined : public RuntimeValue
{
	Undefined() : RuntimeValue(UNDEFINED) {}
	const std::string value = "undefined";
};

struct Null : public RuntimeValue
{
	Null() : RuntimeValue(NULLISH) {}
	const std::string value = "null";
};