#include <iostream>
#include <string>
#include <sstream>
#include "lexer.h"
#include "objects.h"
#include "parser.h"
#include "AST.h"
int main()
{
	try {
		std::string str = "x = 5\nfor i from 0 to 10\n\toutput x";
		Scope globalScope;
		Parser parser;

		CodeBlock* mainBlock = parser.getAST(str);
		mainBlock->eval(&globalScope);
		delete mainBlock;
	}
	catch (std::runtime_error &re)
	{
		std::cout << re.what() << std::endl;
	}
	return 0;
}
