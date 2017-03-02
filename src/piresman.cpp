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
#include <piresman.hpp>
#include <resource.hpp>

    using namespace std;

    PIRManager::PIRManager(sc_module_name n) : ResManager(n)
    {
    }



    void PIRManager::newRun()
    {
    }

    void PIRManager::endRun()
    {
    }

    bool PIRManager::request(AbsRTTask *t, Resource *r, int n)
    {
        bool ret;

        TaskModel* taskModel = _sched->find(t);

        if (taskModel == NULL) {
            throw BaseExc("Cannot find task model!");
        }

        // if resource is not free...
        if (r->isLocked()) {
            // Task that owns the resource
            AbsRTTask *owner = dynamic_cast<AbsRTTask *>(r->getOwner());
            TaskModel* ownerModel = _sched->find(owner);

            if (ownerModel == NULL) {
                throw BaseExc("Cannot find owner model!");
            }   
            // remove the owner from the scheduler. It is not executing.
            _sched->extract(owner);
            // suspend the task.
            _kernel->suspend(t);
            // owner priority = task priority
            ownerModel->changePriority(taskModel->getPriority());
            // reactivate the owner with the new priority
            _kernel->activate(owner);
            // push the blocked task into the blocked queue
            blocked[r->name()].insert(taskModel);
            ret = false;
        }
        else {
            // (if the entry in oldPriorities does not exist?)
            if (oldPriorities.find(t) == oldPriorities.end()) {
                PRIORITY_MAP mm;
                mm[r->name()] = taskModel->getPriority().to_double();
                oldPriorities[t] = mm;
            }
            else {
                (oldPriorities[t])[r->name()] = taskModel->getPriority().to_double();
            }
            r->lock(t);
            ret = true;
        }
        return ret;
    }

    void PIRManager::release(AbsRTTask *t, Resource *r, int n)
    {
        TaskModel* taskModel = _sched->find(t);
        r->unlock();
        // see if there is any blocked task
        if (!blocked[r->name()].empty())
        {
            TaskModel *newTaskModel = blocked[r->name()].front();
            blocked[r->name()].erase(newTaskModel);
            _kernel->suspend(t);
            taskModel->changePriority(sc_time(oldPriorities[t][r->name()],SC_PS));
            if (t->isActive()) _kernel->activate(t);
            _kernel->activate(newTaskModel->getTask());
            r->lock(newTaskModel->getTask());
        }
    }

