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
#include "rttask.hpp"

PeriodicTask::PeriodicTask(sc_time iat)
: Task(new DeltaVar(iat.to_double()), iat, sc_time(0,SC_PS),sc_time(0,SC_PS), "", 1000), period(iat)
{
}

PeriodicTask::PeriodicTask(sc_time iat, sc_time rdl, sc_time ph,
		sc_module_name name, long qs)
: Task(new DeltaVar(iat.to_double()), rdl, ph,sc_time(0,SC_PS), name,qs), period(iat)
{
}


