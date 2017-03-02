/*
 * parti_mrtkernel.hpp
 *
 *  Created on: Dec 9, 2015
 *      Author: Jun Xiao
 */

#ifndef PARTI_MRTKERNEL_HPP_
#define PARTI_MRTKERNEL_HPP_

#include "mrtkernel.hpp"

class Parti_Mrtkernel: public MRTKernel{
public:

	SC_HAS_PROCESS(Parti_Mrtkernel);
	std::map<CPU*, bool> need_dispatch;
	sc_event dispatch_ev;
	map<CPU *, AbsRTTask *> _m_preExe;

	Parti_Mrtkernel(Scheduler*, absCPUFactory*, int n=1,
	        		sc_module_name name = "");

	Parti_Mrtkernel(Scheduler*, int n=1, sc_module_name name = "");
	Parti_Mrtkernel(Scheduler*, sc_module_name name);
	~Parti_Mrtkernel();
	//two dispatch function for partition scheduling algorithms
	void dispatch(AbsRTTask *task);
	void dispatch(CPU *cpu);
	virtual void dispatch();
	void dispatch_call();
	void dispatch_interface1(CPU *p);
	void dispatch_interface2(AbsRTTask *task);
	//dispatch handler
	void onBeginDispatchMulti(CPU* e);
	void onEndDispatchMulti(CPU* e);
	//task event
	virtual void onArrival(AbsRTTask *);
	virtual void suspend(AbsRTTask *);
	virtual void onEnd(AbsRTTask*);
	/**
	           Returns a pointer to the CPU on which t is runnig
	           (NULL if t is not running on any CPU). In this
	           case, it will always return the same CPU, since we
	           are on a single processor platform.
	        */
    virtual CPU* getProcessor(const AbsRTTask* t) const;

	        /**
	           Returns a pointer to the CPU on which t was runnig last.
	           (NULL if t has never run on any CPU). In this case, it will
	           always return the same CPU, since we are on a single
	           processor platform.
	        */
	virtual CPU* getOldProcessor(const AbsRTTask* t) const;
};



#endif /* PARTI_MRTKERNEL_HPP_ */
