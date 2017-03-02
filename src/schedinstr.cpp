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
#include <schedinstr.hpp>


SchedInstr::SchedInstr(Task * f, const string& s, char *n)
: Instr(f, n), _endEvt(this), _threEvt(f, this)
{}

Instr* SchedInstr::createInstance(vector<string> &par)
{
	return new SchedInstr(dynamic_cast<Task*> (sc_find_object(par[1].c_str())),par[0]);
}

void SchedInstr::endRun()
{
	_endEvt.cancel();
	_threEvt.cancel();
}

void SchedInstr::schedule()
{
	_endEvt.notify();
}

void SchedInstr::deschedule()
{
	_endEvt.cancel();
}

void SchedInstr::onEnd()
{
	_father->onInstrEnd();
	RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());
	if (k == NULL) throw BaseExc("Kernel not found!");
	k->disableThreshold();
	k->dispatch();
	_threEvt.notify();
}


