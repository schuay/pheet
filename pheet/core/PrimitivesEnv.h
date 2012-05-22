/*
 * PrimitivesEnv.h
 *
 *  Created on: Jan 31, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PRIMITIVESENV_H_
#define PRIMITIVESENV_H_

#include "../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../primitives/Barrier/Simple/SimpleBarrier.h"
#include "../primitives/Finisher/Basic/Finisher.h"

namespace pheet {

template <class Env, template <class E> class BackoffT, template <class E> class BarrierT, template <class> class FinisherT>
class PrimitivesEnv {
public:
	typedef BackoffT<Env> Backoff;
	typedef BarrierT<Env> Barrier;
	typedef FinisherT<Env> Finisher;
};

template<class Pheet>
using Primitives = PrimitivesEnv<Pheet, ExponentialBackoff, SimpleBarrier, Finisher>;

}

#endif /* PRIMITIVESENV_H_ */
