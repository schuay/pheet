/*
 * type_traits.h
 *
 *  Created on: 12.04.2011
 *      Author: mwimmer
 */

#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

#include <stdlib.h>

namespace pheet {

template <typename T>
class nullable_traits {
	static const T null_value;
};

template <typename T>
class nullable_traits<T*> {
	static T* const null_value;
};

template <typename T>
T* const nullable_traits<T*>::null_value = NULL;

}


#endif /* TYPE_TRAITS_H_ */
