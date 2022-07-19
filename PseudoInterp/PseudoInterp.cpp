#include <iostream>
#include <string>
#include <sstream>
#include "lexer.h"
#include "objects.h"
#include "parser.h"
#include "AST.h"

void interpret(const std::string& inputStr)
{
	try
	{
		Scope globalScope;
		Parser parser;

		const CodeBlock* mainBlock = parser.getAST(inputStr);
		mainBlock->eval(&globalScope, false);
		delete mainBlock;
	}
	catch (std::runtime_error& re)
	{
		std::cout << re.what() << std::endl;
	}
}

int main()
{
	interpret("function factorial(x)\n\tif x == 1\n\t\treturn 1\n\treturn x*factorial(x-1)\noutput factorial(5)");
	return 0;
}
