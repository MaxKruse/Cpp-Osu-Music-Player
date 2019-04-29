#pragma once

#include <random>

namespace Parser {


	// Random Functions using Low/High or a vector of sorts, to return a random index between given low and high
	size_t Random(size_t low = 0, size_t high = 0xFFFFFFF)
	{
		size_t z = rand() % (high - low);
		z += low;
		return z;
	}

	template<typename T>
	size_t Random(std::vector<T> input)
	{
		if (input.size() == 1)
		{
			return 0;
		}
		size_t low = 0;
		size_t high = input.size();
		size_t z = rand() % (high - low);
		z += low - 1;
		return z;
	}

	// See: https://stackoverflow.com/a/3418285
	void replaceAll(std::string & str, const std::string & from, const std::string & to) {
		if (from.empty())
		{
			return;
		}
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

}