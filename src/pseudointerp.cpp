/* PseudoInterp.cpp */

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono> // To measure runtime of code

#include "AST.h"
#include "parser.h"
#include "scope.h"
#include "inputcleaner.h"
#include "errors.h"
/* This color lib only works with windows
 * #include "color.h" */

#define VER "1.0" // Current version of the software


void interpret(const std::string& inputStr)
{
	InputCleaner cleaner(inputStr);
    CodeBlock* mainBlock = nullptr;
	try
	{
		Parser parser;
		mainBlock = parser.getAST(cleaner.clean());
		// Get the AST of the whole code
		Scope globalScope;
		globalScope.enableExternalFunctions();
		// To have functions such as output(), input(), etc.
		const auto start = std::chrono::high_resolution_clock::now();
		mainBlock->eval(&globalScope, false);
		const auto stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<
			std::chrono::milliseconds>(stop - start);

		/* Only in Windows
		 * std::cout << '\n' << dye::green_on_black(
			"Successful execution.\nTime elapsed: " + std::to_string(
				duration.count()) + " ms.") << '\n';*/

		std::cout << '\n' <<  "Successful execution.\nTime elapsed: " +
			std::to_string(duration.count()) + " ms." << '\n';

	}
	catch (CustomError& ce)
	{
		// Print both the custom error message, and the line in the source code where
		// the error occurs.
		/* Colors only in Windows
		 * std::cerr << '\n' << dye::red_on_black(
			ce.what() + "\n" + cleaner.getErrorLine(ce.getPos()));*/

		std::cerr << '\n' << ce.what() + "\n" + cleaner.getErrorLine(ce.getPos());

		// Cleaner is responsible for accepting a position as a integer, and finding the
		// exact line and character corresponding to that position.
	}
    
    if (mainBlock)
        delete mainBlock;
}

int main(int argc, char** argv)
{
	struct commandLineFlags // Bit field to store flags
	{
		unsigned int help : 1 = 0; // Shows help message
		unsigned int ver : 1 = 0; // Displays version
		unsigned int inputFile : 1 = 0; // Accept the input file
		unsigned int inputFileSet : 1 = 0; // 1 if file already set
	} flags;
	std::string inputFilePath;
	try
	{
		while (--argc > 0 && (*++argv)[0] == '-') // While there are more args
		{
			// Scan all characters after the '-'
			for (char c = *++argv[0]; c; c = *++argv[0])
			{
				switch (std::tolower(c))
				{
				case '\?': // Depending on character, set the flag
					flags.help = 1;
					break;
				case 'v':
					flags.ver = 1;
					break;
				case 'i':
					flags.inputFile = 1;
					break;
				default:
					throw std::runtime_error(
						"Illegal command line argument: " + std::string(1, c));
				}
			}
			if (flags.inputFile)
			{
				flags.inputFile = 0;
				if (flags.inputFileSet) // If file already set
					throw std::runtime_error(
						"Input file already set.");
				flags.inputFileSet = 1;
				if (argc == 1) // If no more args, user forgot to enter path
					throw std::runtime_error(
						"Input file path expected.");
				inputFilePath = *++argv; // get path
				--argc;
			}
		}
		if (argc != 0) // If not all arguments detected throw error
			throw std::runtime_error(
				"Illegal command line arguments.");
		if (flags.help)
			std::cout << // Print help message
				"IB pseudocode interpreter made by Rafael Moschopoulos\n "
				"Usage\t-? : Prints this message\n\t-I : Sets "
				"input code file\n\t-V : Prints version number\n";
		if (flags.ver) std::cout << "Version " << VER << '\n'; // Show version

		if (flags.inputFileSet)
		{
			std::ifstream inputFile(inputFilePath); // Input file stream
			if (!inputFile.is_open()) // Handle file error
				throw std::runtime_error( // Report faulty path
					"Error opening file \"" + inputFilePath + "\"");
			std::stringstream fileBuffer;
			fileBuffer << inputFile.rdbuf(); // Read file into buffer
			interpret(fileBuffer.str()); // Interpret code
			inputFile.close();
		}
	}
	catch (std::runtime_error& re) // Report any errors occured
	{
		std::cerr << re.what() << '\n';
	}
	return 0;
}
