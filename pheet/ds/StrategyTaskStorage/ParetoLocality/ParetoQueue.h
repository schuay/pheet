/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef PARETOQUEUE_H_
#define PARETOQUEUE_H_

namespace pheet
{

template <class Item>
class ParetoQueue
{
public:

	/**
	 * Returns the first non-dominated element e and removes it from the queue.
	 *
	 * If no such item is in the queue, the nullptr is returned.
	 *
	 */
	virtual Item* first() = 0;

	/**
	 * Inserts path into the queue.
	 */
	virtual void insert(Item* item) = 0;

	/**
	 * Returns true if the queue contains zero non-dominated items.
	 */
	virtual bool empty() = 0;
};



} /* namespace pheet */


#endif // PARETOQUEUE_H_
