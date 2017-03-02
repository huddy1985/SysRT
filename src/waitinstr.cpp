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
#include <waitinstr.hpp>
Instr* WaitInstr::createInstance(vector<string> &par)
{
	return new WaitInstr(dynamic_cast<Task*> (sc_find_object(par[1].c_str())), par[0]);
}

void WaitInstr::endRun()
{
	_endEvt.cancel();
	_waitEvt.cancel();
}

void WaitInstr::schedule()
{
	_endEvt.notify();
}

void WaitInstr::deschedule()
{
	_endEvt.cancel();
}

void WaitInstr::onEnd()
{
	_father->onInstrEnd();
	RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());
	if (k == NULL) throw BaseExc("Kernel not found!");
	k->requestResource(_father, _res, _numberOfRes);
	_waitEvt.notify();
}

Instr* SignalInstr::createInstance(vector<string> &par)
{
	return new SignalInstr(dynamic_cast<Task*> (sc_find_object(par[1].c_str())), par[0]);
}

void SignalInstr::endRun()
{
	_endEvt.cancel();
	_signalEvt.cancel();
}

void SignalInstr::schedule()
{
	_endEvt.notify();
}

void SignalInstr::deschedule()
{
	_endEvt.cancel();
}


void SignalInstr::onEnd()
{
	_endEvt.cancel();
	_signalEvt.notify();
	_father->onInstrEnd();
	RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());
	if (k == 0) {
		throw BaseExc("SignalInstr has no kernel set!");
	}
	else k->releaseResource(_father, _res, _numberOfRes);
}
