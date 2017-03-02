/***************************************************************************
    copyright            : (C) 2003 by Giuseppe Lipari
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __EDFSCHED_HPP__
#define __EDFSCHED_HPP__

#include <scheduler.hpp>

/**
       Model for the EDF scheduler
 */
class EDFModel: public TaskModel
{
protected:
	bool extP;
	int prio;

public:
	EDFModel(AbsRTTask* t) :TaskModel(t), extP(false) {}
	sc_time getPriority()
	{
		if (extP) return sc_time::from_value(prio);
		else return _rtTask->getDeadline();
	}
	void changePriority(sc_time p)
	{
		if (p == _rtTask->getDeadline()) extP = false;
		else {
			extP = true;
			prio = p.value();
		}
	}

};


/**
       This class implements an EDF scheduler. It redefines only
       the addTask function, because most of the work is done in
       the Scheduler class.
 */
class EDFScheduler: public Scheduler
{
public:
	/**
	 *  Creates an EDFModel, passing the task!! It throws
	 *  a * RTSchedExc exception if the task is already
	 *  present in this * scheduler.
	 */
	void addTask(AbsRTTask* task) throw (RTSchedExc);
	void addTask(AbsRTTask* task, const std::string &p);
	void removeTask(AbsRTTask* task);
	static EDFScheduler *createInstance(vector<string> &par);

};

#endif
