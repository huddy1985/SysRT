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
#ifndef __WAITINSTR_HPP__
#define __WAITINSTR_HPP__
#include <string>
#include <vector>
#include <factory.hpp>
#include <instr.hpp>
#include <taskevt.hpp>
#include <systemc.h>

using namespace std;

class Task;
class WaitInstr;
class SignalInstr;

/**
     event for wait instr
 */
class WaitEvt : public TaskEvt
{
protected:
	WaitInstr * wi;
public:
	WaitEvt(Task* t, WaitInstr* in) :TaskEvt(t), wi(in)
{}
	WaitInstr *getInstr() {return wi;}
};

/**
     event for signal instr
 */
class SignalEvt : public TaskEvt
{
protected:
	SignalInstr *si;
public:
	SignalEvt(Task* t, SignalInstr* in) :TaskEvt(t), si(in) {}
	SignalInstr *getInstr() {return si;}
};

/**
      Simple classes which model wait and signal instruction to use a resource 
      @author Fabio Rossi and Giuseppe Lipari
      @see Instr 
 */

class WaitInstr : public Instr {
	string _res;
	EndInstrEvt _endEvt;
	WaitEvt _waitEvt;
	int _numberOfRes;
public:
	/**
       This is the constructor of the WaitInstr.
       @param f is a pointer to the task containing the pseudo
       instruction
       @param r is the name of the resorce manager handling the
       resource which the task is accessing
       @param nr is the number of resources being taken
       @param n is the instruction name
	 */
	SC_HAS_PROCESS(WaitInstr);
	WaitInstr(Task * f, const char *r, int nr=1, sc_module_name n = sc_gen_unique_name("Insturctions"))
	: Instr(f, n), _res(r), _endEvt(this),
	  _waitEvt(f, this), _numberOfRes(nr)
	{
		SC_METHOD(onEnd);
		dont_initialize();
		sensitive << _endEvt;
	}

	WaitInstr(Task * f, const string &r, int nr=1, sc_module_name n = sc_gen_unique_name("Insturctions"))
	: Instr(f, n), _res(r), _endEvt(this),
	  _waitEvt(f, this), _numberOfRes(nr)
	{
		SC_METHOD(onEnd);
		dont_initialize();
		sensitive << _endEvt;
	}

	static Instr* createInstance(vector<string> &par);

	///Virtual methods from Instr
	virtual void schedule();
	virtual void deschedule();
	virtual sc_time getExecTime() const { return sc_time::from_value(0);};
	virtual sc_time getDuration() const { return sc_time::from_value(0);};
	virtual sc_time getWCET() const throw(RandomVar::MaxException) { return sc_time::from_value(0); }
	string getResource() const { return _res; };
	virtual void reset() {}
	virtual void onEnd();
	virtual void newRun() {};
	virtual void endRun();
	/** Function inherited from clss Instr.It refreshes the state
	 *  of the executing instruction when a change of the CPU speed occurs.
	 */
	virtual void refreshExec(double, double){}

};

/**
     Simple class which models signal instruction to use a resource. 
     @author Fabio Rossi and Giuseppe Lipari
     @see Instr 
 */

class SignalInstr : public Instr {
	string _res;
	EndInstrEvt _endEvt;
	SignalEvt _signalEvt;
	int _numberOfRes;
public:
	/**
       This is the constructor of the SignalInstr
       @param f is a pointer to the task containing the pseudo
       instruction
       @param r is the name of the resource which the task has
       accessed
       @param nr is the number of resources being taken
       @param n is the instruction name
	 */
	SC_HAS_PROCESS(WaitInstr);
	SignalInstr(Task *f,  const char *r, int nr=1, sc_module_name n = sc_gen_unique_name("Insturctions"))
	: Instr(f, n), _res(r), _endEvt(this),
	  _signalEvt(f, this), _numberOfRes(nr)
	{
		SC_METHOD(onEnd);
		dont_initialize();
		sensitive << _endEvt;
	}

	SignalInstr(Task *f, const string &r, int nr=1, sc_module_name n = sc_gen_unique_name("Insturctions"))
	: Instr(f, n), _res(r), _endEvt(this),
	  _signalEvt(f, this), _numberOfRes(nr)
	{
		SC_METHOD(onEnd);
		dont_initialize();
		sensitive << _endEvt;
	}
	static Instr* createInstance(vector<string> &par);
	///Virtual methods from Instr
	virtual void schedule();
	virtual void deschedule();
	virtual sc_time getExecTime() const { return sc_time::from_value(0);};
	virtual sc_time getDuration() const { return sc_time::from_value(0);};
	virtual sc_time getWCET() const throw(RandomVar::MaxException) {return sc_time::from_value(0);}
	virtual void reset() {}
	//virtual void setTrace(Trace *);
	string getResource() const { return _res; };
	virtual void onEnd();
	virtual void newRun() {};
	virtual void endRun();
	/** Function inherited from clss Instr.It refreshes the state
	 *  of the executing instruction when a change of the CPU speed occurs.
	 */
	virtual void refreshExec(double, double) {}

};

#endif
