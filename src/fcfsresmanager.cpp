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
#include <abskernel.hpp>
#include <fcfsresmanager.hpp>
#include <resource.hpp>
#include <task.hpp>


FCFSResManager::FCFSResManager(sc_module_name n) : ResManager(n)
{
}

void FCFSResManager::newRun()
{
}

void FCFSResManager::endRun()
{
}

bool FCFSResManager::request(AbsRTTask *t, Resource *r, int n)
{
	bool ret;

	if (r->isLocked()) {
		_kernel->suspend(t);
		_blocked[r].push_back(t);
		ret = false;
	}
	else {
		r->lock(t);
		_resAndCurrUsers[r] = t;
		ret = true;
	}
	return ret;
}

void FCFSResManager::release(AbsRTTask *t, Resource *r, int n)
{
	r->unlock();
	_resAndCurrUsers[r] = NULL;
	if (!_blocked[r].empty()) {
		r->lock(_blocked[r].front());
		_resAndCurrUsers[r] = _blocked[r].front();
		_kernel->activate(_blocked[r].front());
		_blocked[r].pop_front();
	}
}

