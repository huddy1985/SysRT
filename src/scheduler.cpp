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
#include <scheduler.hpp>
#include <task.hpp>
#include <climits>
#include <pfair_scheduler.hpp>

using namespace std;

TaskModel::TaskModel(AbsRTTask* t) :
		_rtTask(t), active(false), _insertTime(sc_time::from_value(0)), _threshold(
				INT_MAX) {
}

TaskModel::~TaskModel() {
}

bool TaskModel::TaskModelCmp::operator()(TaskModel* a, TaskModel* b) {
	if (a->getPriority() < b->getPriority())
		return true;
	else if (a->getPriority() == b->getPriority()) {
		if (a->getInsertTime() < b->getInsertTime())
			return true;
		else if (a->getInsertTime() == b->getInsertTime())
			if (a->getTaskNumber() < b->getTaskNumber())
				return true;
	}
	return false;
}

void TaskModel::setActive() {
	active = true;
}

void TaskModel::setInactive() {
	active = false;
}



bool TaskModel::isActive() {
	return active;
}

/*-----------------------------------------------------------------*/

void Scheduler::enqueueModel(TaskModel* model) {
	AbsRTTask* task = model->getTask();

	_tasks[task] = model;
}

TaskModel* Scheduler::find(AbsRTTask* task) {
	map<AbsRTTask*, TaskModel*>::iterator mi = _tasks.find(task);

	if (mi == _tasks.end())
		return NULL;
	else
		return (*mi).second;
}

void Scheduler::setKernel(AbsKernel* k) {
	_kernel = k;
}

void Scheduler::insert(AbsRTTask* task) {
	//DBGENTER("Scheduler");

	TaskModel* model = find(task);

	model->setInsertTime(sc_time_stamp());
	model->setActive();
	_queue.insert(model);

}

void Scheduler::extract(AbsRTTask* task) {
	TaskModel* model = find(task);

	if (model != NULL) {
	_queue.erase(model);
	model->setInactive();
	}
}

int Scheduler::getPriority(AbsRTTask* task)  {
	TaskModel* model = find(task);
	return model->getPriority().value();
}

void Scheduler::changePriority(AbsRTTask* task, const std::string &params)
		{

}

int Scheduler::getThreshold(AbsRTTask* task)  {
	TaskModel* model = find(task);
	return model->getThreshold();
}

void Scheduler::setThreshold(AbsRTTask *task, const int th) {
	TaskModel* model = find(task);
	model->setThreshold(th);
}

int Scheduler::enableThreshold(AbsRTTask* task) {
	TaskModel* model = find(task);
	oldPriorities[task] = model->getPriority().value();
	extract(task);
	int tmp = model->getThreshold();
	model->changePriority(sc_time::from_value(tmp));
	insert(task);
	return tmp;
}

void Scheduler::disableThreshold(AbsRTTask *task) {
	TaskModel* model = find(task);
	extract(task);
	model->changePriority(model->getPriority());
	insert(task);
	_kernel->dispatch();
}

void Scheduler::discardTasks(bool f) {
	typedef map<AbsRTTask*, TaskModel*>::iterator IT;
	_queue.clear();
	IT i = _tasks.begin();
	if (f) {
		while (i != _tasks.end()) {
			delete ((*i).second);
			i++;
		}
	}
	_tasks.clear();
}

AbsRTTask* Scheduler::getTaskN(unsigned int n) {
	if (_queue.size() <= n) {
		return NULL;
	}
	priority_list<TaskModel*, TaskModel::TaskModelCmp>::iterator it =
			_queue.begin();
	for (unsigned int i = 0; i < n; i++)
		it++;
	return (*it)->getTask();
}

AbsRTTask* Scheduler::getTaskN(unsigned int n, CPU *e) {
	return getTaskN(n);
}

void Scheduler::notify(AbsRTTask* task) {
	_currExe = task;

}

void Scheduler::newRun() {
	_queue.clear();

	typedef map<AbsRTTask*, TaskModel*>::iterator IT;

	for (IT i = _tasks.begin(); i != _tasks.end(); ++i)
		i->second->setInactive();
}

void Scheduler::endRun() {
}

void Scheduler::print() {
	priority_list<TaskModel*, TaskModel::TaskModelCmp>::iterator it =
			_queue.begin();
}

AbsRTTask* Scheduler::getFirst() {
	return getTaskN(0);

}
AbsRTTask* Scheduler::getFirst(CPU *e) {
	return getTaskN(0);

}
