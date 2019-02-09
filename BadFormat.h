#pragma once
#include "WavException.h"

class BadFormat : public WavException {
public:
	BadFormat(const std::string & message) : WavException(message) {};
};