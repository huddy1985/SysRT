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
#include <strtoken.hpp>
#include <factory.hpp>
#include <fifosched.hpp>
#include <fpsched.hpp>
#include <edfsched.hpp>
#include <rrsched.hpp>


const string FIFOName("FIFOSched");
const string FPName("FPSched");
const string EDFName("EDFSched");
const string RRName("RRSched");


void __regsched_init()
{
	static registerInFactory<Scheduler, FIFOScheduler, string>
	registerfifo(FIFOName);

	static registerInFactory<Scheduler, FPScheduler, string>
	registerfp(FPName);

	static registerInFactory<Scheduler, EDFScheduler, string>
	registeredf(EDFName);


}
