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
#ifndef __SUSPEND_INSTR__
#define __SUSPEND_INSTR__

#include <vector>
#include <string>
#include <randomvar.hpp>
#include <instr.hpp>
#include <systemc.h>
class SuspendInstr : public Instr {
	sc_event suspEvt;
	sc_event resumeEvt;
	sc_time delay;

public:
	SuspendInstr(Task *f, sc_time delay);
	static SuspendInstr * createInstance(std::vector<std::string> &par);

	virtual void schedule();
	virtual void deschedule();
	virtual sc_time getExecTime() const { return sc_time::from_value(0);};
	virtual sc_time getDuration() const { return sc_time::from_value(0);};
	virtual sc_time getWCET() const throw(RandomVar::MaxException) { return sc_time::from_value(0); }
	virtual void reset() {}
	void onSuspend(sc_event *evt);
	void onEnd(sc_event *evt);
	void newRun();
	void endRun();
	/** Function inherited from clss Instr.It refreshes the state 
	 *  of the executing instruction when a change of the CPU speed occurs. 
	 */ 
	virtual void refreshExec(double, double){}
};
#endif 
