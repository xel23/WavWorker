#pragma once
#include "WavException.h"

class BadParams : public WavException {
public:
	BadParams(const std::string & message) : WavException(message) {}
};