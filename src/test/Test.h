/*
 * Test.h
 *
 *  Created on: 07.04.2011
 *      Author: mwimmer
 */

#ifndef TEST_H_
#define TEST_H_

#include <sys/time.h>
#include <stdlib.h>

namespace pheet {

class Test {
public:
	typedef struct timeval Time;

	Test();
	virtual ~Test();

	virtual void run_test() = 0;

	void check_time(Time & time);
	double calculate_seconds(const Time & start, const Time & end);
};

}


#endif /* TEST_H_ */
