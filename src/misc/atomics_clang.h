/*
 * atomics_clang.h
 *
 *  Created on: Feb 2, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef ATOMICS_CLANG_H_
#define ATOMICS_CLANG_H_

#define PTR_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap(p, old_v, new_v))
#define INT_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap(p, old_v, new_v))
#define INT32_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap(p, old_v, new_v))
#define INT64_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap(p, old_v, new_v))
#define UINT64_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap((signed*)p, old_v, new_v))
#define UINT_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap((signed*)p, old_v, new_v))
#define LONG_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap(p, old_v, new_v))
#define ULONG_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap((signed*)p, old_v, new_v))
#define SIZET_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap((signed*)p, old_v, new_v))
#define PTRDIFFT_CAS(p, old_v, new_v)	(__sync_bool_compare_and_swap(p, old_v, new_v))

#define INT_ATOMIC_ADD(p, val)		(__sync_fetch_and_add(p, val))
#define UINT_ATOMIC_ADD(p, val)		(__sync_fetch_and_add((signed*)p, val))
#define LONG_ATOMIC_ADD(p, val)		(__sync_fetch_and_add(p, val))
#define ULONG_ATOMIC_ADD(p, val)	(__sync_fetch_and_add((signed*)p, val))
#define SIZET_ATOMIC_ADD(p, val)	(__sync_fetch_and_add((signed*)p, val))

#define INT_FETCH_AND_ADD(p, val)		(__sync_fetch_and_add(p, val))
#define UINT_FETCH_AND_ADD(p, val)		(__sync_fetch_and_add((signed*)p, val))
#define LONG_FETCH_AND_ADD(p, val)		(__sync_fetch_and_add(p, val))
#define ULONG_FETCH_AND_ADD(p, val)		(__sync_fetch_and_add((signed*)p, val))
#define SIZET_FETCH_AND_ADD(p, val)		(__sync_fetch_and_add((signed*)p, val))

#define INT_ATOMIC_SUB(p, val)		(__sync_fetch_and_sub(p, val))
#define UINT_ATOMIC_SUB(p, val)		(__sync_fetch_and_sub((signed*)p, val))
#define LONG_ATOMIC_SUB(p, val)		(__sync_fetch_and_sub(p, val))
#define ULONG_ATOMIC_SUB(p, val)	(__sync_fetch_and_sub((signed*)p, val))
#define SIZET_ATOMIC_SUB(p, val)	(__sync_fetch_and_sub((signed*)p, val))

#define INT_FETCH_AND_SUB(p, val)		(__sync_fetch_and_sub(p, val))
#define UINT_FETCH_AND_SUB(p, val)		(__sync_fetch_and_sub((signed*)p, val))
#define LONG_FETCH_AND_SUB(p, val)		(__sync_fetch_and_sub(p, val))
#define ULONG_FETCH_AND_SUB(p, val)		(__sync_fetch_and_sub((signed*)p, val))
#define SIZET_FETCH_AND_SUB(p, val)		(__sync_fetch_and_sub((signed*)p, val))

#define MEMORY_FENCE()				(__sync_synchronize())

#endif /* ATOMICS_CLANG_H_ */
