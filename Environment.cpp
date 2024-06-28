#include "Environment.h"
#include "RuntimeValue.h"
#include <format>


Environment::Environment(Environment* parent)
	: pm_parent(parent)
{
}

Environment::~Environment()
{
}

void Environment::declare(std::string& identifier, RuntimeValue* value, bool isConst)
{

	if (m_variableMap.contains(identifier))
	{
		throw std::format("Uncaught SyntaxError: Identifier '{}' has already been declared", identifier);
	}

	if (isConst)
	{
		m_constantSet.insert(identifier);
	}
	m_variableMap.emplace(identifier, value);
}

void Environment::assign(std::string& identifier, RuntimeValue* value) {
	auto it = m_variableMap.find(identifier);

	if (m_constantSet.contains(identifier))
	{
		throw "Uncaught TypeError: Assignment to constant variable.";
	}

	if (it == m_variableMap.end())
	{
		throw std::format("Uncaught ReferenceError : {} is not defined", identifier);
	}

	it->second = value;
}

_Check_return_ RuntimeValue* Environment::lookUp(std::string& identifier)
{
	return m_variableMap.find(identifier)->second;
}

_Check_return_ Environment* Environment::resolve(std::string& identifier)
{
	if (m_variableMap.contains(identifier))
	{
		return this;
	}

	if (pm_parent == nullptr)
	{
		throw std::format("Uncaught ReferenceError: {} is not defined", identifier);
	}

	return pm_parent->resolve(identifier);
}