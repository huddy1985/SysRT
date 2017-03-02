/*
 *
 *
 *  Created on: Nov 25, 2015
 *      Author: Jun Xiao
 */
#include "edfsched.hpp"


class partition_scheduler: public Scheduler{
public:
	Scheduler **schedulers;
	partition_scheduler(int cpu_num){
		schedulers = new Scheduler *[cpu_num];
	}
public:
	// set the scheduler for the i_th core
    void set_scheduller(int i, Scheduler *schedu) {schedulers[i] = schedu;};
    Scheduler* get_scheduler(int i) {return schedulers[i];};
    virtual void addTask(AbsRTTask* task, const std::string &p);
    virtual void addTask(AbsRTTask* task, int pri);
    virtual void removeTask(AbsRTTask* task) {};
    virtual void insert(AbsRTTask *) throw(RTSchedExc, BaseExc);
    virtual void extract(AbsRTTask *) throw(RTSchedExc, BaseExc);
    virtual void notify(AbsRTTask *);
    virtual AbsRTTask *getFirst(CPU*);
    virtual AbsRTTask * getTaskN(unsigned int, CPU*);
};
