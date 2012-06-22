/*
* InARow.h
*
*  Created on: 22.09.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/

#ifndef SORTASK_H_
#define SORTASK_H_

#include <pheet/pheet.h>
#include "../../../pheet/misc/types.h"
#include "../../../pheet/misc/atomics.h"
#include "../../../pheet/primitives/Reducer/List/ListReducer.h"
//#include "../../test_schedulers.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <exception>

#include "SORPerformanceCounters.h"
#include "SORLocalityStrategy.h"

using namespace std;

namespace pheet {

  class SORParams
	{
	public:
		double** G;
		int M;
		int N;
		double omega;
		int slices;
		double total;
		bool prio;
	};

	template <class Pheet>
	class SORSliceTask;

	template <class Pheet>
	class SORStartTask : public Pheet::Task
	{
		SORParams& sp;
		int iterations;
		SORPerformanceCounters<Pheet> pc;

	public:

		SORStartTask(SORParams& sp, int iterations, SORPerformanceCounters<Pheet>& pc):sp(sp),iterations(iterations),pc(pc) { }
		virtual ~SORStartTask() {}

		void operator()()
		{
			typename Pheet::Place** column_owners = new typename Pheet::Place*[sp.slices];


			for(int i=0;i<sp.slices;i++)
				column_owners[i]=0;

			for (int p=0; p<2*iterations; p++) 
			  {
			    {
			      typename Pheet::Finish f;
			      
			      for(int i = 0; i < sp.slices; i++)
				{
				  if(!sp.prio)
				    Pheet::template spawn<SORSliceTask<Pheet> >(column_owners+i,i,sp,p,pc);
				  else
				    Pheet::template spawn_s<SORSliceTask<Pheet>>(SORLocalityStrategy<Pheet>(column_owners[i]),column_owners+i,i,sp,p,pc);
				}
			    }
			}
			
				//			for(int i=0;i<sp.slices;i++)
				//	printf("%X ",(long long)column_owners[i]);
				//printf("\n");

			int Mm1 = sp.M-1;
			int Nm1 = sp.N-1;
			sp.total = 0;
			for (int i=1; i<Mm1; i++) {
				for (int j=1; j<Nm1; j++) {
					sp.total += sp.G[i][j];
						
				}
			}   
		}
	};


	template <class Pheet>
	class SORSliceTask : public Pheet::Task
	{
		typename Pheet::Place** owner_info;
		int id;
		SORParams sp;
		int p;
		SORPerformanceCounters<Pheet> pc;
	public:

		SORSliceTask(typename Pheet::Place** owner_info, int id, SORParams& sp, int p, SORPerformanceCounters<Pheet>& pc):owner_info(owner_info),id(id),sp(sp),p(p),pc(pc) {}
		virtual ~SORSliceTask() {}

		void operator()()
		{
		  struct timeval start_time;
		  gettimeofday(&start_time, NULL);

			if(*owner_info==Pheet::get_place())
				pc.slices_rescheduled_at_same_place.incr();

		  	if(*owner_info == 0)
			  (*owner_info) = Pheet::get_place();
			double omega_over_four = sp.omega * 0.25;
			double one_minus_omega = 1.0 - sp.omega;

			// update interior points
			//
			int Mm1 = sp.M-1;
			int Nm1 = sp.N-1;

			int ilow, iupper, slice, tslice, ttslice;

			tslice = (Mm1) / 2;
			ttslice = (tslice + sp.slices-1)/sp.slices;
			slice = ttslice*2;

			ilow=id*slice+1;
			iupper = ((id+1)*slice)+1;
			if (iupper > Mm1) iupper =  Mm1+1;
			if (id == (sp.slices-1)) iupper = Mm1+1;

			double** G = sp.G;

			for (int i=ilow+(p%2); i<iupper; i=i+2) {


				double* Gi = G[i];
				double* Gim1 = G[i-1];


				if(i == 1) { 
					double* Gip1 = G[i+1];

					for (int j=1; j<Nm1; j=j+2){
						Gi[j] = omega_over_four * (Gim1[j] + Gip1[j] + Gi[j-1]
						+ Gi[j+1]) + one_minus_omega * Gi[j];

					}
				} else if (i == Mm1) {

					double* Gim2 = G[i-2];

					for (int j=1; j<Nm1; j=j+2){
						if((j+1) != Nm1) {
							Gim1[j+1]=omega_over_four * (Gim2[j+1] + Gi[j+1] + Gim1[j]
							+ Gim1[j+2]) + one_minus_omega * Gim1[j+1];
						}
					}

				} else {

					double* Gip1 = G[i+1];
					double* Gim2 = G[i-2];

					for (int j=1; j<Nm1; j=j+2){
						Gi[j] = omega_over_four * (Gim1[j] + Gip1[j] + Gi[j-1]
						+ Gi[j+1]) + one_minus_omega * Gi[j];

						if((j+1) != Nm1) {
							Gim1[j+1]=omega_over_four * (Gim2[j+1] + Gi[j+1] + Gim1[j]
							+ Gim1[j+2]) + one_minus_omega * Gim1[j+1];
						}
					}
				}

			}


			struct timeval stop_time;
			gettimeofday(&stop_time, NULL);
			TaskSched ts(start_time, stop_time,(size_t)Pheet::get_place());
			pc.red.add(ts);


		}

	};
}
#endif /* INAROWTASK_H_ */
