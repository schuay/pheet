/*
 * SystemModelEnv.h
 *
 *  Created on: Jan 31, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef SYSTEMMODELENV_H_
#define SYSTEMMODELENV_H_

#include "../settings.h"

#include "../models/MachineModel/HWLoc/HWLocMachineModel.h"

namespace pheet {

template <class Env, template <class E> class MachineModelT>
class SystemModelEnv {
public:
	typedef MachineModelT<Env> MachineModel;

	template <template <class P> class NewMM>
	using WithMachineModel = SystemModelEnv<Env, NewMM>;

	template <class P>
	using T = SystemModelEnv<P, MachineModelT>;
};

template<class Pheet>
using SystemModel = SystemModelEnv<Pheet, HWLocMachineModel>;

}

#endif /* SYSTEMMODELENV_H_ */
