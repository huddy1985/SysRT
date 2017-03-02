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
#include <fifosched.hpp>
#include <kernel.hpp>

    void FIFOScheduler::addTask(AbsRTTask *task) throw(RTSchedExc)
    {
        FIFOModel *model = new FIFOModel(task); 	

        if (find(task) != NULL) 
            throw RTSchedExc("Element already present");
	
        _tasks[task] = model;
    }

    void FIFOScheduler::addTask(AbsRTTask* task, const std::string &p)
    {
        if (!dynamic_cast<AbsRTTask *>(task)) 
            throw RTSchedExc("Cannot add a AbsRTTask to FIFO");
        // ignoring parameters
        addTask(dynamic_cast<AbsRTTask *>(task));
    }

    FIFOScheduler * FIFOScheduler::createInstance(vector<string> &par)
    {
        return new FIFOScheduler;
    }

