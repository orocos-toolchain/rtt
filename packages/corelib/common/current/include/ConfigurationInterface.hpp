/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  ConfigurationInterface.hpp 

                        ConfigurationInterface.hpp -  description
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
 
#ifndef CONFIGURATIONINTERFACE_HPP
#define CONFIGURATIONINTERFACE_HPP

namespace ORO_CoreLib
{
    /**
     * @brief Generic run-time class configuration interface.
     *
     * An Interface for configuring an object through
     * an external object, iterating over the methods
     * of this interface.
     */
    class ConfigurationInterface
    {
        public:
        virtual ~ConfigurationInterface() {}

        /**
         * Initialize and startup a configuration of
         * an object.
         */
        virtual void configInit() = 0;
            
        /**
         * A stepwise configuration of the object.
         * returns false if it fatally failed.
         */
        virtual bool configStep() = 0;

        /**
         * Returns if the configuration is finished.
         * The configuration is only finished if it was successful.
         */
        virtual bool isFinished() const = 0;

        /**
         * Perform some cleanup activities of the
         * configuration.
         */
        virtual void configCleanup() = 0;
    };




}

#endif
