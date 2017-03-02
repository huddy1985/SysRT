/*
 * ngedf.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: Jun Xiao
 */

#include <npedf.hpp>

void NGEDFScheduler::addTask(AbsRTTask* task) throw (RTSchedExc) {
	enqueueModel(new NGEDFTaskModel(task));
}

void NGEDFScheduler::addTask(AbsRTTask* task, const std::string &p) {
	if (!dynamic_cast<AbsRTTask *>(task))
		throw RTSchedExc(
				"Cannot add a AbsRTTask to EDF (should be AbsTask instead");
	// ignoring parameters
	addTask(dynamic_cast<AbsRTTask *>(task));
}

void NGEDFScheduler::notify(AbsRTTask *task) {
	_currExe = task;
	enableNonpreemt(task);
}

void NGEDFScheduler::enableNonpreemt(AbsRTTask* task) throw (RTSchedExc) {
	TaskModel* model = find(task);

	if (model == NULL)
		throw RTSchedExc("AbsRTTask not found");
	extract(task);
	dynamic_cast<NGEDFTaskModel *>(model)->set_nonpreemtive();
	insert(task);
	cout<<_queue.size()<<endl;

}

void NGEDFScheduler::extract(AbsRTTask* task) throw (RTSchedExc, BaseExc) {
	TaskModel* model = find(task);
	if (model != NULL) { // raise an exception
		_queue.erase(model);
		model->setInactive();
		dynamic_cast<NGEDFTaskModel *>(model)->set_preemtive();
	}
}
