/*
 * NQueensTests.cpp
 *
 *  Created on: 2011-10-11
 *      Author: Anders Gidenstam, Martin Wimmer
 *     License: Pheet license
 */

#include "NQueensTests.h"

#include "ParallelRecursive/ParallelRecursiveNQueens.h"

#include "../test_schedulers.h"

#include <iostream>

namespace pheet {

NQueensTests::NQueensTests() {

}

NQueensTests::~NQueensTests() {

}

void NQueensTests::run_test() {
	if(nqueens_test) {
		std::cout << "----" << std::endl;

		this->run_solver<ParallelRecursiveNQueens<DefaultSynchroneousScheduler> >();
                this->run_solver<ParallelRecursiveNQueens<DefaultBasicScheduler> >();
                this->run_solver<ParallelRecursiveNQueens<DefaultMixedModeScheduler> >();
                this->run_solver<ParallelRecursiveNQueens<PrimitiveHeapPriorityScheduler> >();
                this->run_solver<ParallelRecursiveNQueens<PrimitivePriorityScheduler> >();
                this->run_solver<ParallelRecursiveNQueens<FallbackPriorityScheduler> >(); // Problems here?
	}
}

}
