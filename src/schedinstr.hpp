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
#ifndef __SCHEDINSTR_HPP__
#define __SCHEDINSTR_HPP__

#include <string>
#include <vector>
#include <factory.hpp>
#include <instr.hpp>
#include <taskevt.hpp>
#include <systemc.h>

using namespace std;

class Task;
class SchedInstr;

/**
     sc_event for threshold instr
 */
class SchedIEvt : public TaskEvt
{
protected:
	SchedInstr * ti;
public:
	SchedIEvt(Task* t, SchedInstr* in) :TaskEvt(t), ti(in)
{}
	SchedInstr *getInstr() {return ti;}
};
/**
  Simple classes which model instruction to set a preemption threshold
 */

class SchedInstr : public Instr {
	EndInstrEvt _endEvt;
	SchedIEvt _threEvt;
public:
/**
 //      This is the constructor of the SchedInstr.
 //      @param f is a pointer to the task containing the pseudo
 //      instruction
 //      @param r is the name of the resorce manager handling the
 //      resource which the task is accessing
 //      @param nr is the number of resources being taken
 //      @param n is the instruction name
*/
	SchedInstr(Task * f, const string& s,char *n = (char *)" ");
	static Instr* createInstance(vector<string> &par);
	///Virtual methods from Instr
	virtual void schedule();
	virtual void deschedule();
	virtual sc_time getExecTime() const { return sc_time::from_value(0);};
	virtual sc_time getDuration() const { return sc_time::from_value(0);};
	virtual sc_time getWCET() const throw(RandomVar::MaxException) { return sc_time::from_value(0); }
	virtual void reset() {}
	virtual void onEnd();
	virtual void newRun() {};
	virtual void endRun();
	/** Function inherited from clss Instr.It refreshes the state
	 *  of the executing instruction when a change of the CPU speed occurs.
	 */
	virtual void refreshExec(double, double){}

};

#endif
