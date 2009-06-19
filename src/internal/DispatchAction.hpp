/***************************************************************************
  tag: FMTC  do nov 2 13:06:04 CET 2006  DispatchAction.hpp

                        DispatchAction.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_DISPATCH_ACTION_HPP
#define ORO_DISPATCH_ACTION_HPP

#include "DispatchInterface.hpp"

namespace RTT
{
    /**
     * An action which invokes a dispatch command
     * in execute().
     */
    class DispatchAction
        : public ActionInterface
    {
        DispatchInterface*  mdi;
    public:
        DispatchAction(DispatchInterface* di)
            : mdi(di)
        {}

        ~DispatchAction() {
            delete mdi;
        }

        void reset() { mdi->reset(); }

        bool execute() {
            // if the command was not dispatched yet,
            // do this and return the result.
            if ( !mdi->sent() )
                return mdi->dispatch();
            // if it was dispatched and already executed,
            // return the result of the command function
            if ( mdi->executed() )
                return mdi->valid();
            // in the intermediate state, just return the
            // value of the dispatch().
            return mdi->accepted();
        }

        bool valid() const { return mdi->valid(); }

        void readArguments() { mdi->readArguments(); }

        virtual DispatchAction* clone() const {
            return new DispatchAction( mdi->clone() );
        }

        virtual DispatchAction* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new DispatchAction( mdi->copy(alreadyCloned));
        }

    };

}

#endif
