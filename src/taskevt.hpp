/***************************************************************************
    Author: Jun Xiao and Cesare Bartolini
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __TASKEVT_HPP__
#define __TASKEVT_HPP__

#include "systemc.h"
#include <abstask.hpp>

class Task;

/**
       task
       This class is the base class for all task events.
       handler
 */
class TaskEvt : public sc_event
{
public:
	Task* _task;
	int _cpu;

public:
	TaskEvt(Task* t) :
		sc_event(), _cpu(-1)  {_task = t;}
	Task* getTask() const {return _task;}
	void setTask(Task* t) {_task = t;}

	int getCPU() {return _cpu;}
	void setCPU(int cpu) {_cpu = cpu;}
	virtual void handler() {;}
};

// arrival event for a task
class ArrEvt: public TaskEvt
{
public:
	ArrEvt(Task* t) :TaskEvt(t) {}
	virtual void handler();

};

/// end of instance event
class EndEvt: public TaskEvt
{
public:
	//static const int _END_EVT_PRIORITY = _DEFAULT_PRIORITY - 2;
	EndEvt(Task* t) :TaskEvt(t) {}
	virtual void handler();
};

/// when a task is killed
class KillEvt: public TaskEvt
{
public:
	//static const int _END_EVT_PRIORITY = _DEFAULT_PRIORITY - 2;
	KillEvt(Task* t) :TaskEvt(t) {}
	virtual void handler();
};

/// when the event is triggered, the task starts executing on processor
class SchedEvt: public TaskEvt
{
public:
	SchedEvt(Task* t) : TaskEvt(t) {}
	virtual void handler();
};

/// when this event is triggered, the task does not
/// execute anymore on the processor.
class DeschedEvt: public TaskEvt
{
public:
	DeschedEvt(Task* t) :TaskEvt(t) {}
	virtual void handler();
};

/// to handle buffered arrivals
class FakeArrEvt: public TaskEvt
{
public:
	FakeArrEvt(Task* t) :TaskEvt(t) { }
	virtual void handler();
};

/**
 */
class DlineSetEvt: public TaskEvt
{
protected:
	sc_time _dline;

public:
	DlineSetEvt(Task* t) :TaskEvt(t) {}
	virtual void handler() {}
	void setDline(sc_time d) {_dline = d;}
	sc_time getDline() {return _dline;}

};

class DeadEvt: public TaskEvt
{
public:
	bool _abort;
	bool _kill;
public:
	DeadEvt(Task* t, bool abort = false, bool kill = false)
: TaskEvt(t), _abort(abort), _kill(kill) {}

	virtual void handler();
	void setAbort(bool f) {_abort = f;}
	void setKill(bool f) {_kill = f;}
};


#endif
