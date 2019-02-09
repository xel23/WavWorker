#pragma once
#include "WavException.h"

class ReadingHeader : public WavException {
public:
	ReadingHeader(const std::string & message) : WavException(message) {}
};