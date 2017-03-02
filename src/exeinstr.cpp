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
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <stdlib.h>
#include <factory.hpp>
#include <strtoken.hpp>

#include <cpu.hpp>
#include <exeinstr.hpp>
#include <task.hpp>
#include <systemc.h>

using namespace std;
using namespace parse_util;

Instr *ExecInstr::createInstance(vector<string> &par)
{
	Instr *temp = 0;
	Task *task = dynamic_cast<Task*> (sc_find_object(par[1].c_str()));
	if (isdigit((par[0].c_str())[0])) {
		temp = new FixedInstr(task, sc_time::from_value(strtoul(par[0].c_str(),NULL,0)));
	}
	else {
		string token = get_token(par[0]);
		string p = get_param(par[0]);
		vector<string> parms = split_param(p);

		auto_ptr<RandomVar> var(genericFactory<RandomVar>::instance().create(token,parms));

		temp = new ExecInstr(task, var);
	}
	return temp;
}

void ExecInstr::newRun()
{
	actTime = sc_time::from_value(0);
	lastTime = sc_time::from_value(0);
	flag = true;
	execdTime = sc_time::from_value(0);
	executing = false;
}

void ExecInstr::endRun()
{
	_endEvt.cancel();
}

sc_time ExecInstr::getExecTime() const
{
	sc_time t = sc_time_stamp();
	if (executing) return (execdTime + t - lastTime);
	else return execdTime;
}

sc_time ExecInstr::getDuration() const
{
	//may generate error!!!!!!
	return sc_time::from_value(cost->get());
}

sc_time ExecInstr::getWCET() const throw(RandomVar::MaxException)
    		{
	return sc_time::from_value(cost->getMaximum());
    		}

void ExecInstr::schedule()
{

	sc_time t = sc_time_stamp();
	lastTime = t;
	executing = true;

	if (flag) {
		execdTime = sc_time::from_value(0);
		actTime = sc_time::from_value(0);
		flag = false;
		currentCost = sc_time::from_value(cost->get());
	}

	double currentSpeed=1;
	sc_time tmp = sc_time::from_value(0);
	if (currentCost > actTime)
		tmp = sc_time::from_value((double)ceil( (currentCost - actTime).to_double()/currentSpeed));
	_endEvt.notify(tmp);

}

void ExecInstr::deschedule()
{
	sc_time t = sc_time_stamp();

	_endEvt.cancel();

	if (executing) {
		double currentSpeed =1;
		actTime += sc_time::from_value(((t - lastTime).to_double())*currentSpeed);// number of cycles
		execdTime += (t - lastTime);// number of sc_times
		//can be deleted?
				lastTime = t;
	}
	executing = false;

}



void ExecInstr::onEnd()
{
	sc_time t = sc_time_stamp();
	execdTime += t - lastTime;
	flag = true;
	executing = false;
	lastTime = t;
	actTime = sc_time::from_value(0);
	_endEvt.cancel();
	_father->onInstrEnd();

}


void ExecInstr::reset()
{
	actTime = lastTime = sc_time::from_value(0);
	flag = true;
	execdTime = sc_time::from_value(0);
	_endEvt.cancel();
}


Instr *FixedInstr::createInstance(vector<string> &par)
{
	Task *task = dynamic_cast<Task*> (sc_find_object(par[1].c_str()));
	return new FixedInstr(task, sc_time::from_value(strtoul(par[0].c_str(),NULL,0)));
}


void ExecInstr::refreshExec(double oldSpeed, double newSpeed){
	sc_time t = sc_time_stamp();
	_endEvt.cancel();
	actTime += sc_time::from_value(((t - lastTime).to_double())*oldSpeed);
	execdTime += (t - lastTime);
	lastTime = t;

	sc_time tmp = sc_time::from_value(0);
	if (currentCost > actTime)
		tmp = sc_time::from_value(ceil ((currentCost - actTime).to_double()/newSpeed)) ;

	_endEvt.notify(tmp);
}

