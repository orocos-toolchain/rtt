/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  SyncWriter.hpp 

                        SyncWriter.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef SYNCWRITER_HPP
#define SYNCWRITER_HPP

#include <corelib/RunnableInterface.hpp>

namespace CAN
{
    /**
     * A class which writes a sync message to the bus when
     * it is run.
     */
    class SyncWriter 
        : public ORO_CoreLib::RunnableInterface 
    {
        CANOpenBus* bus;
        TaskInterface* _t;
    public:
        SyncWriter( CANOpenBus* _bus) : bus(_bus) {}
        bool initialize() { return true; }
        void step() { bus->sync(); }
        void finalize() {}
        virtual TaskInterface* getTask() const {
            return _t;
        }
        virtual void setTask( TaskInterface* task ) {
            _t = task;
        }
    };
}

#endif
