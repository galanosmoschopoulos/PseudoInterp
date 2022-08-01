#pragma once
#include <string>
#include <vector>

class InputCleaner
{
public:
	InputCleaner();
	explicit InputCleaner(std::string);
	void setInputStr(const std::string&);
	std::string clean();
	[[nodiscard]] std::string getErrorLine(size_t pos) const;

private:
	std::string str;
	std::string originalStr;
	std::vector<std::string> subStrVec;
	std::vector<int> sumOfDeletedStrs;
};
