#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "AST.h"
#include "parser.h"
#include "scope.h"

#define VER "1.0"

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
		std::cerr << re.what() << std::endl;
	}
}

int main(int argc, char** argv)
{
	struct commandLineFlags
	{
		int help : 1 = 0;
		int ver : 1 = 0;
		int inputFile : 1 = 0;
		int inputFileSet : 1 = 0;
	} flags;
	std::string inputFilePath;
	try {
		while(--argc > 0 && (*++argv)[0] == '-')
		{
			unsigned char c;
			while (c = *++argv[0])
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
			if(flags.inputFile)
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
		if(flags.help) std::cout << "IB pseudocode interpreter made by Galanos Moschopoulos for the Computer Science IA.\nUsage\t-? : Prints this message\n\t-I : Sets input code file\n\t-V : Prints version number\n";
		if(flags.ver) std::cout << "Version " << VER << '\n';

		if (flags.inputFileSet) {
			std::ifstream inputFile(inputFilePath);
			if (!inputFile.is_open()) throw std::runtime_error("Error opening file \"" + inputFilePath + "\"");
			std::stringstream fileBuffer;
			fileBuffer << inputFile.rdbuf();
			interpret(fileBuffer.str());
			inputFile.close();
		}
	}
	catch(std::runtime_error &re)
	{
		std::cerr << re.what() << '\n';
	}
	return 0;
}
