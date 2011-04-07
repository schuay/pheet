/*
 * SortingTests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: mwimmer
 */

#include "SortingTests.h"
#include "Reference/ReferenceSTLSort.h"

#include <iostream>

SortingTests::SortingTests() {

}

SortingTests::~SortingTests() {

}

void SortingTests::run_test() {
	std::cout << "----" << std::endl;
	std::cout << "test\tsorter\ttype\tsize\tseed\tcpus\ttime\truns" << std::endl;

	this->run_sorter<ReferenceSTLSort>();
}
