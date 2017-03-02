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
#ifndef __TASK_HPP__
#define __TASK_HPP__

#include <randomvar.hpp>
#include <strtoken.hpp>
#include <systemc.h>
#include <abstask.hpp>
#include <abskernel.hpp>
#include <taskevt.hpp>
#include <taskexc.hpp>
#include <deque>
#include <cpu.hpp>
#include <map>
#include <fstream>
#include <iostream>

#define _TASK_DBG_LEV "Task"

/* Forward declaration... */
class Instr;
class InstrExc;

// Task states
typedef enum { TSK_IDLE, TSK_READY, TSK_EXEC, TSK_BLOCKED } task_state;

/**

        This class models a cyclic task. A cyclic task is a task that is
        cyclically activated by a timer (for example a periodic task) or
        by an external event (sporadic or aperiodic task). 

        This class models a "run-to-completion" semantic.  Every
        activation (also called arrival), an instance of the task is
        executed. The task executes all the instructions in the
        sequence until the last one, and then the instance is
        completed (task end). At the next activation, the task starts
        executing a new instance, and the instruction pointer is reset
        to the beginning of the sequence.

        Also:

        - when a job arrives, the corresponding deadline is set
        (the class Task has no deadline parameter).

        - it adds a deadline event to check deadline misses; it can
        abort the simulation in case of deadline miss (depending on
        the abort parameter in the constructor).

 */
class Task : public sc_module, public AbsRTTask {

	//private:
	// Hide copy constructor and assignment, tasks cannot be copied
	//Task(const Task &);
	//Task & operator=(const Task &);

public:
	static int task_id;//task id
	int _id;
	int deadline_miss;
	int num_jobs;
	RandomVar *int_time;  // The task is owner of this varible
	sc_time lastArrival;     // The arrival of the last instance!
	sc_time phase;          // Initial phasing for first arrival
	sc_time arrival;         // Arrival time of the current (last) instance
	sc_time execdTime;       // Actual Real-Time execution of the task
	sc_time _maxC;           // Maximum computation time
	std::deque<sc_time> arrQueue; // Arrival queue, sorted FIFO
	int arrQueueSize;      // -1 stands for no-limit
	task_state state;      // IDLE, READY, EXECUTING, BLOCKED 
	typedef std::vector<Instr *> InstrList;
	typedef std::vector<Instr *>::iterator InstrIterator;
	typedef std::vector<Instr *>::const_iterator ConstInstrIterator;
	InstrList instrQueue;
	InstrIterator actInstr;
	AbsKernel *_kernel;
	sc_time _lastSched;
	sc_time _dl;
	sc_time _rdl;
	//to record task response time and execution time
	std::fstream rt;//,et,at,dm;
public:
	bool task_exe=0;
	uint16_t task_affinity=0xFFFF;
	//AbstractFeedbackModule *feedback;
	//sc_object *p = sc_find_object("sd");
public:
	// Events need to be public to avoid an excessive fat interface.
	// Rhis is especially true when considering the probing mechanism
	// (for statistical collection and tracing).
	static std::map<sc_module_name, Task *> _index;
	ArrEvt arrEvt;
	EndEvt endEvt;
	SchedEvt schedEvt;
	DeschedEvt deschedEvt;
	FakeArrEvt fakeArrEvt;
	KillEvt killEvt;
	DeadEvt deadEvt;
	/******************************************************************/

public:

	/** Constructor.

            @param iat Random variable that models activation events. 
            If this parameter is set equal to NULL (or 0), 
            then the task is NOT cyclically activated. In other 
            words, there is no arrival event automatically posted, 
            but the task must be activated by another entity 
            (task or interrupt, or else).
            @param ph  initial time of the activation. It is the first time at which
            the arrival event is posted. This parameter is ignored when 
            iat = 0. 
            @param name Unique name for this entity. 

            @param qs Maximum size of the arrival buffer. This is the maximum 
            number of arrivals that can be buffered. By default, this is
            equal to __LONG_MAX__. If equal to zero, an arrival event is 
            discarded when the task is already active.

            @param maxC Worst-case execution time of an instance. This 
            parameter is not used for the simulation itself, but only 
            for some algorithm, or for analysis). See getWCET(). */
	SC_HAS_PROCESS(Task);

	Task(RandomVar *iat, sc_time rdl, sc_time ph,sc_time maxC,
			sc_module_name name = "",
			long qs = 1000)
	: sc_module(name),_id(task_id++),deadline_miss(0),num_jobs(0),
	  int_time(iat), lastArrival(sc_time(0,SC_PS)), phase(ph),
	  arrival(sc_time(0,SC_PS)), execdTime(sc_time(0,SC_PS)), _maxC(maxC),
	  arrQueue(), arrQueueSize(qs),
	  state(TSK_IDLE),
	  instrQueue(),
	  actInstr(),
	  _kernel(NULL),
	  _lastSched(sc_time(0,SC_PS)),
	  _dl(sc_time(0,SC_PS)), _rdl(rdl),
	  //feedback(NULL),
	  arrEvt(this), endEvt(this), schedEvt(this),
	  deschedEvt(this), fakeArrEvt(this), killEvt(this),
	  deadEvt(this)
	{
		//et.open("/home/jun/workspace/rtosonlyos/Debug/Results/execution_time_"+to_string(_id)+".txt", fstream::in | fstream::out | fstream::app);
		rt.open("/home/jun/workspace/rtosonlyos/Debug/Results/response_time_"+to_string(_id)+".txt", fstream::in | fstream::out | fstream::app);
		//at.open("/home/jun/workspace/rtosonlyos/Debug/Results/arrival_time_"+to_string(_id)+".txt", fstream::in | fstream::out | fstream::app);
		//dm.open("/home/jun/workspace/rtosonlyos/Debug/Results/deadmiss_ratio_"+to_string(_id)+".txt", fstream::in | fstream::out | fstream::app);
		_index[name] = this;
		//std::fstream fs;
		//string filename ="text_";
		//name += std::to_string(s);
		//name += ".txt";
		//fp=sc_create_vcd_trace_file("task_exe");
		//sc_trace(fp,task_exe,"Task.task_exe");
		SC_METHOD(newRun);
		SC_METHOD(onArrival);
		dont_initialize();
		sensitive << arrEvt;
		SC_METHOD(onEndInstance);
		dont_initialize();
		sensitive << endEvt;
		SC_METHOD(onKill);
		dont_initialize();
		sensitive << killEvt;
		SC_METHOD(onSched);
		dont_initialize();
		sensitive << schedEvt;
		SC_METHOD(onDesched);
		dont_initialize();
		sensitive << deschedEvt;
		SC_METHOD(onFakeArrival);
		dont_initialize();
		sensitive << fakeArrEvt;
		SC_METHOD(onDeadMiss);
		dont_initialize();
		sensitive << deadEvt;
	}
	static Task * find(sc_module_name n);
	/**
           This event handler is invoked every time an arrival event
           is triggered.
	 */
	virtual void onArrival();

	/**
           This event handler is invoked when a task completes an instance.
           It resets the executed time counter, and the instruction counter.

           @todo change its name into onInstanceEnd().
	 */
	virtual void onEndInstance();

	/**
	   This event handler is invoked when a task instance has been killed.
	   Similar to onEndInstance, but the endEvt is not processed.
	 */
	void onKill();

	/**
           This event handler is invoked everytime the task is scheduled (i.e.
           dispatched by the kernel).
	 */
	void onSched();

	/**
           This event handler is invoked everytime the task is suspended
           by the kernel or by another entity, or it suspends itself.
	 */
	void onDesched();

	/**
           This event handler is ivoked everytime a buffered arrival has
           to be processed. In this case:
           - an arrival event was triggered while the task was still active
           and was buffered
           - the task has just completed an instance
           - the buffered arrival event is processed

           It is used maily for tracing reason (there is not explicit need
           for an event here).
	 */
	void onFakeArrival();
	/**
                 This event handler is for ondead event
	 */
	//void onDeadevt(DeadEvt *e);
	void onDeadMiss();
	/**
           Virtual destructor. Does nothing.

           @todo should it destroy the instructions? probably the answer is YES.
	 */
	virtual ~Task();

	/**
           For the abstract factory
	 */
	//static Task* createInstance(vector<string> &par);

	/**
           Initializes the internal task structures at the beginning of each run.
           For example:
           - posts the first arrival event
           - resets the instruction counter
           - resets the executed time counter
	 */
	virtual void newRun();

	/**
           Does nothing.
	 */
	virtual void endRun();

	/**
            This functions activates the tasks (post the arrival event at
            the current time).
	 */
	virtual void activate();

	/** This method is used to activate the task (posts the arrival
            event at the time specified in the parameter) 

            @param t time of the arrival event

            @todo what if the task is active? 
            what if the time is in the past?
            what if the arrival event is already posted?
	 */
	virtual void activate(sc_time t);

	/**
            This method permits to kill a task instance that is currently
            executing. The instruction pointer is reset to the first
            instruction, and the executed time counter is reset to 0. No
            further event is posted (in particular, the TaskEndEvt is not
            posted).

            @throws TaskNotActive if the task is not currently active
            @throws TaskNotExecuting if the task is not executing

            @todo the last throws is quite suspect....
	 */
	void killInstance() ;


	/**
            This method permits to select the behaviour of the task when a 
            deadline miss occurs.

            @param kill = true, to kill the task when a deadline miss occurs
            @param kill = false, to contine the task when a deadline miss occurs
	 */
	void killOnMiss(bool kill);

	/**
            Event propagated by instructions. It is invoked when an
            instruction is completed. The default behavior is to increment
            the instruction pointer to the next instrution in the list. If
            there are no more instrutions, the TaskEndEvt is posted at the
            current time.
	 */
	virtual void onInstrEnd();


	void block();
	void rrblock();
	void unblock();


	/**
            Specify that this task has to be traced 
	 */
	//virtual void setTrace(Trace *t);

	/**
            Adds a new instruction at the end of the instruction list. This
            method has to be invoked during initialization to specify the
            list of instructions for a task.

            The instructions created and added in this way must be
            destroyed by the Task destructor. Therefore, the owner of the
            object pointed by instr is the task.
	 */
	void addInstr(Instr *instr);

	/**
            Removes an instruction from the instruction list. This method
            is invoked mainly during destruction of the task.
	 */

	/**
            Removes all instructions. This method removes all instructions
            from the instruction list. 

            @param selfDestruct when this is true (by default is false),
            the insctructions are destroyed (deallocated). This method is
            invoked during the destruction of the task.
	 */
	void discardInstrs(bool selfDestruct = true);

	/** Returns the arrival time of the current instance */
	sc_time getArrival() const;

	/** Returns the arrival time of the previous instance */
	sc_time getLastArrival() const;
	/** Returns the executed time of the last (or current) instance */
	sc_time getExecTime() const;
	sc_time getMinIAT() const { return sc_time(int_time->getMinimum(),SC_PS);}
	virtual sc_time getLastSched() {return _lastSched;}
	/**
            Change the interarrival time. Used to change the period or the
            arrival characteristics of the task.

            @param iat a random variable that describes the arrival
            pattern for this cyclic task.

            @return the pointer to the old variable random variable

            @todo what happens to the existing iat variable?  probably to
            be changed: the task is the owner of the variable, so it
            should be responsible for deletion. Therefore, we would simply
            return the period or minimum interarrival time, or just a copy
            of the random variable with another function. This function
            should not return anything.
	 */
	RandomVar *changeIAT(RandomVar *iat);
	/**
            From AbsTask interface...
	 */
	virtual void schedule();

	/**
           From AbsTask interface...
	 */
	virtual void deschedule();

	/**
            Set the kernel for this task. Called by kernel.addTask(). If
            you want to remove a task from a kernel, call this function
            with k=NULL.
            @param k pointer to the kernel or 0.
            @throws KernelAlreadySet if the user has already set a kernel
            for this task. To move a task from one kernel to another, call
            setKernel(0) before.
	 */
	void setKernel(AbsKernel *k) ;

	/**
            Returns the kernel that contains this task. Can return 0
            if this taks does not belong to any kernel.
	 */
	AbsKernel *getKernel() { return _kernel; }

	/**
            Returns true if the task is active.
	 */
	virtual bool isActive() const;

	/**
            Returns true if the task is executing.
	 */
	virtual bool isExecuting() const;

	/**
            Returns the worst-case computation time. If the maxC parameter
            in the constructor has not been set, or if it has been set
            equal to 0, then this function invokes the getWCET() on every
            instruction in the instruction list, and sums the number
            togheter.  If maxC has been set to a number different from 0,
            then this function returns maxC.
	 */
	virtual sc_time getWCET() const;

	sc_time getDeadline() const {return _dl;}
	sc_time getRelDline() const {return _rdl;}

	void setRelDline(const sc_time& dl) {_rdl = dl;}

	/**
            Returns a pointer to the CPU on which this task is executing.
	 */
	CPU *getCPU() const;

	/**
            Returns a pointer to the old CPU on which this task was
            executing.
	 */
	CPU *getOldCPU() const;

	/*Set and get the task affinity*/
	void set_affinity(uint16_t affinity) {task_affinity=affinity;}
	uint16_t get_affinity(){return task_affinity;}

	/**
            Parse and insert instructions into this task. The input string
            must be a sequence of instructions separated by a
            semicolon. Last instruction must also end with a
            semicolon. The instruction can be one of the following:

            - fixed(n) takes an integer number and generates an
            instruction with fixed computation time;
            - delay(var) takes a random variable and generates a ExecInstr
            instruction with variable execution time, according to the
            specified random variable;
            - wait(r) or lock(r) takes a string r containing the name of a
            resource and produces a WaitInstr on that resource;
            - signal(r) or unlock(r) takes a string r containing the name
            of a resource and produces a SignalInstr on that resource;
            - activate(t) takes a string containing the name of a task and
            generates an ActivateInstr on that task.

            An example of usage is the following:

            @code
            t1.insertCode("fixed(4);wait(Res1);delay(unif(4,10));
            signal(Res1); delay(unif(10,20));");
            @endcode

            In this case, the task performs 5 instructions; the first one
            lasts 4 sc_times; the second one is a wait on resource Res1; the
            third one has variable execution time, uniformely distributed
            between 4 and 10 sc_times; the fourth one is a signal on resource
            Res1; finally, the last instruction has variable execution
            time uniformely distributed between 10 and 20 sc_times.
	 */
	void insertCode(const string &code); //throw(ParseExc);

	/**
            For debugging reasons. Normally you do not need to call this!

	 */
	void printInstrList() const;

	/**
            Function inherited from AbsTask. It refreshes the state of the
            executing task when a change of the CPU speed occurs.

            @todo check which function calls this one.
	 */
	void refreshExec(double oldSpeed, double newSpeed);

	int getTaskNumber() const { return _id;}

	void setAbort(bool f) { deadEvt.setAbort(f); }
	void setnotkill(bool f) { deadEvt.setKill(false); }
	/**
           Returns a constant reference to the instruction queue
           (instrQueue)
	 */
	const InstrList& getInstrQueue() { return instrQueue; };

	/**
           Returns a constant reference to the actual instruction
          (actInstr)
	 */
	const InstrIterator& getActInstr() {return actInstr;};

	/**
        	   Reset the instruction queue pointer:
        	   actInstr = instrQueue.begin()
	 */
	void resetInstrQueue();
	/**
                    Reactivates the task. This method is used to implement a
                    cyclic task. When an arrival event is processed by the
                    onArrival() event handler, this function is called to post the
                    next arrival event.
	 */
	virtual void reactivate();

	/**Task
                    Handle arrival. This is the true arrival event handler.

	 */
	virtual void handleArrival(sc_time arrival);

	/** handles buffered arrivals:  inserts an arrival in the buffer */
	void buffArrival();

	/** handles buffered arrivals: removes an arrival from the buffer */
	void unbuffArrival();

	/** handles buffered arrivals: returns an arrival from the buffer */
	sc_time getBuffArrival();

	/** handles buffered arrivals: returns true if there is a buffered
                    arrival */
	bool chkBuffArrival() const;

};

std::string taskname(const AbsRTTask *t);

#endif
