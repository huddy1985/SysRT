/*
 * pfair_scheduler.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: Jun Xiao
 */


#include "pfair_scheduler.hpp"
#include "kernel.hpp"
#include "rttask.hpp"
#include <math.h>
#include <plist.hpp>

bool pfair_model::quantum_expired()
   {
       Task* t = dynamic_cast<Task*>(_rtTask);
       sc_time last = t->getLastSched();
       if ((sc_time_stamp() - last) >= model_quantum)
           return true;
       return false;
   }


void pfair_model::compute_next_job_time(){
	 next_job_activation_time= sc_time(floor(job_index/task_weight)*model_quantum.to_double(),SC_NS);
	 next_job_deadline=sc_time(ceil((job_index+1)/task_weight)*model_quantum.to_double(),SC_NS);
	 //cout<<"model quantum: "<<model_quantum.to_double()<<endl;
	 cout<<"Task:NO"<<_rtTask->getTaskNumber()<<"next job start time"<<next_job_activation_time<<"deadline: "<<next_job_deadline<<endl;
	 job_index+=1;
	 cout<<"job index is: "<<job_index<<endl;
	 if( (int)(job_index) % ((int)(job_length))==0){
		period_end=true;
		//job_index+=1;
	 }
	 else period_end=false;


}
//delta cycle,wait unitil the end of the quantum
void pfair_model::job_arrival(){
	model_insertion.notify(SC_ZERO_TIME);
}
void pfair_model::model_insert(){
	PeriodicTask* pt= dynamic_cast<PeriodicTask *>(_rtTask);
	pt->unblock();

}
void pfair_model::prepare_next_job(){
	compute_next_job_time();

	if ((next_job_activation_time.to_double()-sc_time_stamp().to_double())>0){
	   job_activation.notify(next_job_activation_time-sc_time_stamp());
	   cout<<"Task:NO"<<_rtTask->getTaskNumber()<<"next job will be activated after "<<(next_job_activation_time-sc_time_stamp()) <<endl;
	}
	else{
		model_insert();
		cout<<"Task:NO"<<_rtTask->getTaskNumber()<<"next job will be activated after 0" <<endl;
	}
}


void pfair_scheduler::addTask(AbsRTTask* t) {
	 pfair_model *model = new pfair_model(t,schedule_quantum);
	        _tasks[t] = model;

}

void pfair_scheduler::removeTask(AbsRTTask* t) {
	;
}



void pfair_scheduler::notify(AbsRTTask* task)
{
    quantum_finish.cancel();
    cout<<"task NO."<<task->getTaskNumber()<<"start quantum at time: "<<sc_time_stamp()<<endl;
    if (task != NULL) {
        extract(task);
        pfair_model* model = dynamic_cast<pfair_model *>(find(task));
        model->round_scheduled=true;
        insert(task);
        quantum_finish.notify(schedule_quantum);
    }
}

void pfair_scheduler::schedule_next_quantum()
{
	int i=0;
	int size = _queue.size();
	priority_list<TaskModel*, TaskModel::TaskModelCmp>::iterator it =
				_queue.begin();
	while(i<size){
        pfair_model* model = dynamic_cast<pfair_model *>(*it);
        if(_queue.size()>1)it++;
        i++;
        if(it==_queue.end()) cout<<"end of queue"<<endl;

        if (model!=0 && model->round_scheduled) {
        	model->round_scheduled=false;
        	Task* pt= dynamic_cast<Task *> (model->getTask());
        	if(!model->period_end){
        	pt->block();
        	size--;
        	i--;
            model->prepare_next_job();
        	}
        	else{
        		model->end=true;
        	}
        }
	}

}
void pfair_scheduler::extract(AbsRTTask* t) throw (RTSchedExc, BaseExc){
	TaskModel* model = find(t);
		if (model != NULL) {// raise an exception
		_queue.erase(model);
		model->setInactive();
		cout<<_queue.size()<<endl;
		pfair_model* pfairmodel=dynamic_cast <pfair_model*> (model);
			if(pfairmodel->round_scheduled||pfairmodel->end){
				pfairmodel->round_scheduled=false;
				pfairmodel->compute_next_job_time();
				pfairmodel->end=false;
			}
		}

}
