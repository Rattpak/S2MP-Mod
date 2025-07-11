#pragma once

#include <string>

class compression
{
public:
#define CHUNK 16384u
	static std::string decompress(const std::string& data);
};

