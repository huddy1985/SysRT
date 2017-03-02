/*
 * test1.cpp
 *
 *  Created on: Oct 29, 2015
 *      Author: Jun Xiao
 */

#include <kernel.hpp>
#include <fpsched.hpp>
#include <edfsched.hpp>
#include <rttask.hpp>
#include <systemc.h>
#include <fifosched.hpp>
#include <rrsched.hpp>
#include <fcfsresmanager.hpp>
#include <piresman.hpp>
#include <interrupt.hpp>
#include <task.hpp>
#include <mrtkernel.hpp>
#include <parti_mrtkernel.hpp>
#include <pfair_scheduler.hpp>
#include <partition_edf.hpp>
#include <dagnode.hpp>
#include <npedf.hpp>
int sc_main(int argc, char* argv[])
{

	sc_set_time_resolution(1,SC_NS);
    // create the scheduler and the kernel
    //
	//FPScheduler fpsched("myschduler");
	//Parti_Mrtkernel
	//partition_scheduler fpsched(2);

	//EDFScheduler edfsched;
	//NGEDFScheduler ngedf;
	//FIFOScheduler   fifo("myschduler");
	//RRScheduler rrsched("rrschded",10);
    //MRTKernel kern(&fpsched,"myrtkernel");
    //MRTKernel kern(&fpsched,11,"myrtkernel");
	//RTKernel kern(&fifosched,"myrtkernel");
	pfair_scheduler pfair("pfair",sc_time(10,SC_NS));
	MRTKernel kern(&pfair,6,"myrtkernel");
	//Parti_Mrtkernel kern(&ngedf,2,"myrtkernel");
    //fpsched.set_scheduller(0,&edfsched);
    //fpsched.set_scheduller(1,&fifosched);
    //FCFSResManager resMan;
    //PIRManager resMan;
    //kern.setResManager(&resMan);

      // creates semaphore R1
      //resMan.addResource("R1",1);
      //resMan.addResource("R2",1);
    // creates the two tasks.
    /* t1 has a period of 15, a relative deadline of 15, starts at time 0,
     and is called "taskA" */
    //Three hard real time tasks
    PeriodicTask t1(sc_time(50,SC_NS),sc_time(50,SC_NS),sc_time(0,SC_NS), "task_1");
    // Creates the pseudoinstructions for the two tasks
    //t1.insertCode("fixed(2);wait(R1);fixed(2);signal(R1);");
      //t1.insertCode("fixed(1);wait(R1);fixed(2);signal(R1);fixed(1);");
    //t1.killOnMiss(true);
//    t1.setTrace(&jtrace);
     //RTTask t1(new UniformVar(10,20), 10, 0, "task A");
      t1.insertCode("fixed(20);");
      //t1.set_affinity(0);
//    /* t2 has a period of 20, a relative deadline of 20, starts at time 0,
//     and is called "taskB" */
    //PeriodicTask t2(sc_time(20,SC_NS), sc_time(20,SC_NS), sc_time(0,SC_NS), "task1");
    //t2.insertCode("fixed(3);wait(R1);fixed(4);signal(R1);fixed(2);");
    //t2.insertCode("fixed(2);wait(R2);fixed(2);signal(R2);fixed(1);");
//    t2.setTrace(&jtrace);
     PeriodicTask t2(sc_time(90,SC_NS),sc_time(90,SC_NS),sc_time(0,SC_NS),"task_2");
     t2.insertCode("fixed(30);");
     //PeriodicTask t5(sc_time(10,SC_NS),sc_time(10,SC_NS),sc_time(0,SC_NS),"task_D");
     //t5.insertCode("fixed(4);");
     //t2.set_affinity(0);
//
    //PeriodicTask t3(sc_time(40,SC_NS),sc_time(40,SC_NS),sc_time(0,SC_NS), "task2");
 //   t3.insertCode t7.insertCode("de("fixed(2);");
    //t3.insertCode("fixed(2);wait(R2);fixed(5);signal(R2);fixed(1);");
    // t3.insertCode("wait(R1);fixed(6);signal(R1);fixed(2);");
//    t3.setTrace(&jtrace);
     PeriodicTask t3(sc_time(140,SC_NS),sc_time(140,SC_NS),sc_time(0,SC_NS), "task_3");
     t3.insertCode("fixed(50);");
    //t3.set_affinity(1);
     //Five soft real time tasks
    PeriodicTask t4(sc_time(190,SC_NS),sc_time(190,SC_NS),sc_time(0,SC_NS), "task_4");
    t4.insertCode("fixed(30);");
    PeriodicTask t5(sc_time(350,SC_NS),sc_time(350,SC_NS),sc_time(0,SC_NS), "task_5");
    t5.insertCode("fixed(340);");
    PeriodicTask t6(sc_time(500,SC_NS),sc_time(500,SC_NS),sc_time(0,SC_NS), "task_6");
    t6.insertCode("fixed(170);");
    /*PeriodicTask t7(sc_time(1000,SC_NS),sc_time(1000,SC_NS),sc_time(0,SC_NS), "task_7");
    t7.insertCode("delay(unif(200,700));");
    //t7.insertCode("fixed(300);");
    PeriodicTask t8(sc_time(1300,SC_NS),sc_time(1300,SC_NS),sc_time(0,SC_NS), "task_8");
    t8.insertCode("delay(unif(500,900));");
    //Sporadic task with minimum interarrival time of 10 and a maximum interarrival time of 20
    Task t9(new UniformVar(1000,5000),sc_time(1500000,SC_NS),sc_time(0,SC_NS),sc_time(0,SC_NS), "task_9");
    t9.insertCode("fixed(200);");
    //t9.killOnMiss(false);
    Task t10(new UniformVar(3000,9000),sc_time(1500000,SC_NS),sc_time(0,SC_NS),sc_time(0,SC_NS), "task_10");
    t10.insertCode("fixed(500);");
    //t10.killOnMiss(false);
    Task t11(new UniformVar(5000,15000),sc_time(1500000,SC_NS),sc_time(0,SC_NS),sc_time(0,SC_NS), "task_11");
    t11.insertCode("fixed(1500);");*/
    //t11.killOnMiss(false);
    //t1.AddChildTask(&t2);
    //t1.AddChildTask(&t3);
    //t1.AddChildTask(&t5);
    //t4.AddParentTask(&t2);
    //t4.AddParentTask(&t3);
    //t4.set_affinity(1);
//
// This interrupt will activate task B. 
    // It has an interarrival time (between bursts of interrupts) that 
    // follows an exponential distribution with a mean of 20;
    // each burst generates 2 interrupts;
    // the interrupt minimum separation time is 5 
    // (see the trace for a more precise idea of how this interrupt is
    // distributed)
    //Interrupt inter2(new ExponentialVar(20), 5, new DeltaVar(2), "inter2");

    // This interrupt will activate task C.  It has an interarrival
    // time (between bursts of interrupts) that follows an exponential
    // distribution with a mean of 30; each time the burst generates a
    // random number of interrupts (btw 1 and 4); the minimum
    // separation time is 4.
    //Interrupt inter3(new ExponentialVar(30), 4, new UniformVar(1,4), "inter3");

    //inter2.addTask(&t2);
    //inter3.addTask(&t3);
    //t1.setAbort(true);
    //t2.setAbort(true);
    //t3.setAbort(true);
    //t4.killOnMiss(true);
    //t5.killOnMiss(true);
    //t6.killOnMiss(true);
    //t7.killOnMiss(true);
    //t8.killOnMiss(true);
    //t9.killOnMiss(true);
    kern.addTask(t1,"0");
    kern.addTask(t2,"1");
    kern.addTask(t3,"2");
    kern.addTask(t4,"3");
    kern.addTask(t5,"4");
    kern.addTask(t6,"5");
    //kern.addTask(t7,"7");
    //kern.addTask(t8,"6");
    //kern.addTask(t9,"8");
    //kern.addTask(t10,"9");
    //kern.addTask(t11,"10");
    //kern.addTask(t3,"5");
    //kern.addTask(t4,"4");
    //kern.addTask(t5,"4");
   

    //fp=sc_create_vcd_trace_file("task_state12");// Create wave.vcd file
//    kern.addTask(t4,"10");
   // sc_trace(fp,t1.task_exe,"t1.task_exe");             // Add signals to trace file
   // sc_trace(fp,t2.task_exe,"t2.task_exe");
   // sc_trace(fp,t3.task_exe,"t3.task_exe");
    //sc_trace(fp,t4.task_exe,"t4.task_exe");
    //sc_trace(fp,t5.task_exe,"t5.task_exe");
       //sc_trace(fp,dout,"dout");
    cout<<"before start"<<endl;
    //kern.setContextSwitchDelay(sc_time(2,SC_NS));
    sc_start(12000000,SC_NS);
    cout<<"terminated"<<endl;
  
     return 1;
}



