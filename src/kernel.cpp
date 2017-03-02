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
#include <algorithm>
#include <cpu.hpp>
#include <kernel.hpp>
#include <resmanager.hpp>
#include <scheduler.hpp>
#include <task.hpp>
#include <reginstr.hpp>
#include <systemc.h>

using namespace std;

RTKernel::~RTKernel()
{
	if (internalCpu == true){
		delete _cpu;
	}
	_currExe = NULL;
	endRun();

}

void RTKernel::addTask(AbsRTTask &t, const string &params)
{
	t.setKernel(this);
	_handled.push_back(&t);
	_sched->addTask(&t, params);
}
void RTKernel::addTask(AbsRTTask &t, int pri)
{
	t.setKernel(this);
	_handled.push_back(&t);
	_sched->addTask(&t, pri);
}

CPU* RTKernel::getProcessor(const AbsRTTask* t) const
{
	return _cpu;
}

CPU* RTKernel::getOldProcessor(const AbsRTTask* t) const
{
	return _cpu;
}
void RTKernel::terminate(AbsRTTask *task)
{
	_sched->extract(task);
}

void RTKernel::activate(AbsRTTask *task)
{
	_sched->insert(task);
}

AbsRTTask* RTKernel::getCurrExe() const
{
	return _currExe;
}

void RTKernel::suspend(AbsRTTask *task)
{
	_sched->extract(task);
	if (_currExe == task) {
		task->deschedule();
		_currExe = NULL;
		_preExe=task;
	}
}

void RTKernel::discardTasks(bool f)
{
	_sched->discardTasks(f);
	_handled.clear();
}

void RTKernel::onArrival(AbsRTTask *task)
{
	_sched->insert(task);
	if(!_isContextSwitching){
		dispatch();
	}
	/*The kernel is doing context switching*/
	else{
		beginDispatchEvt.cancel();
		//Compute the notification time of the next dispatch time
		beginDispatchEvt.notify(_contextSwitchDelay-(sc_time_stamp()-start_context_switch));
	}
}

void RTKernel::onEnd(AbsRTTask *task)
{
	_sched->extract(task);
	_currExe = NULL;
	//to record previous task executed
	_preExe = task;
	if(!_sched->_queue.empty())
		dispatch();
}

void RTKernel::dispatch()
{
	// we have only to notify an Dispatch sc_event (low priority)
	beginDispatchEvt.notify(SC_ZERO_TIME);
}
void RTKernel::dispatch(AbsRTTask* t)
{
	dispatch();
}
void RTKernel::onBeginDispatch()
{

	AbsRTTask *newExe = _sched->getFirst();

	if(_currExe != newExe){
		if (_currExe != NULL){
			_currExe->deschedule();
			_preExe=_currExe;
		}
		if( newExe != NULL) {
			if(newExe!=_preExe){
				_isContextSwitching = true;
				_currExe = newExe;
				endDispatchEvt.notify(_contextSwitchDelay);
				start_context_switch=sc_time_stamp();
			}
			// in case of buffered arrival and no context switch
			else{
				_currExe = newExe;
				_currExe->schedule();
				_sched->notify(newExe);
			}
		}
		else {
			_sched->notify(newExe);
		}

	}
}


void RTKernel::onEndDispatch()
{
	_currExe->schedule();
	_isContextSwitching = false;
	_sched->notify(_currExe);
}


void RTKernel::refresh()
{
	dispatch();
}

void RTKernel::setResManager(ResManager* rm)
{
	_resMng = rm;
	_resMng->setKernel(this, _sched);
}

bool RTKernel::requestResource(AbsRTTask *t, const string &r, int n)
throw(RTKernelExc)
{
	if (_resMng == 0) throw RTKernelExc("Resource Manager not set!");
	bool ret = _resMng->request(t,r,n);
	if (!ret)
		dispatch();
	return ret;
}

void RTKernel::releaseResource(AbsRTTask *t, const string &r, int n)
throw(RTKernelExc)
{
	if (_resMng == 0) throw RTKernelExc("Resource Manager not set!");

	_resMng->release(t,r,n);

	dispatch();
}

void RTKernel::setThreshold(const int th)
{
	_sched->setThreshold(_currExe, th);
}

int RTKernel::enableThreshold()
{
	int tmp = _sched->enableThreshold(_currExe);
	return tmp;
}

void RTKernel::disableThreshold()
{
	_sched->disableThreshold(_currExe);
}

void RTKernel::printState() const
{
}

void RTKernel::newRun()
{
	_currExe = NULL;
}

void RTKernel::endRun()
{
	_currExe = NULL;
}

void RTKernel::print() const
{
}

std::vector<std::string>  RTKernel::getRunningTasks()
{
	std::vector<std::string> tmp_ts;
	std::string tmp_name = taskname(_currExe);

	if (tmp_name != "(nil)")
		tmp_ts.push_back(tmp_name);

	return tmp_ts;
}
