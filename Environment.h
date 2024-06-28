#pragma once
#include <map>
#include <set>
#include <string>

class RuntimeValue;

class Environment
{
public:
	Environment(Environment* parent = nullptr);
	~Environment();
	void declare(std::string& identifier, RuntimeValue* value, bool isConst);
	void assign(std::string& identifier, RuntimeValue* value);
	_Check_return_ RuntimeValue* lookUp(std::string& identifier);
	_Check_return_ Environment* resolve(std::string& identifier);
private:
	std::map<std::string, RuntimeValue*> m_variableMap;
	std::set<std::string> m_constantSet;
	Environment* pm_parent;
};