/***************************************************************************
 Author: Jun Xiao and Cesare Bartolini
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <task.hpp>
#include <taskevt.hpp>
#include <cstdlib>

void ArrEvt::handler()
{
	_task->onArrival();
}

void EndEvt::handler()
{
	_task->onEndInstance();
}

void KillEvt::handler()
{
	_task->onKill();
}

void SchedEvt::handler()
{
	_task->onSched();
}

void DeschedEvt::handler()
{
	_task->onDesched();
}

void FakeArrEvt::handler()
{
	_task->onFakeArrival();
}

void DeadEvt::handler()
{
	if (_abort)
	{
		exit(-1);
	}
	if (_kill && _task->isActive() == true)
	{
		_task->killInstance();
	}
}

