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

#ifndef __EXEINSTR_HPP__
#define __EXEINSTR_HPP__

#include <randomvar.hpp>
#include <systemc.h>
#include <instr.hpp>

using namespace std;

/**
      These instructions (ExecInstr & FixedInstr) are used to model a
      basic block of time-consuming computation, and are the basic
      building block of a real task
 */

class ExecInstr : public Instr {
protected:
	/// End of Instruction flag
	bool flag;
	/// Random var representing the instruction cost/duration
	auto_ptr<RandomVar> cost;
	/// Actual Real-Time execution of the instruction
	sc_time execdTime;
	/// Duration of the current instruction
	sc_time currentCost;
	/// Execution time spent by the instruction
	sc_time actTime;
	/// Last instant of time this instruction was scheduled
	sc_time lastTime;
	/// True if the instruction is currently executing
	bool executing;
public:

	EndInstrEvt _endEvt;
	/** This is the constructor of the ExecInstr
	@param f is a pointer to the task containing the pseudo intruction
	@param c is a random variable containing the duration of the instruction
	@param n is the instruction name
	 */
	SC_HAS_PROCESS(ExecInstr);
	ExecInstr(Task *f, RandomVar *c, sc_module_name n = sc_gen_unique_name("Insturctions")) :
		Instr(f, n), cost(c), _endEvt(this)
	{
		newRun();
		SC_METHOD(onEnd);
		dont_initialize();
		sensitive << _endEvt;
	}

	ExecInstr(Task *f, auto_ptr<RandomVar> &c, sc_module_name n = sc_gen_unique_name("Insturctions")) :
		Instr(f, n), cost(c), _endEvt(this)
	{
		newRun();
		SC_METHOD(onEnd);
		dont_initialize();
		sensitive << _endEvt;
	}
	static Instr *createInstance(vector<string> &par);
	virtual ~ExecInstr() {}
	//Virtual methods from Instr
	virtual void schedule();
	virtual void deschedule();
	virtual void onEnd();
	virtual void reset();
	virtual sc_time getDuration() const;
	virtual sc_time getWCET() const throw(RandomVar::MaxException);
	virtual sc_time getExecTime() const;
	//From Entity...
	virtual void newRun();
	virtual void endRun();


	/** Function inherited from Instr. It refreshes the state of the
	 *  executing instruction when a change of the CPU speed occurs.
	 */
	void refreshExec(double oldSpeed, double newSpeed);

};

/**
     This is defined for user's commodity. Actually it is an ExecInstr
     having a fixed duration.
 */
class FixedInstr : public ExecInstr {
public:
	FixedInstr(Task *t, sc_time duration, sc_module_name n = sc_gen_unique_name("Insturctions")) :
		ExecInstr(t, new DeltaVar(duration.to_double()), n)
{}

	static Instr *createInstance(vector<string> &par);
};

#endif
