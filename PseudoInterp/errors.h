#pragma once
#include <exception>
#include <string>

class CustomError : public std::exception
{
protected:
	std::string message;
	size_t position = 0;
	bool posSet = false;
public:
	CustomError();
	CustomError(std::string msg, size_t pos);
	explicit CustomError(std::string msg);
	virtual std::string what();
	[[nodiscard]] size_t getPos() const;
	void setPos(size_t);
	[[nodiscard]] bool isPosSet() const;
};

class ValueError final : public CustomError
{
public:
	ValueError();
	ValueError(const std::string& msg, size_t pos);
	explicit ValueError(const std::string& msg);
	std::string what() override;
};

class TypeError final : public CustomError
{
public:
	TypeError();
	TypeError(const std::string& msg, size_t pos);
	explicit TypeError(const std::string& msg);
	std::string what() override;
};

class ArgumentError final : public CustomError
{
public:
	ArgumentError();
	ArgumentError(const std::string& msg, size_t pos);
	explicit ArgumentError(const std::string& msg);
	std::string what() override;
};

class RangeError final : public CustomError
{
public:
	RangeError();
	RangeError(const std::string& msg, size_t pos);
	explicit RangeError(const std::string& msg);
	std::string what() override;
};

class FatalError final : public CustomError
{
public:
	FatalError();
	FatalError(const std::string& msg, size_t pos);
	explicit FatalError(const std::string& msg);
	std::string what() override;
};

class NameError final : public CustomError
{
public:
	NameError();
	NameError(const std::string& msg, size_t pos);
	explicit NameError(const std::string& msg);
	std::string what() override;
};

class ParsingError final : public CustomError
{
public:
	ParsingError();
	ParsingError(const std::string& msg, size_t pos);
	explicit ParsingError(const std::string& msg);
	std::string what() override;
};

class LexingError final : public CustomError
{
public:
	LexingError();
	LexingError(const std::string& msg, size_t pos);
	explicit LexingError(const std::string& msg);
	std::string what() override;
};
