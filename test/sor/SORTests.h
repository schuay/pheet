/*
* SORTests.h
*
*  Created on: 5.12.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/


#ifndef SORTESTS_H_
#define SORTESTS_H_

#include "../Test.h"
#include "../Tests.h"

#include "../init.h"

namespace pheet {

	class SORTests : public Tests
	{
	public:
		void run_test(bool usestrategy);
	private:
		template <class Scheduler>
		void test(bool prio);
	};

}

#endif /* SORTESTS_H_ */
