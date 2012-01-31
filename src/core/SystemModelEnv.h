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

#include "../models/CPUHierarchy/Oversubscribed/OversubscribedSimpleCPUHierarchy.h"

namespace pheet {

template <class Env, class MachineModelT>
class SystemModelEnv {
public:
	typedef MachineModelT MachineModel;
};

template <class Env>
class PheetSystemModel : public SystemModelEnv<Env, OversubscribedSimpleCPUHierarchy> {
	typedef SystemModelEnv<Env> Base;
	typedef typename Base::MachineModel MachineModel;
};

}

#endif /* SYSTEMMODELENV_H_ */
