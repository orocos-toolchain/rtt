/***************************************************************************
  tag: Klaas Gadeyne Thu Sep 21 17:29:27 CEST 2006 PulseTrainGeneratorInterface.hpp 

                        PulseTrainGeneratorInterface.hpp -  description
                           -------------------
    begin                : Thu Sep 21 17:29:27 CEST 2006
    copyright            : (C) 2006 FMTC
    email                : klaas gadeyne at fmtc be
 
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
 
#ifndef PULSETRAINGENERATOR_HPP
#define PULSETRAINGENERATOR_HPP

#include "../NameServer.hpp"
#include "../NameServerRegistrator.hpp"
#include "../Time.hpp" // Orocos Time types

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    /**
     * @brief A generic interface to a pulsetraingenerator for
     * simulation of encoder like behaviour.
     *
     */
    class PulseTrainGeneratorInterface
        : private NameServerRegistrator<PulseTrainGeneratorInterface*>
    {
    public:
        /**
         * @brief Create a PulseTrainGeneratorInterface with a name.
         *
         * When \a name is not "", and unique, it can be retrieved
         * through PulseTrainGeneratorInterface::nameserver .
         */
        PulseTrainGeneratorInterface( const std::string& name )
            : NameServerRegistrator<PulseTrainGeneratorInterface*>( nameserver, name, this )
        { }

        /**
         * @brief Create a not nameserved PulseTrainGeneratorInterface instance.
         */
        PulseTrainGeneratorInterface()
        { }

        /**
         * The NameServer of this interface.
         * @see NameServer
         */
        static NameServer<PulseTrainGeneratorInterface*> nameserver;

        virtual ~PulseTrainGeneratorInterface() {}

        /**
         * @brief Set Pulse Width
	 * @param picos Pulse Width expressed in (integer) picoseconds
	 * @return true when succeeded
	 * @pre PulseWidth should be smaller than PulsePeriod
         */
        virtual bool pulseWidthSet(psecs picos) = 0;

        /**
         * @brief Set Pulse Period
	 * @param picos Pulse Period expressed in (integer) picoseconds
	 * @return true when succeeded
	 * @pre PulseWidth should be smaller than PulsePeriod
         */
        virtual bool pulsePeriodSet(psecs picos) = 0;
           
        /**
         * @brief Start the pulse train generation.
         */
        virtual bool start() = 0;

        /**
         * @brief Stop the pulse train generation.
         */
        virtual bool stop() = 0;
    };
}

#endif
/***************************************************************************
  tag: Klaas Gadeyne Thu Sep 21 17:29:27 CEST 2006 PulseTrainGeneratorInterface.hpp 

                        PulseTrainGeneratorInterface.hpp -  description
                           -------------------
    begin                : Thu Sep 21 17:29:27 CEST 2006
    copyright            : (C) 2006 FMTC
    email                : klaas gadeyne at fmtc be
 
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
 
#ifndef PULSETRAINGENERATOR_HPP
#define PULSETRAINGENERATOR_HPP

#include "../NameServer.hpp"
#include "../NameServerRegistrator.hpp"
#include "../Time.hpp" // Orocos Time types

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    /**
     * @brief A generic interface to a pulsetraingenerator for
     * simulation of encoder like behaviour.
     *
     */
    class PulseTrainGeneratorInterface
        : private NameServerRegistrator<PulseTrainGeneratorInterface*>
    {
    public:
        /**
         * @brief Create a PulseTrainGeneratorInterface with a name.
         *
         * When \a name is not "", and unique, it can be retrieved
         * through PulseTrainGeneratorInterface::nameserver .
         */
        PulseTrainGeneratorInterface( const std::string& name )
            : NameServerRegistrator<PulseTrainGeneratorInterface*>( nameserver, name, this )
        { }

        /**
         * @brief Create a not nameserved PulseTrainGeneratorInterface instance.
         */
        PulseTrainGeneratorInterface()
        { }

        /**
         * The NameServer of this interface.
         * @see NameServer
         */
        static NameServer<PulseTrainGeneratorInterface*> nameserver;

        virtual ~PulseTrainGeneratorInterface() {}

        /**
         * @brief Set Pulse Width
	 * @param picos Pulse Width expressed in (integer) picoseconds
	 * @return true when succeeded
	 * @pre PulseWidth should be smaller than PulsePeriod
         */
        virtual bool pulseWidthSet(psecs picos) = 0;

        /**
         * @brief Set Pulse Period
	 * @param picos Pulse Period expressed in (integer) picoseconds
	 * @return true when succeeded
	 * @pre PulseWidth should be smaller than PulsePeriod
         */
        virtual bool pulsePeriodSet(psecs picos) = 0;
           
        /**
         * @brief Start the pulse train generation.
         */
        virtual bool start() = 0;

        /**
         * @brief Stop the pulse train generation.
         */
        virtual bool stop() = 0;
    };
}

#endif
