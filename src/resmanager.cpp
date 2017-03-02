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
#include <abstask.hpp>
#include <resmanager.hpp>
#include <resource.hpp>

using namespace std;

ResManager::ResManager(sc_module_name n) : sc_module(n),
		_kernel(0),
		_res()
{
}

ResManager::~ResManager()
{
	std::vector<Resource *>::iterator i= _res.begin();
	for (;i != _res.end();i++) delete *i;

	// delete the resource list
	_res.clear();
}

void ResManager::setKernel(AbsKernel *k, Scheduler *s)
{
	_kernel = k;
	_sched = s;
}

void ResManager::addResource(sc_module_name name, int n)
{
	Resource *r = new Resource(name, n);
	_res.push_back(r);
}

bool ResManager::request(AbsRTTask *t, const string &name, int n)
{

	Resource *r = dynamic_cast<Resource *>( sc_find_object(name.c_str()) );
	bool ret = request(t,r,n);
	return ret;
}

void ResManager::release(AbsRTTask *t, const string &name, int n)
{
	Resource *r = dynamic_cast<Resource *>( sc_find_object(name.c_str()));
	release(t,r,n);
}
