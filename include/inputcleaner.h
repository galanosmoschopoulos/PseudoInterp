#pragma once
#include <string>
#include <vector>

class InputCleaner
{
public:
	InputCleaner();
	explicit InputCleaner(std::string);
	void setInputStr(const std::string&);
	std::string clean(); // Returns cleaned str
	// Get the specific line where the error occured
	[[nodiscard]] std::string getErrorLine(size_t pos) const;

private:
	std::string str;
	std::string originalStr; // User's input
	std::vector<std::string> subStrVec; // each line is kept separately
	std::vector<int> sumOfDeletedStrs; /* # of deleted lines to restore
										  original position at error */
};
