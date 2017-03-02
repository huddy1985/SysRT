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
#include <threinstr.hpp>

ThreInstr::ThreInstr(Task * f, const string& th, char *n)
: Instr(f, n), _endEvt(this), _threEvt(f, this), _th(th)
{}

Instr* ThreInstr::createInstance(vector<string> &par)
{
	return new ThreInstr(dynamic_cast<Task*> (sc_find_object(par[1].c_str())), par[0]);
}

void ThreInstr::endRun()
{
	_endEvt.cancel();
	_threEvt.cancel();
}

void ThreInstr::schedule()
{
	_endEvt.notify();
}

void ThreInstr::deschedule()
{
	_endEvt.cancel();
}

void ThreInstr::onEnd()
{
	_father->onInstrEnd();
	RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());
	if (k == NULL) throw BaseExc("Kernel not found!");
	k->setThreshold(0);
	k->enableThreshold();
	_threEvt.notify();
}

