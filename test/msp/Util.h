#ifndef MSPUTIL_H_
#define MSPUTIL_H_

#include <iostream>
#include <stdexcept>
#include <stdlib.h>

namespace msp
{

class Util
{

public:
	/**
	 * Parse a string to int.
	 *
	 * If str can be parsed to int, put return true with result stored in "result".
	 * Otherwise, print error message to std::cerr and return false.
	 *
	 */
	static bool
	parse_int(char const* str, int& result);

};

}

#endif // MSPUTIL_H_
