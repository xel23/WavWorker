#pragma once
#include "WavException.h"

class UnsupportedFormat : public WavException {
public:
	UnsupportedFormat(const std::string & message) : WavException(message) {}
};