/***************************************************************************
  tag: Peter Soetens  Mon Jun 10 14:43:20 CEST 2002  ComponentOneShot.cpp 

                        ComponentOneShot.cpp -  description
                           -------------------
    begin                : Mon June 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#include "corelib/ComponentOneShot.hpp"
#include <rtai_cpp/iostream.h>
namespace ORO_OS {

/**
 * This wraps the better written RTAI::Task class by Erwin Rol
 */

int ComponentOneShot::run() 
{
    rtos_printf("One Shot Component created...\n");
    if (runComp !=0)
        runComp->step();
    else
        step();
    rtos_printf("Breaked : Killing myself\n");
    return 0;
}


    

ComponentOneShot::ComponentOneShot(RunnableInterface* r=0) 
     :Task(4096,5), // STACK, PRIORITY
      runComp(r)
{
    rtos_printf("ComponentOneShot Constructor\n");
}

#define POLL_FREQ 50

ComponentOneShot::~ComponentOneShot() 
{
    rtos_printf("Waiting in destructor\n");
/*
    rt_task_struct* current = rt_whoami();
    while (rt_sem_wait_if(&tSem) < 0) 
    {
        current->state = TASK_INTERRUPTIBLE; // ???
        schedule_timeout(jiffies + (HZ + POLL_FREQ/2 -1)/POLL_FREQ);
    }
  */  
    //rt_busy_sleep(10000000); // XXX this is as in RTAI examples
    
    rtos_printf("ComponentOneShot destroyed\n");
    
}

int ComponentOneShot::start() 
{
    resume();
    return 0;
}



void ComponentOneShot::taskNameSet(const char* nm)
{
    sprintf(taskName,"%s",nm);
}

char* ComponentOneShot::taskNameGet()
{
    return taskName;
}
   
}
    
