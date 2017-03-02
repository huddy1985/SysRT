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
#include <rrsched.hpp>
#include <task.hpp>
#include <cassert>
#include <sstream>

using namespace std;

bool RRScheduler::RRModel::isRoundExpired()
{
	Task* t = dynamic_cast<Task*>(_rtTask);

	assert(t != NULL);

	sc_time last = t->getLastSched();
	if (_rrSlice.value() > 0 && (sc_time_stamp() - last) >= _rrSlice)
		return true;
	return false;
}

sc_time RRScheduler::RRModel::getPriority()
{
	return sc_time::from_value(1);
}

void RRScheduler::RRModel::changePriority(sc_time p)
{
	throw RRSchedExc("Cannot change priority in RRSched!");
}



void RRScheduler::setRRSlice(AbsRTTask* task, sc_time slice)
{
	RRModel* model = dynamic_cast<RRModel *>(find(task));
	cout<<"setRRslice"<<endl;
	if (model == 0) throw RRSchedExc("Cannot find task");
	model->setRRSlice(slice);
}

void RRScheduler::notify(AbsRTTask* task)
{
	_rrEvt.cancel();
	if (task != NULL) {
		RRModel* model = dynamic_cast<RRModel *>(find(task));
		//cout<<"notify"<<endl;
		if (model == 0) throw RRSchedExc("Cannot find task");
		if (model->getRRSlice().value() > 0) {
			_rrEvt.notify(model->getRRSlice());
			cout<<"rrEvt notify at time "<<sc_time_stamp()<<" "<< model->getRRSlice() <<endl;
		}
	}


}

void RRScheduler::round()
{
	RRModel* model = dynamic_cast<RRModel *>(_queue.front());

	if (model!=0 && model->isRoundExpired()) {
		_queue.erase(model);
		if (model->isActive()) {
			model->setInsertTime(sc_time_stamp());
			_queue.insert(model);
		}
	}
	//         if it is not active... ?
	RRModel* first = dynamic_cast<RRModel *>(_queue.front());
	if (first != 0) {
		sc_time slice = first->getRRSlice();
		if (slice.value() == 0) _rrEvt.cancel();
		else {
			if (first != model || first->isRoundExpired()) {
				_rrEvt.cancel();
				_rrEvt.notify(slice);
				cout<<"rrEvt notified agained"<<endl;
			}
		}
	}
	if (_kernel) {
		_kernel->dispatch();
	}

}

void RRScheduler::addTask(AbsRTTask *task) throw(RRSchedExc)
{
	RRModel *model = new RRModel(task);
	if (find(task) != NULL)
		throw RRSchedExc("Element already present");
	_tasks[task] = model;
	model->setRRSlice(sc_time::from_value(defaultSlice));
}

void RRScheduler::addTask(AbsRTTask* task, const std::string &p)
{
	AbsRTTask *mytask = dynamic_cast<AbsRTTask *>(task);
	if (mytask == 0)
		throw RRSchedExc("Cannot add a AbsRTTask to RR");
	RRModel *model = new RRModel(mytask);
	if (find(task) != NULL)
		throw RRSchedExc("Element already present");
	_tasks[task] = model;
	int slice = 0;
	if (p == "") slice = defaultSlice;
	else {
		stringstream ss(p);
		ss >> slice;
	}
	cout<<slice<<endl;
	model->setRRSlice(sc_time::from_value(slice));

}
