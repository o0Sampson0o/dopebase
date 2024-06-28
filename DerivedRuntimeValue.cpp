#pragma once
#include "DerivedRuntimeValue.h"

// Numeric value ;
NumericValue::NumericValue(double value)
	: RuntimeValue(NUMERIC), value(value)
{
}

_Check_return_ StringValue* NumericValue::toString() const
{
	return new StringValue(std::to_string(value));
}

// String value ; 
StringValue::StringValue(std::string value)
	: RuntimeValue(STRING), value(value)
{
}

_Check_return_ StringValue* StringValue::toString() const
{
	return new StringValue(this->value);
}

_Check_return_ NumericValue* StringValue::toNumeric() const
{
	if (std::all_of(value.begin(), value.end(), ::isdigit))
	{
		return new NumericValue(std::stoi(value));
	}
	return new NumericValue(std::numeric_limits<double>::quiet_NaN());
}