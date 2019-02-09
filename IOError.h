#pragma once
#include "WavException.h"

class IOError : public WavException {
public:
	IOError(const std::string & message) : WavException(message) {}
};