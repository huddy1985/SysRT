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
#include <cpu.hpp>
#include <mrtkernel.hpp>


CPU::~CPU()
{
	steps.clear();
}
void CPU::onBeginDispatchMul()
{

	// if necessary, deschedule the task.
	MRTKernel *multikernel = dynamic_cast<MRTKernel *>(_kernel);
	multikernel->onBeginDispatchMulti(this);
}

void CPU::onEndDispatchMul()
{
	// performs the "real" context switch
	MRTKernel *multikernel = dynamic_cast<MRTKernel *>(_kernel);
	multikernel->onEndDispatchMulti(this);
}


int CPU::getCurrentLevel()
{
	if (PowerSaving)
		return currentLevel;
	else
		return 0;
}

double CPU::getMaxPowerConsumption()
{
	int numlevels = steps.size();
	if (PowerSaving)
		return (steps[numlevels-1].frequency)*(steps[numlevels-1].voltage)*(steps[numlevels-1].voltage);
	else
		return 0;
}

double CPU::getCurrentPowerConsumption()
{
	if (PowerSaving)
		return (steps[currentLevel].frequency)*(steps[currentLevel].voltage)*(steps[currentLevel].voltage);
	else
		return 0;
}

double CPU::getCurrentPowerSaving()
{
	if (PowerSaving) {
		long double maxPowerConsumption = getMaxPowerConsumption();
		long double saved = maxPowerConsumption - getCurrentPowerConsumption();
		return (double) (saved/maxPowerConsumption);
	}
	else
		return 0;
}


double CPU::setSpeed(double newLoad)
{
	if (PowerSaving) {
		for (int i=0; i < (int) steps.size(); i++)
			if (steps[i].speed >= newLoad && i != currentLevel) {
					frequencySwitching++;
				    currentLevel = i;
				    return steps[i].speed; //It returns the new speed
		}

		return 1; //an error
	}
	else
		return 1; // An error occurred or PowerSaving is not enabled
}


double CPU::getSpeed()
{
	if (PowerSaving)
		return steps[currentLevel].speed;
	else
		return 1;
}


double CPU::getSpeed (int level)
{
	int numlevels = steps.size();
	if ( (!PowerSaving) || (level > (numlevels - 1)) )
		return 1;
	else
		return steps[level].speed;
}

unsigned long int CPU::getFrequencySwitching()
{
	return frequencySwitching;
}




void CPU::check(){
	cout << "Checking CPU:" << cpuName << endl;;
	cout << "Max Power Consumption is :" << getMaxPowerConsumption() << endl;
	for (vector<cpulevel>::iterator iter = steps.begin(); iter != steps.end(); iter++){
		cout << "-Level-" << endl;
		cout << "\tFrequency:" << (*iter).frequency << endl;
		cout << "\tVoltage:" << (*iter).voltage << endl;
		cout << "\tSpeed:" << (*iter).speed << endl;
	}
	for (int i=0; i < (int) steps.size(); i++)
		cout << "Speed level" << getSpeed(i) << endl;
	for (vector<cpulevel>::iterator iter = steps.begin(); iter != steps.end(); iter++){
		cout << "Setting speed to " << (*iter).speed << endl;
		setSpeed((*iter).speed);
		cout << "New speed is  " << getSpeed() << endl;
		cout << "Current level is  " << getCurrentLevel() << endl;
		cout << "Current Power Consumption is  " << getCurrentPowerConsumption() << endl;
		cout << "Current Power Saving is  " << getCurrentPowerSaving() << endl;
	}
}





uniformCPUFactory::uniformCPUFactory()
{
	_curr=0;
	_n=0;
	index = 0;
}


uniformCPUFactory::uniformCPUFactory(char* names[], int n)
{
	_n=n;
	_names = new char*[n];
	for (int i=0; i<n; i++) {
		_names[i]=names[i];
	}
	_curr=0;
	index = 0;
}


CPU* uniformCPUFactory::createCPU(int num_levels, double V[], int F[],sc_module_name name)
{
	CPU *c;
	if (_curr==_n)
		if (num_levels==1)
			// Creates a CPU without Power Saving:
			c =  new CPU(name);
			else
				// Creates a CPU with Power Saving:
				c = new CPU(num_levels, V, F,name);
	else
		if (num_levels==1)
			// Creates a CPU without Power Saving:
			c =  new CPU(_names[_curr++]);
		else
			// Creates a CPU with Power Saving:
			c = new CPU(num_levels, V, F,_names[_curr++]);

	c->setIndex(index++);
	return c;
}
