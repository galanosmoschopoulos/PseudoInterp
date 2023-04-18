/* errors.h */

#pragma once
#include <exception>
#include <string>

class CustomError : public std::exception
{
protected:
	std::string message; // Specific error message
	size_t position = 0; // The position in the input string
	bool posSet = false; /* The position should only be set once
							(when the error is first thrown)*/
public:
	CustomError();
	CustomError(std::string msg, size_t pos);
	explicit CustomError(std::string msg);
	virtual std::string what(); // This returns the rror text
	[[nodiscard]] size_t getPos() const;
	void setPos(size_t); // Set where the error was found in code
	[[nodiscard]] bool isPosSet() const;
};

// All different types of error are derived from CustomError
// The what function is overriden to provide error specific text
class ValueError final : public CustomError
{ // When the value of an object is inappropriate in context
public:
	ValueError();
	ValueError(const std::string& msg, size_t pos);
	explicit ValueError(const std::string& msg);
	std::string what() override;
};

class TypeError final : public CustomError
{ // When objects of incompatible types are received by operators
public:
	TypeError();
	TypeError(const std::string& msg, size_t pos);
	explicit TypeError(const std::string& msg);
	std::string what() override;
};

class ArgumentError final : public CustomError
{ // The arguments passed to a function are not correct (i.e., less than expected)
public:
	ArgumentError();
	ArgumentError(const std::string& msg, size_t pos);
	explicit ArgumentError(const std::string& msg);
	std::string what() override;
};

class RangeError final : public CustomError
{ // When a numerical object is out of range in its context
public:
	RangeError();
	RangeError(const std::string& msg, size_t pos);
	explicit RangeError(const std::string& msg);
	std::string what() override;
};

class FatalError final : public CustomError
{ // When something has gone so wrong, that it can't be identified
public:
	FatalError();
	FatalError(const std::string& msg, size_t pos);
	explicit FatalError(const std::string& msg);
	std::string what() override;
};

class NameError final : public CustomError
{ // For variable IDs that cannot be resolved in the scope
public:
	NameError();
	NameError(const std::string& msg, size_t pos);
	explicit NameError(const std::string& msg);
	std::string what() override;
};

class ParsingError final : public CustomError
{ // Something wrong with the input syntax
public:
	ParsingError();
	ParsingError(const std::string& msg, size_t pos);
	explicit ParsingError(const std::string& msg);
	std::string what() override;
};

class LexingError final : public CustomError
{ // Something wrong in the lexing of the code 
public:
	LexingError();
	LexingError(const std::string& msg, size_t pos);
	explicit LexingError(const std::string& msg);
	std::string what() override;
};
