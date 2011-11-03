/*
* InARowTests.h
*
*  Created on: 22.09.2011
*      Author: Daniel Cederman
*     License: Pheet license
*/

#include "InARowTests.h"
#include "InARowGameTask.h"
#include "../test_schedulers.h"
#include <iostream>

namespace pheet {

	const unsigned int scenario1[] = {2,4,3,1,1,2,3,4,5,4,1,0};
	const unsigned int scenario2[] = {3,2,1,1,4,1,2,4,3,4,4,3,1,5,6,7,0};
	template <class Scheduler>
	void InARowTests::test(unsigned int width, unsigned int height, unsigned int rowlength, unsigned int lookahead, procs_t cpus, unsigned int* scenario)
	{
		typename Scheduler::CPUHierarchy hier(cpus);
		Scheduler s(&hier);
		Time start, end;
		check_time(start);
		s.template finish<InARowGameTask<typename Scheduler::Task> >(width, height, rowlength, lookahead, scenario);
		check_time(end);
		std::cout << "Time: " << calculate_seconds(start, end) << std::endl;
		s.print_performance_counter_headers();
		std::cout << endl;
		s.print_performance_counter_values();
	}

	void InARowTests::run_test()
	{
		//test<DefaultBasicScheduler>(8,8,4,4,2,(unsigned int*)scenario2);
		//test<DefaultBasicScheduler>(8,8,4,7,8,(unsigned int*)scenario2);
		
		//test<DefaultSynchroneousScheduler>(8,8,4,10,1,(unsigned int*)scenario1);
		//		return;
		//test<DefaultBasicScheduler>(8,8,4,7,8,(unsigned int*)scenario2);
		
		  test<PrimitivePriorityScheduler>(8,8,4,7,8,(unsigned int*)scenario2);
	}

}
