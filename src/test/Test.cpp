/*
 * Test.cpp
 *
 *  Created on: 07.04.2011
 *      Author: mwimmer
 */

#include "Test.h"

Test::Test() {
	// TODO Auto-generated constructor stub

}

Test::~Test() {
	// TODO Auto-generated destructor stub
}

void Test::check_time(Time & time) {
	gettimeofday(&time, NULL);
}

double Test::calculate_seconds(const Time & start, const Time & end) {
	double dStart = 1.0e-6 * start.tv_usec;
	double dStop = (end.tv_sec - start.tv_sec) + 1.0e-6 * end.tv_usec;

	return dStop - dStart;
}
