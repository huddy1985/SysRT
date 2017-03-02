/*
 * parti_mrtkernel.cpp
 *
 *  Created on: Dec 9, 2015
 *      Author: Jun Xiao
 */

#include "parti_mrtkernel.hpp"
#include "task.hpp"
#include <iterator>
#include <cpu.hpp>
#include <scheduler.hpp>
#include <algorithm>
#include <vector>

Parti_Mrtkernel::Parti_Mrtkernel(Scheduler* s, absCPUFactory* fact, int n ,
		sc_module_name name) :
		MRTKernel(s, fact, n, name) {
	;
}

/**
 * Constructor: needs to know which scheduler the kernel want
 * to use, and from how many processor the system is composed.
 */
Parti_Mrtkernel::Parti_Mrtkernel(Scheduler* s, int n , sc_module_name name) : MRTKernel(s, n, name) {
	for(unsigned int i=0; i<cpus.size(); i++)  {
		need_dispatch[cpus[i]]=false;
	}

	SC_METHOD(dispatch_call);
	sensitive<<dispatch_ev;
	dont_initialize();
}

/**
 Needs to know scheduler and name
 */
Parti_Mrtkernel::Parti_Mrtkernel(Scheduler* s, sc_module_name name) :
		MRTKernel(s, name) {
	;
}
Parti_Mrtkernel::~Parti_Mrtkernel() {
	;
}

//two dispatch function for partition scheduling algorithms
void Parti_Mrtkernel::dispatch(AbsRTTask *task) {
	Task *t = dynamic_cast<Task *>(task);
	dispatch(cpus[t->task_affinity]);
}

void Parti_Mrtkernel::dispatch_interface2(AbsRTTask *task) {
	Task *t = dynamic_cast<Task *>(task);
	need_dispatch[cpus[t->task_affinity]] = true;
	dispatch_ev.notify(SC_ZERO_TIME);

}

void Parti_Mrtkernel::dispatch(CPU *p) {
	MRTKernel::dispatch(p);

}

void Parti_Mrtkernel::dispatch_interface1(CPU *p) {
	need_dispatch[p] = true;
	dispatch_ev.notify(SC_ZERO_TIME);
}

void Parti_Mrtkernel::dispatch() {
	for (auto iterator = cpus.begin(); iterator != cpus.end(); iterator++) {
		need_dispatch[*iterator]=true;
	}
	dispatch_ev.notify(SC_ZERO_TIME);
}

void Parti_Mrtkernel::dispatch_call() {
	for (auto iterator = cpus.begin(); iterator != cpus.end(); iterator++) {
		if(need_dispatch[*iterator]==true)
		{
			dispatch(*iterator);
			need_dispatch[*iterator]=false;
		}
	}
}

//dispatch handler
void Parti_Mrtkernel::onBeginDispatchMulti(CPU* e) {
	AbsRTTask *newExe = _sched->getFirst(e);
	if (_m_currExe[e] != newExe) {
		if (_m_currExe[e] != NULL) {
			_m_currExe[e]->deschedule();
			_m_preExe[e] = _m_currExe[e];
		}
		if (newExe != NULL) {
			if (newExe !=_m_preExe[e]) {
				_isContextSwitching[e] = true;
				_m_currExe[e] = newExe;
				e->end_event_notify(_contextSwitchDelay);
				start_context_switching[e] = sc_time_stamp();
			}
			// in case of buffered arrival and no context switch
			else {
				_m_currExe[e] = newExe;
				_m_currExe[e]->schedule();
				_sched->notify(newExe);
			}
		} else {
			_sched->notify(newExe);
		}
	}

}
void Parti_Mrtkernel::onEndDispatchMulti(CPU* p) {
	_m_currExe[p]->schedule();
	_isContextSwitching[p] = false;
	_sched->notify(_m_currExe[p]);
}

//task event
void Parti_Mrtkernel::onArrival(AbsRTTask *t) {
	_sched->insert(t);
	dispatch_interface2(t);

}
void Parti_Mrtkernel::suspend(AbsRTTask *task) {
	_sched->extract(task);
	CPU *p = getProcessor(task);
	if (p != NULL) {
		task->deschedule();
		_m_currExe[p] = NULL;
		_m_oldExe[task] = p;
		_m_dispatched[task] = NULL;
		_m_preExe[p] = task;
	}
}

void Parti_Mrtkernel::onEnd(AbsRTTask* task) {

	CPU *p = getProcessor(task);
	_sched->extract(task);
	_m_oldExe[task] = p;
	_m_currExe[p] = NULL;
	_m_preExe[p] = task;
	_m_dispatched[task] = NULL;

	dispatch_interface1(p);

}
 CPU* Parti_Mrtkernel::getProcessor(const AbsRTTask* task) const{
	 const Task *t = dynamic_cast<const Task *>(task);
	 return cpus[t->task_affinity];
 }

 CPU* Parti_Mrtkernel::getOldProcessor(const AbsRTTask *task) const{
	 const Task *t = dynamic_cast<const Task *>(task);
	 return cpus[t->task_affinity];

 }
