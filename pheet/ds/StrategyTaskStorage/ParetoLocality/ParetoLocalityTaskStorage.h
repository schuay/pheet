/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef PARETOLOCALITYTASKSTORAGE_H_
#define PARETOLOCALITYTASKSTORAGE_H_

namespace pheet
{

template <class Pheet, class Strategy>
class ParetoLocalityTaskStorage : public Pheet::Scheduler::BaseTaskStorage
{
public:
	typedef ParetoLocalityTaskStorage<Pheet, Strategy> Self;

};

} /* namespace pheet */

#endif /* PARETOLOCALITYTASKSTORAGE_H_ */
