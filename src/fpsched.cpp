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
#include <sstream>

#include <fpsched.hpp>
#include <kernel.hpp>


void FPScheduler::addTask(AbsRTTask *task, int prio)
{
	FPModel *model = new FPModel(task, sc_time::from_value(prio));
	_tasks[task] = model;
}

FPScheduler * FPScheduler::createInstance(vector<string> &par)
{
	return new FPScheduler;
}

void FPScheduler::addTask(AbsRTTask *task, const std::string &p)
{

	int prio;

	std::stringstream str(p);

	str >> prio;

	addTask(task, prio);


}
