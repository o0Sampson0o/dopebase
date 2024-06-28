#include <iostream>
#include <print>
#include <string>
#include <algorithm>
#include "Interpreter.h"
#include "Environment.h"

int main() {
	Environment* globalEnv = new Environment();

	std::string queryStr;
	Interpreter interpreter = Interpreter();
	std::println("Welcome to dopebase 1.1");
	
	while (true) {
		std::print("> ");
		std::getline(std::cin, queryStr);

		if (queryStr == "exit") {
			std::println("bye!");
			break;
		}

		queryStr += '\n';
		Query* query = interpreter.parse(queryStr);
		RuntimeValue* value = interpreter.eval(query, globalEnv);
	}

	return EXIT_SUCCESS;
}