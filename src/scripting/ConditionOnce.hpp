/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionOnce.hpp

                        ConditionOnce.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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


#ifndef CONDITIONONCE_HPP
#define CONDITIONONCE_HPP

#include "../base/ConditionInterface.hpp"
#include "rtt-config.h"

namespace RTT
{ namespace scripting {


    /**
     * A conditional that evaluates the first time true
     * and afterwards always false (or vice versa).
     */
    class RTT_API ConditionOnce
        : public base::ConditionInterface
    {

    public:
        /**
         * Create a condition which will return once \a true
         * or once \a false.
         * @param what supply \a false it must return once false and
         * supply \a true if it must return once true.
         */
        ConditionOnce(bool what);

        virtual ~ConditionOnce();

        virtual bool evaluate();

        virtual void reset();

        base::ConditionInterface* clone() const;

    private:
        /**
         * Becomes true after one evaluation after its reset()
         * method has been called.
         */
        bool ran;
        bool _what;
    };

}}

#endif
