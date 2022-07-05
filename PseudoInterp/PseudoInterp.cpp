#include <iostream>
#include <string>
#include "lexer.h"
#include "objects.h"
#include "parser.h"
#include "AST.h"

int main()
{
	try {
		std::string str = "a  =   5		  \n	 	 \noutput  a	   \n\n";

		Scope globalScope;
		Parser parser;

		parser.getAST(str)->eval(&globalScope);
	}
	catch (std::runtime_error &re)
	{
		std::cout << re.what() << std::endl;
	}
	return 0;
}
