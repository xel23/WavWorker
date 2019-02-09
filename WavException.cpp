#include "WavException.h"

WavException::WavException(const std::string & message) noexcept : text_error(message) {}

const char * WavException::what() const noexcept
{
	return text_error.c_str();
}
