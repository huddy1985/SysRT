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

#ifndef __INSTR_HPP__
#define __INSTR_HPP__

#include <baseexc.hpp>
#include <randomvar.hpp>
#include <taskevt.hpp>
#include <systemc.h>

#define _INSTR_DBG_LEV "Instruction"

    using namespace std;

    class Task;
    //class sc_object_manager;
    /**

       Exceptions for the instructions
    */
    class InstrExc: public BaseExc
    {
    public: 
        InstrExc(const string &msg, const string &cl)
            :BaseExc(msg, cl, "InstrExc") {}

    };

    /**
       The base class for every pseudo instruction. Pseudo-instructions
       represents the code that a task executes. An instruction is identified
       by an execution time (possibly random) and by a certain optional 
       functionality. 
       A task contains a list of instructions, that are executed in
       sequence.
       @see Task.
    */
    class Instr: public sc_module {
    protected:
        Task* _father;
    public:
        //static sc_object_manager* ob_manager;
        typedef string BASE_KEY_TYPE;
        Instr(Task *f, sc_module_name n = sc_gen_unique_name("Insturctions")) :sc_module(n), _father(f) {;}
        virtual ~Instr() {}
        /** 
         * Returns a ponter to that task which ownes this instruction.
         */ 
        Task* getTask() const {return _father;}
  
        /** 
         * Called when the instruction is scheduled.
         */
        virtual void schedule() = 0;
  
        /** 
         * Called when the instruction  is descheduled.
         */
        virtual void deschedule() = 0;
  
        /** 
         * Called upon the instruction end event
         */
        virtual void onEnd() {}

        /** 
            This method permits to kill a task which is currently
            executing. It resets the internal state of the executing
            instruction.

            This is currently used only by the fault-tolerant scheduling
            algorithms.
        */ 
        virtual void reset() = 0;

        /**
           Returns how long the instrucion has been executed from the last
           reset().

           NOTE: the resetExecdTime is now implicit!!!!
        */
        virtual sc_time getExecTime() const = 0;

        /**
           Returns the total computation time of the instruction
        */
        virtual sc_time getDuration() const = 0;

        /**
           Returns the worst-case execution time for this instruction.
        */
        virtual sc_time getWCET() const throw(RandomVar::MaxException) = 0;

        //virtual void setTrace(Trace*) = 0;

        // virtual methods from entity
        virtual void newRun() = 0;
        virtual void endRun() = 0;  

        /** 
            It refreshes the state of the executing instruction 
            when a change of the CPU speed occurs. 
        */ 
        virtual void refreshExec(double oldSpeed, double newSpeed) = 0;
    };

    /**
       End event for instructions
    */
    class EndInstrEvt : public sc_event {
        Instr *_instr;
    public:
        EndInstrEvt(Instr * in) : sc_event(), _instr(in) {}
        virtual Instr* getInstruction() const;        
    };


#endif
