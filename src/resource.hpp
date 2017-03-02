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

#ifndef __RESOURCE_HPP__
#define __RESOURCE_HPP__

#include <string>
#include <systemc.h>

class AbsRTTask;

/**
        Very simple class that models a resource shared by two or more
        tasks. Every task should use this resource only through a critical
        section surrounded by wait and signal instructions.

        @see ResManager, WaitInstr, SignalInstr
 */
class Resource: public sc_module
{
protected:
	int _total;
	int _available;
	AbsRTTask* _owner;

public:

	/// simple constructor
	Resource(sc_module_name n, int nr = 1);

	/// copy constructor
	Resource(const Resource &r);

	/// lock the resource
	void lock(AbsRTTask *owner, int n = 1);

	/// unlock the resource
	void unlock(int n = 1);

	/// checks if the resource is locked
	bool isLocked() const;

	/// returns how many resource units are available
	int available() const;

	/// maximum number of resource units
	int total() const;

	/// returns the resource owner
	AbsRTTask* getOwner() const;

	void newRun();
	void endRun();

};

#endif
