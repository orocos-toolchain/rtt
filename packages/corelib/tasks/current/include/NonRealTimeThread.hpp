/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  NonRealTimeThread.hpp 

                        NonRealTimeThread.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 

#ifndef NONREALTIMETHREAD_HPP
#define NONREALTIMETHREAD_HPP

#include "TimerThread.hpp"

namespace ORO_CoreLib
{

    /**
     * It is used to run periodically TaskNonRealTime instances, which
     * will try to meet deadlines 'as good as possible'.
     */
    class NonRealTimeThread 
        : public TimerThread
    {

        public:
        /**
         * Returns the unique (singleton) NonRealTimeThread.
         */
        static NonRealTimeThread* Instance();

        /**
         * Releases the NonRealTimeThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        protected:

        /**
         * Default Constructor
         */
        NonRealTimeThread();

        private:
        /**
         * Singleton
         */    
        static NonRealTimeThread* cp;

        virtual ~NonRealTimeThread();

    };
}

#endif
