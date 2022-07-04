#include <iostream>
#include <string>
#include "lexer.h"
#include "objects.h"
#include "parser.h"
#include "AST.h"


int main()
{
	try {
		Scope globalScope;
		Parser parser;

		parser.getAST("num = 1")->eval(&globalScope);
	}
	catch (std::runtime_error &re)
	{
		std::cout << re.what() << std::endl;
	}
	return 0;
}
