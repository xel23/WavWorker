#pragma once
#include <exception>
#include <string>

class WavException : public std::exception {
private:
	std::string text_error;
public:
	WavException(const std::string &message) noexcept;
	~WavException() {};
	const char * what() const noexcept;
};

class HeaderException : public WavException {
public:
	HeaderException(const std::string & message) : WavException(message) {}
};

class BadFormat : public WavException {
public:
	BadFormat(const std::string & message) : WavException(message) {};
};

class ReadingHeader : public WavException {
public:
	ReadingHeader(const std::string & message) : WavException(message) {}
};

class UnsupportedFormat : public WavException {
public:
	UnsupportedFormat(const std::string & message) : WavException(message) {}
};

class BadParams : public WavException {
public:
	BadParams(const std::string & message) : WavException(message) {}
};

class IOError : public WavException {
public:
	IOError(const std::string & message) : WavException(message) {}
};