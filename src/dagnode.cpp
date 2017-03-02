/***************************************************************************
Author: Jun Xiao
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <dagnode.hpp>
#include <task.hpp>
#include <taskevt.hpp>

DagNode::DagNode(sc_time iat, sc_time rdl, sc_time ph, sc_module_name name ,
		long qs) :
		PeriodicTask(iat, rdl, ph, name,qs) {
	job_index = 0;
}
void DagNode:: AddParentTaskOnly(DagNode* parent) {
	parents.push_back(parent);
}
void DagNode::AddChildrenTaskOnly(DagNode* child) {
	children.push_back(child);
}
void DagNode::AddChildTask(DagNode* child) {
	children.push_back(child);
	child->AddParentTaskOnly(this);
}
void DagNode::AddParentTask(DagNode* parent) {
	parents.push_back(parent);
	parent->AddChildrenTaskOnly(this);
}

bool DagNode::isSourceTask() {
	if (parents.empty())
		return true;
	else
		return false;
}
bool DagNode::isSinkTask() {
	if (children.empty())
		return true;
	else
		return false;
}
bool DagNode::check_parents_job() {
	for (auto iter = parents.begin(); iter != parents.end();
			iter++) {
		if ((*iter)->job_index <= job_index)
			return false;
	}
	return true;

}
void DagNode::activate_chilren(){
	for (auto iter = children.begin(); iter != children.end(); iter++)
		(*iter)->activate();
}

void DagNode::newRun(void) {
	if (!instrQueue.empty()) {
		actInstr = instrQueue.begin();
	} else
		throw EmptyTask();
	state = TSK_IDLE;
	task_exe = 0;
	lastArrival = arrival = phase;
	cout << "new run by" << this->name() << endl;
	if (int_time != NULL && isSourceTask())
		arrEvt.notify(arrival);
	_dl = sc_time(0, SC_PS);

}
//remeber to set it virtual
void DagNode::onEndInstance() {

	// from old Task ...
	deadEvt.cancel();
	// normal code
	cout << "task on end" << endl;
	if (not isActive()) {
		throw TaskNotActive("OnEnd() on a non-active task");
	}
	if (not isExecuting()) {
		throw TaskNotExecuting("OnEnd() on a non-executing task");
	}

	actInstr = instrQueue.begin();
	lastArrival = arrival;

	int cpu_index = getCPU()->getIndex();

	endEvt.setCPU(cpu_index);
	_kernel->onEnd(this);
	state = TSK_IDLE;
	task_exe = 0;
	//increase the job index
	job_index++;
	//activate children task
	activate_chilren();

	if (chkBuffArrival()) {
		fakeArrEvt.notify();
		cout << this->name() << " more buffer arrival " << sc_time_stamp()<< endl;
	}
}
void DagNode::activate() {
	//if sink, notify directly
	//check if all parents finishes their jobs
	if (isSourceTask()) {
		arrEvt.cancel();
		arrEvt.notify();
	} else {
		if(check_parents_job()){
			arrEvt.cancel();
			arrEvt.notify();
		}
	}
}

void DagNode::onArrival()
{
	if (!isActive()) {
		// Standard Task Arrival: do standard
		// book-keeping and forward the event to the
		// father
		handleArrival( sc_time_stamp());
		cout<<"calling on arrival of the kernel"<<endl;
		_kernel->onArrival(this);
	} else {
		deadEvt.notify();
		buffArrival();
	}
	if(isSourceTask())
		reactivate();
}
