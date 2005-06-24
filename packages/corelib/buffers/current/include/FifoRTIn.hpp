/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  FifoRTIn.hpp 

                        FifoRTIn.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#ifndef FIFORTIN_HPP
#define FIFORTIN_HPP

#include "FifoRTCommon.hpp"

#include "Event.hpp"

namespace ORO_CoreLib
{

    /**
     * An input fifo that can be used in real time space.
     * Care must be taken that no 2 FifoRTin objects are constructed with
     * the same fifonumber. XXX (a next generation implementation can possibly
     * check for this situation).
     * @see ObservableReadInterface for more information on registering
     *      a listener.
     */
    class FifoRTIn
    {

        public:
            /**
             * Create an input fifo with a size of \a bytes bytes and
             * associated with fifo-number \a fifoNr.
            * When deleted, it will destroy the fifo
            *
            * @post  when there already existed a fifo with nr fifoNr before
            *      this constructor was called, no new fifo is created
             */
            FifoRTIn( unsigned int fifoNr, size_t bytes );

            /**
             * Wrap an input fifo around an already created rtosf 
             * (associated with fifo-number \a fifoNr).
             * When this object is deleted, it won't destroy the fifo.
             */
            FifoRTIn( unsigned int fifoNr );

            /**
             * Destroy (wrapper) fifo.
             */
            ~FifoRTIn();

            /**
             * Read from the fifo.
             */
            int read( char* buf, size_t length );
            
            int get( char& c);

            /**
             * Return the fifo number.
             */
            unsigned int fifoNr() const;

            //static int getFreeFifoNr();

        /**
         * This event is fired if data comes in the fifo.
         * The event's argument is the fifo number.
         */
        Event<void(int)>* getEvent() const;
        protected:
            // forbidden
            FifoRTIn();

            /**
             * When using rtosf, use this handler to signal listeners.
             */
            static int fifoHandler( unsigned int fifo, int rw );

        private:
        unsigned int fifo;
        bool created;
        unsigned int bytesReady; 
    };

} // namespace

#endif // FIFORTIN_HPP

