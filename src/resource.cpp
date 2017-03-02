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

#include <abstask.hpp>
#include <resource.hpp>
using namespace std;

Resource::Resource(sc_module_name n, int nr) :
        		sc_module(n),
				_total(nr),
				_available(nr),
                _owner(0)
{
}

Resource::Resource(const Resource &r) :
        		sc_module(string(r.name())+"_copy"),
				_total(r.total()),
				_available(r.total()),
				_owner(0)
{
}

void Resource::lock(AbsRTTask *t, int n)
{
	_available -= n;
	_owner = t;
}

void Resource::unlock(int n)
{
	_available += n;
	_owner = 0;
}

bool Resource::isLocked() const
{
	return (_available == 0);
}

int  Resource::available() const
{
	return _available;
}

int  Resource::total() const
{
	return _total;
}

void Resource::newRun()
{
}

void Resource::endRun()
{
}


AbsRTTask* Resource::getOwner()const
{
	return _owner;
}

