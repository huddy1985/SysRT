/*
 *
 *
 *  Created on: Nov 25, 2015
 *      Author: Jun Xiao
 */
#include "partition_edf.hpp"
#include "task.hpp"

void partition_scheduler::addTask(AbsRTTask* t, const std::string &p) {
	Task* task = dynamic_cast<Task *>(t);
	schedulers[task->task_affinity]->addTask(t, p);
}

void partition_scheduler::addTask(AbsRTTask* t, int pri) {
	Task* task = dynamic_cast<Task *>(t);
	schedulers[task->task_affinity]->addTask(t, pri);

}

void partition_scheduler::insert(AbsRTTask *t) throw (RTSchedExc, BaseExc) {
	Task* task = dynamic_cast<Task *>(t);
	schedulers[task->task_affinity]->insert(t);
}
void partition_scheduler::extract(AbsRTTask *t) throw (RTSchedExc, BaseExc) {
	Task* task = dynamic_cast<Task *>(t);
	schedulers[task->task_affinity]->extract(t);

}
void partition_scheduler::notify(AbsRTTask *t) {
	Task* task = dynamic_cast<Task *>(t);
	schedulers[task->task_affinity]->notify(t);
}
AbsRTTask* partition_scheduler::getFirst(CPU *e) {
	return schedulers[e->getIndex()]->getFirst();
}

AbsRTTask* partition_scheduler::getTaskN(unsigned int n, CPU *e) {
	return schedulers[e->getIndex()]->getTaskN(n);
}

