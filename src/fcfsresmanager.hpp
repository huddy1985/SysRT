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
#ifndef __FCFSRESMANAGER_HPP__
#define __FCFSRESMANAGER_HPP__

#include <deque>
#include <map>
#include <resmanager.hpp>

    class AbsRTTask;
    class Task;

    /**
       Simple Resource manager which implements a FCFS strategy 
       for a single resource
    */
    class FCFSResManager : public ResManager {
    public:
        /** Constructor of FCFSResManager
         * 
         * @param n is the resource manager name
         */
        FCFSResManager(sc_module_name n = "fcfs_resource_manager");
 
        virtual void newRun();
        virtual void endRun();
 
    protected:
        virtual bool request(AbsRTTask*, Resource*, int n=1);
        virtual void release(AbsRTTask*, Resource*, int n=1); 
    private:  
        map<Resource *, AbsRTTask *> _resAndCurrUsers;
        typedef deque<AbsRTTask *> BLOCKED_QUEUE;
        map<Resource *, BLOCKED_QUEUE> _blocked;
    };

#endif
