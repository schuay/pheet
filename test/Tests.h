/*
 * Tests.h
 *
 *  Created on: 21 jun 2012
 *      Author: Daniel Cederman
 */

#ifndef TESTS_H_
#define TESTS_H_


#include "init.h"

namespace pheet {

class Tests {
public:
  virtual ~Tests() {};
  virtual void run_test(bool usestrategy) = 0;
};

}


#endif /* TESTS_H_ */
