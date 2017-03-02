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
#include <cstdlib>
#include <cstring>
#include <factory.hpp>
#include <strtoken.hpp>
#include <abskernel.hpp>
#include <instr.hpp>
#include <task.hpp>
#include <taskevt.hpp>
#include <systemc.h>

using namespace parse_util;
map<sc_module_name, Task *> Task::_index;
int Task::task_id = 0;
Task::~Task()
{
	//cout<<"total number of jobs: "<<num_jobs<<"\n";
	//cout<<"number of deadline miss: "<<deadline_miss<<"\n";
	//cout<<"task "<<_id<<"deadline miss ratio: "<< (double)deadline_miss/(double)num_jobs<<"\n";
	//dm<<"total number of jobs: "<<num_jobs<<"\n";
	//dm<<"number of deadline miss: "<<deadline_miss<<"\n";
	//dm<<"task "<<_id<<"deadline miss ratio: "<< (double)deadline_miss/(double)num_jobs<<"\n";
	discardInstrs(true);
	//et.close();
	rt.close();
	//at.close();
	//dm.close();
	//sc_close_vcd_trace_file(fp);
}

void Task::newRun(void)
{
	state = TSK_IDLE;
	task_exe=0;
	lastArrival = arrival = phase;
	if (int_time != NULL) arrEvt.notify(arrival);
	_dl = sc_time(0,SC_PS);

}

void Task::endRun(void)
{
	while (!arrQueue.empty()) {
		arrQueue.pop_front();
	}
	arrEvt.cancel();
	endEvt.cancel();
	schedEvt.cancel();
	deschedEvt.cancel();
	fakeArrEvt.cancel();
	deadEvt.cancel();
}

Task *Task::find(sc_module_name n)
{
	Task *res = 0;
	typedef map<sc_module_name, Task *>::iterator NI;
	NI i = _index.find(n);
	if (i != _index.end()) res = (*i).second;
	return res;
}

/* Methods from the interface... */
bool Task::isActive(void) const
{
	return state != TSK_IDLE;
}

bool Task::isExecuting(void) const
{
	return state == TSK_EXEC;
};

void Task::schedule(void)
{
	schedEvt.notify();
}

void Task::deschedule()
{
	schedEvt.cancel();
	deschedEvt.notify();
}

void Task::setKernel(AbsKernel *k)
{
	_kernel = k;
}


void Task::reactivate()
{
	sc_time v;

	if (int_time != NULL) {
		v = sc_time(int_time->get(),SC_PS);
		if (v.to_double() > 0) arrEvt.notify(v);
	}
}


void Task::handleArrival(sc_time arr)
{
	sc_time executed(0,SC_PS);
	sc_time computation_time(0,SC_PS);
	arrival = arr;
	execdTime = executed;
	actInstr = instrQueue.begin();

	// reset all instructions
	ConstInstrIterator p = instrQueue.begin();
	while (p != instrQueue.end()) {
		(*p)->reset();
		computation_time += (*p)->getDuration();
		p++;
	}
	state = TSK_READY;
	task_exe=0;
	_dl = getArrival() + _rdl;
	if (_dl >=  sc_time_stamp()) {
		deadEvt.notify(_dl);
	}
}

void Task::block()
{
	// check that the task is not idle and is not already blocked
	_kernel->suspend(this);
	state = TSK_BLOCKED;
	task_exe=0;
	_kernel->dispatch();
}

void Task::unblock()
{
	state = TSK_READY;
	task_exe=0;
	_kernel->onArrival(this);
}

sc_time Task::getArrival() const
{
	return arrival;
}

sc_time Task::getLastArrival() const
{
	return lastArrival;
}

sc_time Task::getExecTime() const
{
	if (isActive()) {
		return execdTime + (*actInstr)->getExecTime();
	} else {
		return execdTime;
	}
}

sc_time Task::getBuffArrival()
{
	sc_time time = arrQueue.front();

	arrQueue.pop_front();

	return time;
}

bool Task::chkBuffArrival() const
{
	return !arrQueue.empty();
}

void Task::buffArrival()
{
	if ((int)arrQueue.size() <= arrQueueSize) {
		arrQueue.push_back( sc_time_stamp());
	}
}

void Task::unbuffArrival()
{
	if (!arrQueue.empty()) {
		arrQueue.pop_back();
	}
}

RandomVar *Task::changeIAT(RandomVar *iat)
{
	RandomVar *ret = int_time;

	int_time = iat;
	return ret;
}

void Task::addInstr(Instr *instr)
{
	instrQueue.push_back(instr);
}

void Task::discardInstrs(bool selfDestruct)
{
	// Unset all the Instructions
	if (selfDestruct) {
		ConstInstrIterator p = instrQueue.begin();

		while (p != instrQueue.end()) {
			delete *p;
			p++;
		}
	}
	// delete all list entries
	instrQueue.clear();
}

/* And finally, the event handlers!!! */

void Task::onArrival()
{
	num_jobs++;
	if (!isActive()) {
		handleArrival( sc_time_stamp());
		_kernel->onArrival(this);

	} else {
		deadEvt.notify();
		buffArrival();
	}
	reactivate();
}

void Task::onEndInstance()
{
	// from old Task ...
	deadEvt.cancel();
	actInstr = instrQueue.begin();
	lastArrival = arrival;
	int cpu_index = getCPU()->getIndex();
	endEvt.setCPU(cpu_index);
	_kernel->onEnd(this);
	state = TSK_IDLE;
	task_exe=0;
	//et<<execdTime<<"\n";
	rt<<sc_time_stamp()-lastArrival<<"\n";
	if (chkBuffArrival()) {
		fakeArrEvt.notify();
		//cout<<this->name()<<" more buffer arrival "<<sc_time_stamp()<<endl;
	}
}

void Task::killInstance()
{
	endEvt.cancel();
	(*actInstr)->deschedule();
	execdTime += (*actInstr)->getExecTime();
	if (chkBuffArrival()) {
		fakeArrEvt.notify(SC_ZERO_TIME);
	}
	killEvt.notify();
}

void Task::onKill()
{
	killEvt.cancel();
	lastArrival = arrival;
	if(isExecuting())
		_kernel->onEnd(this);
	else
		_kernel->terminate(this);
	state = TSK_IDLE;
	task_exe=0;
	if (chkBuffArrival()) {
		fakeArrEvt.notify();
	}
}

void Task::onSched()
{
	int cpu_index = getCPU()->getIndex();
	schedEvt.setCPU(cpu_index);
	deschedEvt.cancel();
	state = TSK_EXEC;
	task_exe=1;
	_lastSched=sc_time_stamp();
	(*actInstr)->schedule();
	// from Task ...
	deadEvt.setCPU(cpu_index);
}

void Task::onDesched()
{
	int cpu_index = getOldCPU()->getIndex();
	deschedEvt.setCPU(cpu_index);
	endEvt.cancel();
	(*actInstr)->deschedule();
	state = TSK_READY;
	task_exe=0;
}

void Task::onInstrEnd()
{
	execdTime += (*actInstr)->getExecTime();
	actInstr++;
	if (actInstr == instrQueue.end()) {
		onEndInstance();
	} else if (isExecuting()) {
		(*actInstr)->schedule();
	}
}

void Task::onFakeArrival()
{
	handleArrival(getBuffArrival());
	_kernel->onArrival(this);
}

void Task::onDeadMiss()
{
	deadline_miss++;
	rt<<sc_time_stamp()-lastArrival<<" deadline miss"<<"\n";
	if (deadEvt._abort)
	{
		exit(-1);
	}
	if (deadEvt._kill && this->isActive() == true)
	{
		this->killInstance();
	}
}

void Task::activate()
{
	arrEvt.cancel();
	arrEvt.notify();
}

void Task::activate(sc_time t)
{
	arrEvt.cancel();
	arrEvt.notify(t);
}

sc_time Task::getWCET() const
{
	sc_time tt (0,SC_PS);
	if (_maxC.to_double() == 0) {
		ConstInstrIterator i = instrQueue.begin();
		while (i != instrQueue.end()) {
			tt += (*i)->getWCET();
			i++;
		}
	} else tt = _maxC;
	return tt;
}

void Task::insertCode(const string &code) //throw(ParseExc)
{
	vector<string> instr = split_instr(code);
	for (unsigned int i=0; i<instr.size(); ++i) {
		vector<string>::iterator j;
		string token = get_token(instr[i]);
		string param = get_param(instr[i]);
		vector<string> par_list = split_param(param);
		par_list.push_back(string(name()));
		auto_ptr<Instr> curr = genericFactory<Instr>::instance().create(token, par_list);
		//cout<<token<<"\n"<<param<<endl;
		Instr *curr_instr = curr.release();
		addInstr(curr_instr);
	}
}

void Task::printInstrList() const
{
}

CPU *Task::getCPU() const
{
	return _kernel->getProcessor(this);
}


CPU *Task::getOldCPU() const
{
	return _kernel->getOldProcessor(this);
}

void Task::refreshExec(double oldSpeed, double newSpeed)
{
	(*actInstr)->refreshExec(oldSpeed, newSpeed);
}

std::string taskname(const AbsRTTask *t)
{
	const sc_module *e = dynamic_cast<const sc_module *>(t);
	if (e) return string(e->name());
	else return "(nil)";
}


void Task::resetInstrQueue()
{
	actInstr = instrQueue.begin();
}


void Task::killOnMiss(bool kill)
{
	deadEvt.setKill(kill);
}

