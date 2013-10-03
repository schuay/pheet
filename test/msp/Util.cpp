/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Util.h"

namespace msp
{

bool
Util::
parse_int(char const* str, int& result)
{
	try {
		result = std::stoi(str);
		return true;
	} catch (const std::invalid_argument&) {
		std::cerr << "Invalid argument: " << str << std::endl;
	} catch (const std::out_of_range&) {
		std::cerr << "Argument out of range: " << str << std::endl;
	}

	return false;
}

}
