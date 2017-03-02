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
#include <kernel.hpp>
#include <task.hpp>
#include "suspend_instr.hpp"

using namespace std;

SuspendInstr::SuspendInstr(Task *f, sc_time d) :
			Instr(f),
			delay(d)
{
}

SuspendInstr * SuspendInstr::createInstance(vector<string> &par)
{
	if (par.size() != 2) throw parse_util::ParseExc("SuspendInstr::createInstance", "Wrong number of arguments"); 

	Task *t = dynamic_cast<Task*> (sc_find_object(par[1].c_str()));
	sc_time d = sc_time::from_value(atoi(par[0].c_str()));

	return new SuspendInstr(t, d);
}

void SuspendInstr::schedule()
{
	suspEvt.notify();
}

void SuspendInstr::deschedule()
{
}

void SuspendInstr::onSuspend(sc_event *evt)
{
	AbsKernel *k = _father->getKernel();
	k->suspend(_father);
	k->dispatch();
	resumeEvt.notify(delay);
}

void SuspendInstr::onEnd(sc_event *evt)
{
	_father->onInstrEnd();
	AbsKernel *k = _father->getKernel();
	k->onArrival(_father);
}

void SuspendInstr::newRun()
{
}

void SuspendInstr::endRun()
{
	suspEvt.cancel();
	resumeEvt.cancel();
}

