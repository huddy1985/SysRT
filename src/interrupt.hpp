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

#ifndef __INTERRUPT_HPP__
#define __INTERRUPT_HPP__

#include <vector>
#include <randomvar.hpp>
#include <systemc.h>

using namespace std;

class Task;

/**
     This class models an interrupt. It can be periodic/sporadic (the
     default), or bursty. The corresponding action is to activate a
     number of tasks.
 */
class Interrupt : public sc_module {
protected:
	/// the tasks
	vector<Task *> tasks;

	/// interarrival time between bursts
	RandomVar *int_time;

	/// bursty period (minimum interval btw consecutive interrupts)
	int bp;

	/// burst lenght (number of consecutive interrupts in a burst).
	RandomVar *burst_lenght;

	/// Counts the number of activations in the burst
	int count;

	/// max number of activations in this round
	int max_act;

public:

	/// Trigger Event
	sc_event triggerEvt;

	/**
       Constructor
       @param iat interarrival time btw bursts
       @param burstperiod minimum interval btw consecutive interrupts
       @param burstlenght number of consecutive instances in a burst
       @param name the name of the interrupt
	 */
	SC_HAS_PROCESS(Interrupt);
	Interrupt(RandomVar *iat,
			int burstperiod=1,
			RandomVar *burstlenght=NULL, sc_module_name name = sc_gen_unique_name("Interrupts")) :
				sc_module(name),
				int_time(iat),
				bp(burstperiod),
				burst_lenght(burstlenght),
				count(0),
				max_act(0)
	{
		if (burst_lenght == NULL) burst_lenght = new DeltaVar(1);
		newRun();
		SC_METHOD(onTrigger);
		dont_initialize();
		sensitive << triggerEvt;
	}
	~Interrupt();

	/// add a task to the interrupt activation list
	void addTask(Task *t);

	/**
       Called by the trigger event. Activates the tasks and posts the
       trigger event again.
	 */
	void onTrigger();

	void newRun();
	void endRun();
};


#endif
