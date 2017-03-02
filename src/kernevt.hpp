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
#ifndef __KERNEVT_HPP__
#define __KERNEVT_HPP__

#include <systemc.h>
#include <kernel.hpp>


class RTKernel;

class KernelEvt : public sc_event
{
protected:
	RTKernel* _kernel;

public:
	KernelEvt(RTKernel* k)
: sc_event() { _kernel=k; }
	void setKernel(RTKernel* k) { _kernel=k; };
	RTKernel* getKernel(){ return _kernel; };
};

class DispatchEvt : public KernelEvt
{
public:
	DispatchEvt(RTKernel* k) : KernelEvt(k) {}
};

class BeginDispatchEvt : public KernelEvt
{
public:
	BeginDispatchEvt(RTKernel* k) : KernelEvt(k) {}
};

class EndDispatchEvt : public KernelEvt
{
public:
	EndDispatchEvt(RTKernel* k) : KernelEvt(k) {}
};


#endif
