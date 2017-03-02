/*
 * ngedf.hpp
 *
 *  Created on: Dec 15, 2015
 *      Author: Jun Xiao
 */

#ifndef NPEDF_HPP_
#define NPEDF_HPP_
#include <edfsched.hpp>

class NGEDFTaskModel: public EDFModel {
public:
	bool preemtive;

public:
	NGEDFTaskModel(AbsRTTask* t) :
		EDFModel(t), preemtive(false) {
	}
	sc_time getPriority() {
		if (preemtive) return _rtTask->getDeadline();
		else return sc_time::from_value(0);

	}
	void set_preemtive(){
		preemtive=true;
	}
	void set_nonpreemtive(){
		preemtive=false;
	}

};

class NGEDFScheduler: public EDFScheduler {
	void addTask(AbsRTTask* task) throw (RTSchedExc);
	void addTask(AbsRTTask* task, const std::string &p);
	virtual void notify(AbsRTTask *task);
	void extract(AbsRTTask* task) throw (RTSchedExc, BaseExc);
	void enableNonpreemt(AbsRTTask* task) throw (RTSchedExc);

};

#endif /* NPEDF_HPP_ */
