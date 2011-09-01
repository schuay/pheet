/*
 * PerformanceCounterArray.h
 *
 *  Created on: 01.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef PERFORMANCECOUNTERARRAY_H_
#define PERFORMANCECOUNTERARRAY_H_

/*
 *
 */
namespace pheet {

template <template <typename S> class PC, class bool> class BasicPerformanceCounter;

template <template <typename S> class PC>
class PerformanceCounterArray<PC, false> {
public:
	PerformanceCounterArray(size_t size);
	PerformanceCounterArray(PerformanceCounterArray& other);
	~PerformanceCounterArray();

	PC<false>& operator[](size_t i);
	PC<false> pc;
};

template <template <typename S> class PC>
PerformanceCounterArray<PC, false>::PerformanceCounterArray(size_t size) {

}

template <template <typename S> class PC>
PerformanceCounterArray<PC, false>::PerformanceCounterArray() {

}

template <template <typename S> class PC>
PerformanceCounterArray<PC, false>::~PerformanceCounterArray() {

}

template <template <typename S> class PC>
PC<false>& PerformanceCounterArray<PC, false>::operator[](size_t i) {
	return pc;
}

template <template <typename S> class PC>
class PerformanceCounterArray<PC, true> {
public:
	PerformanceCounterArray(size_t size);
	~PerformanceCounterArray();

	PC<true>& operator[](size_t i);
	PC<true>* pc;
};

template <template <typename S> class PC>
PerformanceCounterArray<PC, true>::PerformanceCounterArray(size_t size) {

}

template <template <typename S> class PC>
PerformanceCounterArray<PC, true>::PerformanceCounterArray(PerformanceCounterArray& other) {

}

template <template <typename S> class PC>
PerformanceCounterArray<PC, true>::~PerformanceCounterArray() {

}

template <template <typename S> class PC>
PC<true>& PerformanceCounterArray<PC, true>::operator[](size_t i) {
	return pc;
}

}

#endif /* PERFORMANCECOUNTERARRAY_H_ */
