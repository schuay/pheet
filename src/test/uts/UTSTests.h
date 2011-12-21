/*
* UTSTests.h
*
*  Created on: 5.12.2011
*      Author: Daniel Cederman
*     License: Pheet license
*/


#ifndef UTSTESTS_H_
#define UTSTESTS_H_

#include "../Test.h"
#include "../../test_settings.h"

namespace pheet {

	class UTSTests
	{
	public:
		void run_test();
	private:
		template <class Scheduler>
		void test();
	};

}

#endif /* INAROWTESTS_H_ */
