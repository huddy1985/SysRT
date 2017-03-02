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
#include <mrtkernel.hpp>
#include <resmanager.hpp>
#include <scheduler.hpp>
#include <task.hpp>
#include <iterator>

using namespace std;

template<class IT>
void clean_mapcontainer(IT b, IT e)
{
	for (IT i=b; i!=e; i++) delete i->second;
}

vector<CPU*> MRTKernel::getProcessors() const
{
	vector<CPU*> s;

	typedef map<CPU *, AbsRTTask *>::const_iterator IT;
	int j = 0;

	for(IT i = _m_currExe.begin(); i != _m_currExe.end(); i++, j++)
		s[j] = i->first;
	return s;
}

CPU *MRTKernel::getFreeProcessor()
{
	for(ITCPU i=_m_currExe.begin(); i!=_m_currExe.end(); i++)
		if (i->second == NULL) return i->first;
	return NULL;
}


bool MRTKernel::isDispatched(CPU *p)
{
	map<const AbsRTTask *, CPU *>::iterator j = _m_dispatched.begin();

	for (; j !=_m_dispatched.end(); ++j )
		if (j->second == p) return true;

	return false;
}

MRTKernel::ITCPU MRTKernel::getNextFreeProc(ITCPU s, ITCPU e)
{
	for (ITCPU i=s; i!=e; ++i)
		if (i->second == NULL && !isDispatched(i->first)) return i;

	return e;
}



void MRTKernel::internalConstructor(int n)
{
	for(int i=0; i<n; i++)  {
		CPU *c = _CPUFactory->createCPU();
		_m_currExe[c] = NULL;
		_isContextSwitching[c] = false;
		start_context_switching[c]=sc_time(0,SC_PS);
		switch_overhead[c]=sc_time(0,SC_PS);
		c->setKernel(this);
		cpus.push_back(c);

	}
	migration=0;
	context_switch=0;

	_sched->setKernel(this);
	cstmc.open("Results/csmc.txt", fstream::in | fstream::out | fstream::app);
}

MRTKernel::MRTKernel(Scheduler *s, absCPUFactory *fact, int n,
		sc_module_name name)
: RTKernel(s,name) , _CPUFactory(fact), _migrationDelay(sc_time::from_value(0))
{
	internalConstructor(n);

}

MRTKernel::MRTKernel(Scheduler *s, int n, sc_module_name name)
: RTKernel(s, name), _migrationDelay(sc_time::from_value(0))
{
	_CPUFactory = new uniformCPUFactory();

	internalConstructor(n);
}

MRTKernel::MRTKernel(Scheduler *s, sc_module_name name)
: RTKernel(s, name), _migrationDelay(sc_time::from_value(0))
{
	_CPUFactory = new uniformCPUFactory();

	internalConstructor(1);
	newRun();
}

MRTKernel::~MRTKernel()
{
	cout<<"total number of context switch: "<<context_switch<<"\n";
	cout<<"total number of task migration: "<<migration<<"\n";
	cstmc<<"total number of context switch: "<<context_switch<<"\n";
	cstmc<<"total number of task migration: "<<migration<<"\n";
	cstmc.close();
	delete _CPUFactory;
	endRun();
}

void MRTKernel::addCPU(CPU *c)
{
	_m_currExe[c] = NULL;
	_isContextSwitching[c] = false;
	cpus.push_back(c);
}

void MRTKernel::addTask(AbsRTTask &t, const string &param)
{
	RTKernel::addTask(t, param);
	_m_oldExe[&t] = NULL;
	_m_dispatched[&t] = NULL;
}

void MRTKernel::addTask(AbsRTTask &t, int param)
{
	RTKernel::addTask(t, param);
	_m_oldExe[&t] = NULL;
	_m_dispatched[&t] = NULL;
}

CPU *MRTKernel::getProcessor(const AbsRTTask *t) const
{
	CPU* ret = NULL;

	typedef  map<CPU*, AbsRTTask*>::const_iterator IT;

	for (IT i = _m_currExe.begin(); i != _m_currExe.end(); i++)
		if (i->second == t) ret = i->first;
	return ret;
}

CPU* MRTKernel::getOldProcessor(const AbsRTTask* t) const
{
	CPU *ret = NULL;
	ret = _m_oldExe.find(t)->second;
	return ret;
}

void MRTKernel::suspend(AbsRTTask *task)
{
	_sched->extract(task);
	CPU *p = getProcessor(task);
	if (p != NULL){
		task->deschedule();
		_m_currExe[p] = NULL;
		_m_oldExe[task] = p;
		_m_dispatched[task] = NULL;
	}
}
//void MRTKernel::rrsuspend(AbsRTTask *task)
//{
//	_sched->extract(task);
//	CPU *p = getProcessor(task);
//	if (p != NULL){
//		_m_currExe[p] = NULL;
//		_m_oldExe[task] = p;
//		_m_dispatched[task] = NULL;
//	}
//}

void MRTKernel::onArrival(AbsRTTask *t)
{
	_sched->insert(t);
	dispatch();
}

void MRTKernel::onEnd(AbsRTTask *task)
{
	CPU *p = getProcessor(task);
	_sched->extract(task);
	_m_oldExe[task] = p;
	_m_currExe[p] = NULL;
	_m_dispatched[task] = NULL;
	dispatch(p);
}
void MRTKernel::terminate(AbsRTTask *task)
{
	_sched->extract(task);
	_m_dispatched[task] = NULL;
}
void MRTKernel::dispatch()
{
	int ncpu = _m_currExe.size();
	int num_newtasks = 0; // tells us how many "new" tasks in the
	// ready queue
	int i;

	for (i=0; i<ncpu; ++i) {
		AbsRTTask *t = _sched->getTaskN(i);
		if (t == NULL) break;
		else if (getProcessor(t) == NULL &&
				_m_dispatched[t] == NULL) num_newtasks++;
	}
	if (num_newtasks == 0) return; // nothing to do

		ITCPU start = _m_currExe.begin();
	ITCPU stop = _m_currExe.end();
	ITCPU f = start;
	do {
		f = getNextFreeProc(f, stop);
		if (f != stop) {
			dispatch(f->first);
			num_newtasks--;
			f++;
		}
		else { // no more free processors
			// now we deschedule tasks
			for(;;) {
				AbsRTTask *t = _sched->getTaskN(i++);
				CPU *c = _m_dispatched[t];
				if (c != NULL) {
					dispatch(c);
					num_newtasks--;
					break;
				}
			}
		}
	} while (num_newtasks > 0);
}

void MRTKernel::dispatch(CPU *p)
{

	context_switch++;
	p->begin_event_cancel();
	if (_isContextSwitching[p]) {
		p->begin_event_notify(switch_overhead[p]-(sc_time_stamp()-start_context_switching[p]));
		p->end_event_cancel();
		if (p->_endEvt.getTask() != NULL)
			_m_dispatched[p->_endEvt.getTask()] = NULL;
	}
	else{
		p->begin_event_notify(SC_ZERO_TIME);
	}
}
void MRTKernel::dispatch(AbsRTTask* t)
{
	CPU* c=getOldProcessor(t);
	if(c!=NULL)
	{
		dispatch(c);
	}
}
void MRTKernel::onBeginDispatchMulti(CPU* p)
{
	AbsRTTask *dt  = _m_currExe[p];
	AbsRTTask *st  = NULL;

	if ( dt != NULL ) {
		_m_oldExe[dt] = p;
		_m_currExe[p] = NULL;
		_m_dispatched[dt] = NULL;
		dt->deschedule();
	}

	// select the first non dispatched task in the queue
	int i = 0;
	while ((st = _sched->getTaskN(i)) != NULL)
		if (_m_dispatched[st] == NULL) break;
		else i++;

	if (st == NULL) {
		return;
	}

	if (st) _m_dispatched[st] = p;
	p->_endEvt.setTask(st);
	_isContextSwitching[p] = true;
	sc_time overhead (_contextSwitchDelay);

	//in case of task migration
	if (st != NULL && _m_oldExe[st] != p && _m_oldExe[st] != NULL){
		overhead += _migrationDelay;
		migration++;
	}

	p->end_event_notify(overhead);
	start_context_switching[p] = sc_time_stamp();
	switch_overhead[p] = overhead;

}

void MRTKernel::onEndDispatchMulti(CPU* p)
{
	AbsRTTask *st = p->_endEvt.getTask();
	_m_currExe[p] = st;

	// st could be null (because of an idling processor)
	if (st) st->schedule();

	_isContextSwitching[p] = false;
	_sched->notify(st);
}

void MRTKernel::printState()
{
	sc_module *task;
	for (ITCPU i=_m_currExe.begin(); i!=_m_currExe.end(); i++) {
		task = dynamic_cast<sc_module *>(i->second);
		if (task != NULL) ;

	}
}

void MRTKernel::newRun()
{
	for (ITCPU i=_m_currExe.begin(); i!=_m_currExe.end(); i++) {
		if (i->second!=NULL)
			_sched->extract(i->second);
		i->second=NULL;
	}
	map<const AbsRTTask *, CPU *>::iterator j = _m_dispatched.begin();
	for ( ; j != _m_dispatched.end(); ++j )
		j->second = NULL;

	j = _m_oldExe.begin();
	for ( ; j != _m_oldExe.end(); ++j )
		j->second = NULL;
}

void MRTKernel::endRun()
{
	for (ITCPU i=_m_currExe.begin(); i!=_m_currExe.end(); i++) {
		if (i->second!=NULL)
			_sched->extract(i->second);
		i->second=NULL;
	}
}

void MRTKernel::print()
{
}

AbsRTTask* MRTKernel::getTask(CPU* c)
{
	return _m_currExe[c];
}

std::vector<std::string> MRTKernel::getRunningTasks()
{
	std::vector<std::string> tmp_ts;
	for (auto i = _m_currExe.begin(); i != _m_currExe.end(); i++)
	{
		std::string tmp_name = taskname((*i).second);
		if (tmp_name != "(nil)")
			tmp_ts.push_back(tmp_name);
	}
	return tmp_ts;
}

