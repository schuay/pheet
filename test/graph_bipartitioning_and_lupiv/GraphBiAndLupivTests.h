/*
* GraphBiAndLupivTests.h
*
*  Created on: 5.12.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/


#ifndef GRAPHBIANDLUPIVTESTS_H_
#define GRAPHBIANDLUPIVTESTS_H_

#include "../Test.h"
#include "../Tests.h"

#include "../init.h"

namespace pheet {

	class GraphBiAndLupivTests : public Tests
	{
	public:
		void run_test(bool usestrategy);
	private:
		template <class Scheduler>
		void test();
	};

}

#endif /* GRAPHBIANDLUPIVTESTS_H_ */
