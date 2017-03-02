/*
 * pfair_sheduler.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: Jun Xiao
 */

#ifndef PFAIR_SCHEDULER_HPP_
#define PFAIR_SCHEDULER_HPP_

#include "scheduler.hpp"
#include "rttask.hpp"

class pfair_model: public TaskModel,public sc_module
{
   protected:
	sc_time job_start_time;
	sc_time job_deadline;
	sc_time next_job_activation_time;
	sc_time next_job_deadline;
	sc_time model_quantum;
	float task_weight;
	float job_length;
	double period;

   public:
	float job_index=0;
	sc_event job_activation;
	sc_event model_insertion;
	bool round_scheduled=false;
	bool period_end=false;
	bool blocked=true;
	bool end=false;
   public:
	SC_HAS_PROCESS(pfair_model);
	pfair_model(AbsRTTask* t,sc_time quantum, sc_module_name name = sc_gen_unique_name("pfair_model")):TaskModel(t){
		model_quantum=quantum;
		PeriodicTask* pt= dynamic_cast<PeriodicTask *>(_rtTask);
		pt->compute_weight();
		task_weight=pt->get_weight();
		cout<<task_weight<<endl;
        period = dynamic_cast<PeriodicTask*>(_rtTask)->getPeriod().to_double();
        job_length=pt->getWCET().to_double()/model_quantum.to_double();
        cout<<"job length is: "<<job_length<<endl;
        compute_next_job_time();
        SC_METHOD(job_arrival);
        dont_initialize();
        sensitive << job_activation;
        SC_METHOD(model_insert);
        dont_initialize();
        sensitive << job_activation;
	}
	sc_time getPriority(){
		if(round_scheduled) return sc_time(0,SC_NS);
		else
		return next_job_deadline; }
	//void set_quantum(sc_time quantum){model_quantum=quantum; cout<<model_quantum<<endl;};
	void changePriority(sc_time p) {;}
	void setInserTime(sc_time t){ next_job_activation_time=t; }
	sc_time getInserTime() { return next_job_activation_time; }
	void compute_next_job_time();
	//void compute_first_job_time();
	void job_arrival();
	void model_insert();
	void prepare_next_job();
	bool quantum_expired();
	void set_round_schedule(bool bo){round_scheduled=bo;};

};

class pfair_scheduler: public Scheduler
{
public:
	sc_time schedule_quantum;
	sc_event quantum_finish;
	SC_HAS_PROCESS(pfair_scheduler);
	pfair_scheduler(sc_module_name name,sc_time quantum):Scheduler(name),schedule_quantum(quantum){
		SC_METHOD(schedule_next_quantum);
		dont_initialize();
		sensitive << quantum_finish;
	}
	void addTask(AbsRTTask* t);
	void addTask(AbsRTTask *task, const std::string &params){addTask(dynamic_cast<AbsRTTask *>(task));};
	void removeTask(AbsRTTask* t);
	void schedule_next_quantum();
	void notify(AbsRTTask* t);
	virtual void extract(AbsRTTask *) throw (RTSchedExc, BaseExc);

};




#endif /* PFAIR_SCHEDULER_HPP_ */
