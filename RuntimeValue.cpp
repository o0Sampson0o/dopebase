#include "RuntimeValue.h"
#include "DerivedRuntimeValue.h"

RuntimeValue::RuntimeValue(Type type)
	: type(type)
{

}

RuntimeValue::~RuntimeValue()
{
}

_Check_return_ RuntimeValue* RuntimeValue::operator+(const RuntimeValue& val)
{
	switch (this->type)
	{
	case STRING:
	{
		StringValue* p_thisStringValue = reinterpret_cast<StringValue*>(this);
		switch (val.type)
		{
		case STRING:
			return new StringValue(p_thisStringValue->value + reinterpret_cast<const StringValue*>(&val)->value);
		case NUMERIC:
			return new StringValue(p_thisStringValue->value + reinterpret_cast<const NumericValue*>(&val)->toString()->value);
		default:
			break;
		}
	} break;
	case NUMERIC:
	{
		NumericValue* p_thisNumericValue = reinterpret_cast<NumericValue*>(this);
		switch (val.type)
		{
		case STRING:
			return new StringValue(p_thisNumericValue->toString()->value + reinterpret_cast<const StringValue*>(&val)->value);
		case NUMERIC:
			return new NumericValue(p_thisNumericValue->value + reinterpret_cast<const NumericValue*>(&val)->value);
		default:
			break;
		}
	} break;
	default:
		break;
	}
	return new Undefined();
}

_Check_return_ NumericValue* RuntimeValue::operator-(const RuntimeValue& val)
{
	NumericValue* numericThis = this->toNumeric();
	NumericValue* numericVal = val.toNumeric();
	return new NumericValue(numericThis->value - numericVal->value);
}

_Check_return_ NumericValue* RuntimeValue::operator*(const RuntimeValue& val)
{
	NumericValue* numericThis = this->toNumeric();
	NumericValue* numericVal = val.toNumeric();
	return new NumericValue(numericThis->value * numericVal->value);
}

_Check_return_ NumericValue* RuntimeValue::operator/(const RuntimeValue& val)
{
	NumericValue* numericThis = this->toNumeric();
	NumericValue* numericVal = val.toNumeric();
	return new NumericValue(numericThis->value / numericVal->value);
}

_Check_return_ NumericValue* RuntimeValue::operator%(const RuntimeValue& val)
{
	NumericValue* numericThis = this->toNumeric();
	NumericValue* numericVal = val.toNumeric();
	return new NumericValue(fmod(numericThis->value, numericVal->value));
}

_Check_return_ StringValue* RuntimeValue::toString() const
{
	return new StringValue();
}

_Check_return_ NumericValue* RuntimeValue::toNumeric() const
{
	return new NumericValue(std::numeric_limits<double>::quiet_NaN());
}
