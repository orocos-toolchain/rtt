/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  StreamProcessor.hpp 

                        StreamProcessor.hpp -  description
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
 
 
#ifndef STREAMPROCESSOR_HPP
#define STREAMPROCESSOR_HPP

namespace ORO_CoreLib
{

    /**
     * An interface for setting and getting a
     * stream object. This can be a file, a string buffer,...
     */
    template<class Stream>
    class StreamProcessor
    {
        public:
        /**
         * Create a StreamProcessor.
         * @param _s The stream to be processed.
         */
        StreamProcessor( Stream& _s )
            :s(&_s)
        {}
        
        /**
         * Set a new Stream.
         * @param _s The stream to be processed.
         */
        void setStream( Stream& _s)
        {
            s = &_s;
        }

        /**
         * Get the current Stream
         * @return The current Stream.
         */
        Stream& getStream()
        {
            return *s;
        }
        
        protected:
        Stream* s;
    };

}

#endif
