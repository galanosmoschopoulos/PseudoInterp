#include "errors.h"

CustomError::CustomError(std::string msg, const size_t pos) : message(std::move(msg)), position(pos)
{
	posSet = true;
}

CustomError::CustomError(std::string msg) : message(std::move(msg))
{
}

CustomError::CustomError() = default;
std::string CustomError::what() { return "Error: " + message; }
size_t CustomError::getPos() const { return position; }

void CustomError::setPos(const size_t pos)
{
	position = pos;
	posSet = true;
}

bool CustomError::isPosSet() const
{
	return posSet;
}


ValueError::ValueError() = default;
ValueError::ValueError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

ValueError::ValueError(const std::string& msg)
{
	message = msg;
}

std::string ValueError::what() { return "Value Error: " + message; }

TypeError::TypeError() = default;
TypeError::TypeError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

TypeError::TypeError(const std::string& msg)
{
	message = msg;
}

std::string TypeError::what() { return "Type Error: " + message; }

ArgumentError::ArgumentError() = default;
ArgumentError::ArgumentError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

ArgumentError::ArgumentError(const std::string& msg)
{
	message = msg;
}

std::string ArgumentError::what() { return "Argument Error: " + message; }

RangeError::RangeError() = default;
RangeError::RangeError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

RangeError::RangeError(const std::string& msg)
{
	message = msg;
}

std::string RangeError::what() { return "Range Error: " + message; }

FatalError::FatalError() = default;
FatalError::FatalError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

FatalError::FatalError(const std::string& msg)
{
	message = msg;
}

std::string FatalError::what() { return "Fatal Error: " + message; }

NameError::NameError() = default;
NameError::NameError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

NameError::NameError(const std::string& msg)
{
	message = msg;
}

std::string NameError::what() { return "Name Error: " + message; }

ParsingError::ParsingError() = default;
ParsingError::ParsingError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

ParsingError::ParsingError(const std::string& msg)
{
	message = msg;
}

std::string ParsingError::what() { return "Parsing Error: " + message; }

LexingError::LexingError() = default;
LexingError::LexingError(const std::string& msg, const size_t pos)
{
	message = msg;
	position = pos;
	posSet = true;
}

LexingError::LexingError(const std::string& msg)
{
	message = msg;
}

std::string LexingError::what() { return "Lexing Error: " + message; }
