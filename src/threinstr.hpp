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
#ifndef __THREINSTR_HPP__
#define __THREINSTR_HPP__

#include <string>
#include <vector>
#include <factory.hpp>
#include <instr.hpp>
#include <taskevt.hpp>
#include <systemc.h>

using namespace std;
class Task;
class ThreInstr;

/**
     event for threshold instr
 */
class ThreEvt : public TaskEvt
{
protected:
	ThreInstr * ti;
public:
	ThreEvt(Task* t, ThreInstr* in) :TaskEvt(t), ti(in)
{}
	ThreInstr *getInstr() {return ti;}
};
/**
      To model instruction to set a preemption threshold
 */

class ThreInstr : public Instr {
	EndInstrEvt _endEvt;
	ThreEvt _threEvt;
	string _th;
public:
	/**
 //      This is the constructor of the ThreInstr.
 //      @param f is a pointer to the task containing the pseudo
 //      instruction
 //      @param r is the name of the resorce manager handling the
 //      resource which the task is accessing
 //      @param nr is the number of resources being taken
 //      @param n is the instruction name
	 */
	ThreInstr(Task * f, const string& th, char *n = (char *) " ");
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
