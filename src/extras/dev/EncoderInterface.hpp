/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  EncoderInterface.hpp

                        EncoderInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef ENCODERINTERFACE_HPP
#define ENCODERINTERFACE_HPP

#include "../NameServer.hpp"
#include "../NameServerRegistrator.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    /**
     * @brief The most generic interface to a position/turn counter,
     * for encoding positions.
     *
     * Within one turn, there are
     * resolution() position increments. After a turn-increment,
     * the position will be exactly zero or resolution(). An
     * upcounting() == true encoder will count from zero to
     * resolution() between two turn increments. An upcounting() == false
     * encoder will count from resolution() to zero between two turn
     * increments.
     * @ingroup DeviceInterface
     */
    class EncoderInterface
        : private NameServerRegistrator<EncoderInterface*>
    {
    public:
        /**
         * @brief Create a EncoderInterface with a name.
         *
         * When \a name is not "", and unique, it can be retrieved
         * through EncoderInterface::nameserver .
         */
        EncoderInterface( const std::string& name )
            : NameServerRegistrator<EncoderInterface*>( nameserver, name, this )
        { }

        /**
         * @brief Create a not nameserverd EncoderInterface instance.
         */
        EncoderInterface()
        { }

        /**
         * The NameServer of this interface.
         * @see NameServer
         */
        static NameServer<EncoderInterface*> nameserver;

        virtual ~EncoderInterface() {}

        /**
         * @brief Get the position within the current turn.
         */
        virtual int positionGet() const = 0;

        /**
         * @brief Get the current turn.
         */
        virtual int turnGet() const = 0;

        /**
         * @brief Set the position within the current turn.
         */
        virtual void positionSet( int p) = 0;

        /**
         * @brief Set the current turn.
         */
        virtual void turnSet( int t ) = 0;

        /**
         * @brief Return the position resolution. This number
         * can be negative or positive and denotes the
         * the maximal or minimal value positionGet().
         */
        virtual int resolution() const = 0;

        /**
         * @brief Returns true if after a positive turn increment,
         * position increments positively.
         *
         * Meaning from 0 to |resolution()| or from
         * resolution() to zero if resolution() < 0
         */
        virtual bool upcounting() const = 0;
    };
}

#endif
