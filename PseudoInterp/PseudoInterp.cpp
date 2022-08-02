#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <chrono>
#include "AST.h"
#include "parser.h"
#include "scope.h"
#include "inputcleaner.h"
#include "errors.h"
#include "color.h"
#include <stack>
#define VER "1.0"


void interpret(const std::string& inputStr)
{
	InputCleaner cleaner(inputStr);
	try
	{
		Parser parser;
		auto mainBlock = parser.getAST(cleaner.clean());
		Scope globalScope;
		globalScope.enableExternalFunctions();
		const auto start = std::chrono::high_resolution_clock::now();
		mainBlock->eval(&globalScope, false);
		const auto stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		delete mainBlock;
		std::cout << '\n' << dye::green_on_black(
			"Successful execution.\nTime elapsed: " + std::to_string(duration.count()) + " ms.") << '\n';
	}
	catch (CustomError& ce)
	{
		std::cerr << '\n' << dye::red_on_black(ce.what() + "\n" + cleaner.getErrorLine(ce.getPos()));
	}
}

int main(int argc, char** argv)
{
	struct commandLineFlags
	{
		unsigned int help : 1 = 0;
		unsigned int ver : 1 = 0;
		unsigned int inputFile : 1 = 0;
		unsigned int inputFileSet : 1 = 0;
	} flags;
	std::string inputFilePath;
	try
	{
		while (--argc > 0 && (*++argv)[0] == '-')
		{
			for (char c = *++argv[0]; c; c = *++argv[0])
			{
				switch (std::tolower(c))
				{
				case '\?':
					flags.help = 1;
					break;
				case 'v':
					flags.ver = 1;
					break;
				case 'i':
					flags.inputFile = 1;
					break;
				default:
					throw std::runtime_error("Illegal command line argument: " + std::string(1, c));
				}
			}
			if (flags.inputFile)
			{
				flags.inputFile = 0;
				if (flags.inputFileSet) throw std::runtime_error("Input file already set.");
				flags.inputFileSet = 1;
				if (argc == 1) throw std::runtime_error("Input file path expected.");
				inputFilePath = *++argv;
				--argc;
			}
		}
		if (argc != 0) throw std::runtime_error("Illegal command line arguments.");
		if (flags.help) std::cout <<
			"IB pseudocode interpreter made by Galanos Moschopoulos for the Computer Science IA.\nUsage\t-? : Prints this message\n\t-I : Sets input code file\n\t-V : Prints version number\n";
		if (flags.ver) std::cout << "Version " << VER << '\n';

		if (flags.inputFileSet)
		{
			std::ifstream inputFile(inputFilePath);
			if (!inputFile.is_open()) throw std::runtime_error("Error opening file \"" + inputFilePath + "\"");
			std::stringstream fileBuffer;
			fileBuffer << inputFile.rdbuf();
			interpret(fileBuffer.str());
			inputFile.close();
		}
	}
	catch (std::runtime_error& re)
	{
		std::cerr << re.what() << '\n';
	}
	
	return 0;
}
