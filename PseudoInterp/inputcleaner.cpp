#include "inputcleaner.h"
#include <algorithm>
#include <sstream>

InputCleaner::InputCleaner() = default;

InputCleaner::InputCleaner(std::string inputStr) : originalStr(std::move(inputStr))
{
}

void InputCleaner::setInputStr(const std::string& inputStr)
{
	originalStr = inputStr;
}

std::string InputCleaner::clean()
{
	subStrVec.erase(subStrVec.begin(), subStrVec.end());
	std::stringstream ss(originalStr);
	std::string subStr, finalStr;
	const std::string whitespaces(" \t\f\v\n\r");
	int deletedLines = 0;
	while (std::getline(ss, subStr, '\n'))
	{
		if (subStr.compare(0, 2, "//") == 0) // Removes all-comment lines
		{
			deletedLines++;
			break;
		}
		if (std::ranges::any_of(subStr, isgraph))
		{
			// If the string contains graphic characters (i.e. not just spaces)
			subStr.erase(subStr.find_last_not_of(whitespaces) + 1);
			finalStr += subStr + "\n"; // Add to new string
			subStrVec.push_back(subStr + '\n');
			sumOfDeletedStrs.push_back(deletedLines);
		}
		else deletedLines++;
	}
	return finalStr;
}

std::string InputCleaner::getErrorLine(const size_t errPos) const
{
	std::stringstream ss;
	size_t currLen = 0, nlines = 0, posInLine = 0;
	for (const auto& subStr : subStrVec)
	{
		// Given a pos in a multi-line string, it finds the num of line and the pos in that line
		if (currLen + subStr.size() <= errPos)
		{
			currLen += subStr.size();
			nlines++;
		}
		else
		{
			posInLine = errPos - currLen;
			break;
		}
	}
	if (nlines >= subStrVec.size()) // If it's at the end of file
	{
		nlines = subStrVec.size() - 1;
		posInLine = subStrVec.rbegin()->size() - 1;
	}
	else if (posInLine >= subStrVec[nlines].size()) posInLine = subStrVec[nlines].size() - 1;
	// And account for this weird error

	ss << "Line: " << nlines + sumOfDeletedStrs[nlines] + 1 << '\n';
	std::string s = subStrVec[nlines];
	std::ranges::replace(s.begin(), s.end(), '\t', ' '); // Replace tabs with space to save space in the error printout
	ss << s;
	for (size_t i = 0; i < posInLine; ++i) ss << ' '; // Print the '^' symbol to mark exact error location
	ss << "^";
	return ss.str();
}
