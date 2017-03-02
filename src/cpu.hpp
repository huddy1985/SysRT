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
#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <set>
#include <string>
#include <vector>
#include <systemc.h>
#include <abskernel.hpp>
#include <cpuevt.hpp>

using namespace std;

struct cpulevel {
	/// Voltage of each step (in Volts)
	double voltage;

	/// Frequency of each step (in MHz)
	int frequency;

	/// The speed is a value between 0 and 1
	double speed;
};

/**
		cpu model
 */

class CPU: public sc_module {
public:
	vector<cpulevel> steps;
	/// Name of the CPU
	string cpuName;
	/// currentLevel is a value between 0 and steps.size() -1
	int currentLevel;
	bool PowerSaving;
	/// Number of speed changes
	unsigned long int frequencySwitching;
	AbsKernel *_kernel;

	// this is the CPU index in a multiprocessor environment
	int index;
	sc_time start_context_switch;

	BeginDispatchMultiEvt _beginEvt;
	EndDispatchMultiEvt _endEvt;

public:
	SC_HAS_PROCESS(CPU);
	// Constructor for CPUs without Power Saving
	CPU(sc_module_name name = sc_gen_unique_name("cpu")) : sc_module(name), frequencySwitching(0), index(0)
	{
		//cpuName = name;
		PowerSaving = false;
		SC_METHOD(onBeginDispatchMul);
		dont_initialize();
		sensitive << _beginEvt;
		SC_METHOD(onEndDispatchMul);
		dont_initialize();
		sensitive << _endEvt;
	}

	// Constructor for CPUs with Power Saving
	CPU(int num_levels, double V[], int F[],sc_module_name name= sc_gen_unique_name("cpu")):
		sc_module(name),frequencySwitching(0), index(0)
	{

		// Setting voltages and frequencies
		for (int i = 0; i < num_levels; i ++) {
			cpulevel cl;
			cl.voltage = V[i];
			cl.frequency = F[i];
			steps.push_back(cl);
		}

		// Setting speeds (basing upon frequencies)
		for (vector<cpulevel>::iterator iter = steps.begin();
				iter != steps.end(); iter++)
			(*iter).speed = ((double) (*iter).frequency) /
			((double)F[num_levels -1]);

		currentLevel = num_levels - 1;
		PowerSaving = true;
		SC_METHOD(onBeginDispatchMul);
		dont_initialize();
		sensitive << _beginEvt;
		SC_METHOD(onEndDispatchMul);
		dont_initialize();
		sensitive << _endEvt;
	}


	~CPU();

	void onBeginDispatchMul();
	void onEndDispatchMul();
	void begin_event_notify(sc_time begintime) {_beginEvt.cancel();_beginEvt.notify(begintime);}
	void begin_event_cancel() {_beginEvt.cancel();}
	void end_event_notify(sc_time endtime) {_endEvt.cancel();_endEvt.notify(endtime);}
	void end_event_cancel(){_endEvt.cancel();}
	void setKernel(AbsKernel * mulkernel){_kernel=mulkernel;}
	/// set the processor index
	void setIndex(int i) { index = i; }

	/// get the processor index
	int getIndex() { return index; }

	/// Useful for debug
	virtual int getCurrentLevel();

	/// Returns the maximum power consumption obtainable with this
	/// CPU
	virtual double getMaxPowerConsumption();

	/// Returns the current power consumption of the CPU If you
	/// need a normalized value between 0 and 1, you should divide
	/// this value using the getMaxPowerConsumption() function.

	virtual double getCurrentPowerConsumption();

	/// Returns the current power saving of the CPU
	virtual double getCurrentPowerSaving();

	/** Sets a new speed for the CPU accordingly to the system
	 *  load.  Returns the new speed.
	 */
	virtual double setSpeed(double newLoad);

	/// Returns the current CPU speed (between 0 and 1)
	virtual double getSpeed();

	virtual double getSpeed(int level);

	virtual unsigned long int getFrequencySwitching();

	virtual void newRun() {};
	virtual void endRun() {};

	///Useful for debug
	virtual void check();
};


typedef set<CPU*> CPUSet;

/**
 * The abstract CPU factory. Is the base class for every CPU factory which
 * will be implemented.
 */
class absCPUFactory {
public:
	/*
	 * Allocates a CPU and returns a pointer to it
	 */
	virtual CPU* createCPU(int num_levels=1,double V[]=NULL, int F[]=NULL,sc_module_name name=sc_gen_unique_name("cpu")) = 0;

	/**
            Virtual destructor.
	 */
	virtual ~absCPUFactory() {}
};


/**
 * uniformCPUFactory. A factory of uniform CPUs (whose speeds are maximum).
 * Allocates a CPU and returns a pointer to it
 */
class uniformCPUFactory : public absCPUFactory {
	char** _names;
	int _curr;
	int _n;
	int index;
public:
	uniformCPUFactory();
	uniformCPUFactory(char* names[], int n);
	CPU* createCPU(int num_levels=1,
			double V[]=NULL, int F[]=NULL,sc_module_name name=sc_gen_unique_name("cpu"));
};


#endif
