#include "errors.h"

CustomError::CustomError(std::string msg, const size_t pos) : message(std::move(msg)), position(pos) {}
CustomError::CustomError(std::string msg) : message(std::move(msg)) {}
CustomError::CustomError() = default;
std::string CustomError::what() { return "Error: " + message; }
size_t CustomError::getPos() const { return position; }
void CustomError::setPos(const size_t pos)
{
	position = pos;
}


ValueError::ValueError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
ValueError::ValueError(const std::string& msg)
{
	message = msg;
}
std::string ValueError::what() { return "Value Error: " + message; }

TypeError::TypeError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
TypeError::TypeError(const std::string& msg)
{
	message = msg;
}
std::string TypeError::what() { return "Type Error: " + message; }

ArgumentError::ArgumentError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
ArgumentError::ArgumentError(const std::string& msg)
{
	message = msg;
}
std::string ArgumentError::what() { return "Argument Error: " + message; }

RangeError::RangeError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
RangeError::RangeError(const std::string& msg)
{
	message = msg;
}
std::string RangeError::what() { return "Range Error: " + message; }

FatalError::FatalError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
FatalError::FatalError(const std::string& msg)
{
	message = msg;
}
std::string FatalError::what() { return "Fatal Error: " + message; }

NameError::NameError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
NameError::NameError(const std::string& msg)
{
	message = msg;
}
std::string NameError::what() { return "Name Error: " + message; }

ParsingError::ParsingError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
ParsingError::ParsingError(const std::string& msg)
{
	message = msg;
}
std::string ParsingError::what() { return "Parsing Error: " + message; }

LexingError::LexingError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
}
LexingError::LexingError(const std::string& msg)
{
	message = msg;
}
std::string LexingError::what() { return "Lexing Error: " + message; }


