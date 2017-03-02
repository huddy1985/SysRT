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
#ifndef __FPSCHED_HPP__
#define __FPSCHED_HPP__

#include <scheduler.hpp>

//#define _FP_SCHED_DBG	"fpsched"
//#define _FP_SCHED_DBG_LEV "fpsched"

/**
       This class implements a generic fixed priority scheduler.
       This class redefines only the addTask function, because most of the
       work is done in the Scheduler class. The user must specify the
       priority of each task esplicitily.
 */
class FPScheduler : public Scheduler
{
protected:

	class FPModel: public TaskModel
	{
	protected:
		sc_time _prio;

	public:
		FPModel(AbsRTTask *t, sc_time p) : TaskModel(t), _prio(p) {}
		sc_time getPriority() { return _prio; }

		void setPriority(sc_time p) {_prio = p; }

		void changePriority(sc_time p) {
			setPriority(p);
		}
	};


	public:
	FPScheduler(sc_module_name name = ""): Scheduler(name){
	};
	/**
           Empty definition of pure virtual function addTask.
           @param t task to be added.
	 */
	void addTask(AbsRTTask *t)  {}

	/**
           Create an FPModel passing the task and the priority. It throws a
           RTSchedExc exception if the task is already present in this
           scheduler.
           @param t task to be added.
           @param prio task's priority.
	 */
	void addTask(AbsRTTask *t, int prio);

	void addTask(AbsRTTask *t, const std::string &p);

	void removeTask(AbsRTTask *t) {}

	static FPScheduler *createInstance(vector<string> &par);

};


#endif
