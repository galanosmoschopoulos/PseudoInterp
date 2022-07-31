#include "inputcleaner.h"

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
	if (nlines >= subStrVec.size())
	{
		nlines = subStrVec.size() - 1;
		posInLine = subStrVec.rbegin()->size() - 1;
	}
	else if (posInLine >= subStrVec[nlines].size()) posInLine = subStrVec[nlines].size() - 1;

	ss << "Line: " << nlines + sumOfDeletedStrs[nlines] + 1 << '\n';
	std::string s = subStrVec[nlines];
	std::ranges::replace(s.begin(), s.end(), '\t', ' ');
	ss << s;
	for (size_t i = 0; i < posInLine; ++i) ss << ' ';
	ss << "^";
	return ss.str();
}
