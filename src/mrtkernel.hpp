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
#ifndef __MRTKERNEL_HPP__
#define __MRTKERNEL_HPP__

#include <vector>
#include <map>
#include <iterator>
#include <kernel.hpp>
#include <kernevt.hpp>
#include <systemc.h>

//#define _MRTKERNEL_DBG_LEV "MRTKernel"

class absCPUFactory;

/**

        An implementation of a real-time multi processor kernel with
        global scheduling. It contains:

        - a pointer to the CPU factory used to create CPUs managed by
          the kernel;

        - a pointer to a Scheduler, which implements the scheduling
          policy;

        - a pointer to a Resource Manager, which is responsable for
          resource access related operations and thus implements a
          resource allocation policy;

        - a map of pointers to CPU and task, which keeps the
          information about current task assignment to CPUs;

        - the set of tasks handled by this kernel.

        This implementation is quite general: it lets the user of this
        class the freedom to adopt any scheduler derived form
        Scheduler and a resorce manager derived from ResManager or no
        resorce manager at all.  The kernel for a multiprocessor
        system with different CPUs can be also be simulated. It is up
        to the instruction class to implement the correct duration of
        its execution by asking the kernel of its task the speed of
        the processor on whitch it's scheduled.

        We will probably have to derive from this class to implement
        static partition and mixed task allocation to CPU.

        @see absCPUFactory, Scheduler, ResManager, AbsRTTask
 */
class MRTKernel : public RTKernel {
public:
	std::fstream cstmc;
	std::vector<CPU *> cpus;
	//the number of migration and context switch
	int migration;
	int context_switch;
protected:

	/// CPU Factory. Used in one of the constructors.
	absCPUFactory *_CPUFactory;

	/// The currently executing tasks (one per processor).
	map<CPU *, AbsRTTask *> _m_currExe;

	/// Where the task was executing before being suspended
	map<const AbsRTTask *, CPU*> _m_oldExe;

	/// This denotes where the task has been dispatched.  The set
	/// of dispatched tasks includes the set of executing tasks:
	/// first a task is dispatched, (but not executing yet) in the
	/// onBeginDispatchMulti. Then, in the onEndDispatchMulti, its
	/// execution starts on the processor.
	std::map<const AbsRTTask *, CPU*> _m_dispatched;

	/// true is the CPU is on a context switch
	std::map<CPU*, bool> _isContextSwitching;
	std::map<CPU*, sc_time> start_context_switching;
	std::map<CPU*, sc_time> switch_overhead;

	/// the amount of delay due to migration (will become a
	/// RandomVar sc_eventually).
	sc_time  _migrationDelay;

	void internalConstructor(int n);

	/**
	 * Returns a pointer to a free CPU (NULL if every CPU is busy).
	 */
	CPU *getFreeProcessor();

	bool isDispatched(CPU *p);

	typedef map<CPU *, AbsRTTask *>::iterator ITCPU;
	ITCPU getNextFreeProc(ITCPU s, ITCPU e);

public:

	/**
	 * Constructor: needs to know which scheduler and CPU factory
	 * the kernel want to use, and how many processor the system
	 * is composed of.
	 */
	MRTKernel(Scheduler*, absCPUFactory*, int n=1,
			sc_module_name name = "");

	/**
	 * Constructor: needs to know which scheduler the kernel want
	 * to use, and from how many processor the system is composed.
	 */
	MRTKernel(Scheduler*, int n=1, sc_module_name name = "");

	/**
           Needs to know scheduler and name
	 */
	MRTKernel(Scheduler*, sc_module_name name);

	~MRTKernel();

	/**
	 * Adds a CPU to the set of CPUs handled by the kernel.
	 */
	void addCPU(CPU*);
	virtual void terminate(AbsRTTask *t);

	/**
           Add a task to the kernel

           Spcify the scheduling parameters in param.
	 */
	virtual void addTask(AbsRTTask &t, const std::string &param = "");
	virtual void addTask(AbsRTTask &t, int param);

	// inherited from RTKernel
	virtual void onArrival(AbsRTTask *);
	virtual void suspend(AbsRTTask *);
	virtual void onEnd(AbsRTTask *);

	/**
            Dispatching on a given CPU. 

            This is different from the version we have on RTKernel,
            since we may need to specify on which CPU we have to
            select a new task (for example, in the onEnd() and
            suspend() functions). In the onArrival() function,
            instead, we stil do not know which processor is
            free!

	 */
	virtual void dispatch(CPU *cpu);

	/**
           This function is called by the onArrival and by the
            activate function. When we call this, we first select a
            free processor, then we call the other dispatch,
            specifying on which processor we need to schedule.
	 */
	virtual void dispatch();
	virtual void dispatch(AbsRTTask* t);

	virtual void onBeginDispatchMulti(CPU* e);
	virtual void onEndDispatchMulti(CPU* e);


	/**
	 * Returns a pointer to the CPU on whitch t is running (NULL if
	 * t is not running on any CPU)
	 */
	virtual CPU *getProcessor(const AbsRTTask *) const;

	/**
	 * Returns a pointer to the CPU on whitch t was running (NULL if
	 * t was not running on any CPU)
	 */
	virtual CPU *getOldProcessor(const AbsRTTask *) const;

	/**
           Returns a vector containing the pointers to the processors.
           Deprecated, will be removed soon
	 */
	std::vector<CPU*> getProcessors() const;

	/**
           Set the migration delay. This is the overhead to be added
           to the ContextSwitchDelay when the task is migrated from
           one processor to another.
	 */
	void setMigrationDelay(const sc_time &t) {
		_migrationDelay = t;
	}
	virtual void newRun();
	virtual void endRun();
	virtual void print();
	virtual void printState();
	/**
	 *  Returns a pointer to the task which is executing on given
	 *  CPU (NULL if given CPU is idle)
	 */
	virtual AbsRTTask* getTask(CPU*);
	/**
         It returns the name of the tasks (std::string) stored in a
         std::vector<std::string>.
         Each element of the vector corresponds to the name of a
         different running task.
	 */
	virtual std::vector<std::string> getRunningTasks();
};

#endif
