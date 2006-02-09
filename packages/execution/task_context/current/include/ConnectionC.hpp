/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ConnectionC.hpp 

                        ConnectionC.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_EXECUTION_CONNECTIONC_HPP
#define ORO_EXECUTION_CONNECTIONC_HPP

#include <string>
#include "DataSource.hpp"
#include <corelib/Handle.hpp>

namespace ORO_CoreLib {
    class EventProcessor;
}

namespace ORO_Execution
{
    class EventService;

    /**
     * A user friendly callback connection to an event of a TaskContext.
     */
    class ConnectionC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;
        ORO_CoreLib::Handle h;
    public:
        /**
         * The default constructor.
         * Make a copy from another ConnectionC object
         * in order to make it usable.
         */
        ConnectionC();
        /**
         * The constructor.
         * @see EventService
         */
        ConnectionC( const EventService* gcf, const std::string& name,
                     boost::function<void(void)> func, ORO_CoreLib::EventProcessor* ep );

        /**
         * A ConnectionC is copyable by value.
         */
        ConnectionC(const ConnectionC& other);

        /**
         * A ConnectionC is assignable;
         */
        ConnectionC& operator=(const ConnectionC& other);

        ~ConnectionC();

        /**
         * Add a datasource argument to the Connection.
         */
        ConnectionC& arg( ORO_CoreLib::DataSourceBase::shared_ptr a );

        /**
         * Add a reference argument to the Connection.
         */
        template< class ArgT >
        ConnectionC& arg( ArgT& a )
        {
            return this->arg(DataSourceBase::shared_ptr( new ReferenceDataSource<ArgT>( a ) ) );
        }

        /**
         * Get a Handle to the connection.
         */
        ORO_CoreLib::Handle handle() const;
    };
}

#endif
