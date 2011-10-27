/*
* InARowTests.h
*
*  Created on: 22.09.2011
*      Author: Daniel Cederman
*     License: Pheet license
*/


#ifndef INAROWTESTS_H_
#define INAROWTESTS_H_

#include "../Test.h"
#include "../../test_settings.h"

namespace pheet {

	class InARowTests : Test
	{
	public:
		void run_test();
	private:
		template <class Scheduler>
		void test(unsigned int width, unsigned int height, unsigned int rowlength, unsigned int lookahead, procs_t cpus, unsigned int* scenario);
	};

}

#endif /* INAROWTESTS_H_ */
