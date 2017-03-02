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
#ifndef __RTTASK_HPP__
#define __RTTASK_HPP__

#include <cstdlib>
#include <cstring>
#include <task.hpp>
#include "systemc.h"

using namespace std;

/**
       Models a simple periodic task. It's a simpler interface to
       Task.
 */
class PeriodicTask: public Task
{
	sc_time period;
	float utilization_weight;
public:
	PeriodicTask(sc_time iat);

	PeriodicTask(sc_time iat, sc_time rdl, sc_time ph,
			sc_module_name name = "", long qs = 100);
	virtual ~PeriodicTask(){;};
	inline sc_time getPeriod() { return period; }
	float get_weight() {return utilization_weight;}
	void compute_weight(){
		utilization_weight= getWCET().to_double()/period.to_double();
	}
};

#endif
