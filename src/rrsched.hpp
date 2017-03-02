/***************************************************************************
Author: Jun Xiao and Giuseppe Lipari
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __RRSCHED_HPP__
#define __RRSCHED_HPP__

#include <map>
#include <baseexc.hpp>
#include <plist.hpp>
#include <scheduler.hpp>

//#define _RR_SCHED_DBG_LEV "RRSched"

class RTKernel;

/**
        Round Robin scheduler. 
 */
class RRScheduler: public Scheduler
{
protected:

	class RRSchedExc : public BaseExc {
	public:
		RRSchedExc(string msg) :
			BaseExc(msg, "RoundRobinScheduler", "rrsched.cpp") {}
	};

	class RRModel : public TaskModel {
	protected:
		sc_time _rrSlice;
	public:
		RRModel(AbsRTTask* t) : TaskModel(t), _rrSlice(sc_time::from_value(1)) {}
		virtual ~RRModel() {}

		virtual sc_time getPriority();
		virtual void changePriority(sc_time p);

		/**
               Returns the slice size (in number of sc_times)
		 */
		sc_time getRRSlice() {return _rrSlice;}

		/**
                Sets the slice size to s (in number of sc_times)
		 */
		void setRRSlice(sc_time s) {_rrSlice = s;}

		/**
               This function returns true if the round has expired for the
               currently executing thread.
		 */
		bool isRoundExpired();
	};

	sc_event _rrEvt;

	int defaultSlice;

	public:

	/** Constructor */
	SC_HAS_PROCESS(RRScheduler);
	RRScheduler(sc_module_name name = "",int defSlice=1):
		Scheduler(name), defaultSlice(defSlice)
	{

		SC_METHOD(round);
		dont_initialize();
		sensitive << _rrEvt;
	}
	/**
           Set the Round Robin slice.
	 */
	virtual void setRRSlice(AbsRTTask* task, sc_time slice);

	/**
           Notify to recompute the round
	 */
	virtual void notify(AbsRTTask* task);

	/**
           This is called by the sc_event rrEvt.
	 */
	void round();
	void addTask(AbsRTTask *t) throw(RRSchedExc);
	void addTask(AbsRTTask *t, const std::string &p);
	void removeTask(AbsRTTask *t) {}
};

#endif
