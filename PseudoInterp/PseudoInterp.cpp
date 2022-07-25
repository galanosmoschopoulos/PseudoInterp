#include <iostream>
#include <string>

#include "AST.h"
#include "parser.h"
#include "scope.h"

void interpret(const std::string& inputStr)
{
	try
	{

		Scope globalScope;
		*IDNode("Array", 0).eval(&globalScope, true) = Object(ArrayConstructor());
		
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
	interpret("N = 10\nfunction fib(n)\n\ta = 0\n\tb = 1\n\tfor i from 0 to n-1\n\t\ttmp = b\n\t\tb = a+b\n\t\ta=tmp\n\treturn a\noutput fib(N)");
	interpret("N = 10\nfunction fib(n)\n\tif n == 0\n\t\treturn 0\n\telif n == 1\n\t\treturn 1\n\treturn fib(n-1)+fib(n-2)\noutput fib(N)");
	interpret("N = 10\nfunction fib(n)\n\tarr = Array(n+1)\n\tarr[0] = 0\n\tarr[1]=1\n\tfor i from 2 to n\n\t\tarr[i] = arr[i-1]+arr[i-2]\n\treturn arr[n]\noutput fib(N)");
	return 0;
}
