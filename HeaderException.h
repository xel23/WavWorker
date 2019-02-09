#pragma once
#include "WavException.h"

class HeaderException : public WavException {
public:
	HeaderException(const std::string & message) : WavException(message) {}
};