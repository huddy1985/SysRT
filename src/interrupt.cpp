/***************************************************************************
Anthor: Jun Xiao and Giuseppe Lipari
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cassert>

#include <randomvar.hpp>
#include <interrupt.hpp>
#include <task.hpp>

using namespace std;

Interrupt::~Interrupt()
{
	delete int_time;
	delete burst_lenght;
}

void Interrupt::addTask(Task *t)
{
	tasks.push_back(t);
}

void Interrupt::newRun()
{
	assert(int_time != NULL);
	assert(burst_lenght != NULL);

	triggerEvt.notify(sc_time((int)(int_time->get()),SC_PS));
	max_act = static_cast<int>(burst_lenght->get());
	count = 0;
}

void Interrupt::endRun()
{
}

void Interrupt::onTrigger()
{
	for (unsigned int i=0; i<tasks.size(); ++i) tasks[i]->activate();

	count ++;
	if (count == max_act) {
		triggerEvt.notify(sc_time( (int) (int_time->get()),SC_PS));
		max_act = static_cast<int>(burst_lenght->get());
		count = 0;
	}
	else
		triggerEvt.notify(sc_time(bp,SC_PS));
}


