################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
src/cpu.cpp \
src/dagnode.cpp \
src/edfsched.cpp \
src/exeinstr.cpp \
src/fcfsresmanager.cpp \
src/fifosched.cpp \
src/fpsched.cpp \
src/instr.cpp \
src/interrupt.cpp \
src/kernel.cpp \
src/mrtkernel.cpp \
src/npedf.cpp \
src/parti_mrtkernel.cpp \
src/partition_edf.cpp \
src/pfair_scheduler.cpp \
src/piresman.cpp \
src/randomvar.cpp \
src/reginstr.cpp \
src/regsched.cpp \
src/regtask.cpp \
src/regvar.cpp \
src/resmanager.cpp \
src/resource.cpp \
src/rrsched.cpp \
src/rttask.cpp \
src/schedinstr.cpp \
src/scheduler.cpp \
src/strtoken.cpp \
src/suspend_instr.cpp \
src/task.cpp \
src/taskevt.cpp \
src/threinstr.cpp \
src/waitinstr.cpp 

OBJS += \
obj/cpu.o \
obj/dagnode.o \
obj/edfsched.o \
obj/exeinstr.o \
obj/fcfsresmanager.o \
obj/fifosched.o \
obj/fpsched.o \
obj/instr.o \
obj/interrupt.o \
obj/kernel.o \
obj/mrtkernel.o \
obj/npedf.o \
obj/parti_mrtkernel.o \
obj/partition_edf.o \
obj/pfair_scheduler.o \
obj/piresman.o \
obj/randomvar.o \
obj/reginstr.o \
obj/regsched.o \
obj/regtask.o \
obj/regvar.o \
obj/resmanager.o \
obj/resource.o \
obj/rrsched.o \
obj/rttask.o \
obj/schedinstr.o \
obj/scheduler.o \
obj/strtoken.o \
obj/suspend_instr.o \
obj/task.o \
obj/taskevt.o \
obj/threinstr.o \
obj/waitinstr.o 

CPP_DEPS += \
obj/cpu.d \
obj/dagnode.d \
obj/edfsched.d \
obj/exeinstr.d \
obj/fcfsresmanager.d \
obj/fifosched.d \
obj/fpsched.d \
obj/instr.d \
obj/interrupt.d \
obj/kernel.d \
obj/mrtkernel.d \
obj/npedf.d \
obj/parti_mrtkernel.d \
obj/partition_edf.d \
obj/pfair_scheduler.d \
obj/piresman.d \
obj/randomvar.d \
obj/reginstr.d \
obj/regsched.d \
obj/regtask.d \
obj/regvar.d \
obj/resmanager.d \
obj/resource.d \
obj/rrsched.d \
obj/rttask.d \
obj/schedinstr.d \
obj/scheduler.d \
obj/strtoken.d \
obj/suspend_instr.d \
obj/task.d \
obj/taskevt.d \
obj/threinstr.d \
obj/waitinstr.d 


# Each subdirectory must supply rules for building sources it contributes
obj/%.o: src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/local/systemc-2.3.1/include -I"/media/sda5/huddy1985/project/systemc/SysRT/src" -O2 -g3 -Wall -c -fPIC -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


