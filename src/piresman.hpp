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
#ifndef __PIRESMAN_HPP__
#define __PIRESMAN_HPP__

#include <map>
#include <plist.hpp>
#include <resmanager.hpp>
#include <scheduler.hpp>

//#define _PIRESMAN_DBG_LEV  "piresman"

    /**
       This class implements the Priority Inheritance Protocol. It has the
       same interface of any Resource Manager class, in addition it has a
       setScheduler() function, because the PI needs the scheduler for
       changing the priority of a task.
    */
    class PIRManager : public ResManager {
//         Scheduler *_sched;
    public:

        /**
         * Constructor
         */
        PIRManager(sc_module_name n = "PI_resouce_manager");
  
        void newRun();

        void endRun();

    protected:
        /**
           Returns true if the resource can be locked, false otherwise
           (in such a case, the task should be blocked)
         */
        virtual bool request(AbsRTTask *t, Resource *r, int n=1);
        
        /**
           Releases the resource.
         */
        virtual void release(AbsRTTask *t, Resource *r, int n=1);

    private:
        /// correspondence task / priority 
        typedef map<std::string, int> PRIORITY_MAP;
        /// Blocked tasks, ordered by priority. 
        /// There is one such queue for each resource
        typedef priority_list<TaskModel *, TaskModel::TaskModelCmp> BLOCKED_QUEUE;
        // Stores the old task priorities
        map<AbsRTTask *, PRIORITY_MAP> oldPriorities;
        // stores the blocked tasks for each resource, ordered by priority
        map<string, BLOCKED_QUEUE> blocked;
    };

#endif
