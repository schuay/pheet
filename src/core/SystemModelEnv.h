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

template <class Env, template <class Env> class MachineModelT>
class SystemModelEnv {
public:
	typedef MachineModelT<Env> MachineModel;
};

template <class Env>
class PheetSystemModel : public SystemModelEnv<Env, HWLocMachineModel> {
public:
	typedef SystemModelEnv<Env, HWLocMachineModel> Base;
	typedef typename Base::MachineModel MachineModel;
};

}

#endif /* SYSTEMMODELENV_H_ */
