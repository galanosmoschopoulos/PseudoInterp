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
		std::string str = "x = 45\noutput x-- == 23\noutput x";
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
