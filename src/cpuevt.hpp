/***************************************************************************
Author: Jun Xiao
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __CPUEVT_HPP__
#define __CPUEVT_HPP__

#include <set>
#include <string>
#include <vector>
#include "systemc.h"

using namespace std;
class CPU;
class AbsRTTask;


class BeginDispatchMultiEvt : public sc_event {
};

/**
        This class models and sc_event of "start of context switch". It
        serves to implement a context switch on a certain processor.
        Different from the EndDispatchEvt for single processor
        kernels (RTKernel), since it needs to store a pointer to the
        CPU on which the context switch may happen.
 */
class EndDispatchMultiEvt : public sc_event {
	AbsRTTask *_task;
public:
	EndDispatchMultiEvt():sc_event(),_task(0){;}
	void setTask(AbsRTTask *t) {_task = t; }
	AbsRTTask *getTask() { return _task; }
};
#endif
