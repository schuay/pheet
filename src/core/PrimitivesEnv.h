/*
 * PrimitivesEnv.h
 *
 *  Created on: Jan 31, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIMITIVESENV_H_
#define PRIMITIVESENV_H_

#include "../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../primitives/Barrier/Simple/SimpleBarrier.h"

namespace pheet {

template <class Env, template <class E> class BackoffT, template <class E> class BarrierT>
class PrimitivesEnv {
public:
	typedef BackoffT<Env> Backoff;
	typedef BarrierT<Env> Barrier;
};

template <class Env>
class PheetPrimitives : public PrimitivesEnv<Env, StandardExponentialBackoff, SimpleBarrier> {
public:
	typedef PrimitivesEnv<Env, StandardExponentialBackoff, SimpleBarrier> Base;
	typedef typename Base::Backoff Backoff;
	typedef typename Base::Barrier Barrier;
};
//typedef PrimitivesEnv<ExponentialBackoff, SimpleBarrier> PheetPrimitives;

}

#endif /* PRIMITIVESENV_H_ */
