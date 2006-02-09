/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  EventC.hpp 

                        EventC.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_EVENTC_HPP
#define ORO_EXECUTION_EVENTC_HPP

#include <string>
#include "DataSource.hpp"

namespace ORO_Execution
{
    class EventService;

    /**
     * A user friendly event of a TaskContext.
     */
    class EventC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;
        DataSourceBase::shared_ptr m;
    public:
        /**
         * Default constructor.
         * Use assignment/copy construction to make the default
         * EventC meaningful.
         */
        EventC();

        /**
         * The constructor.
         * @see EventService
         */
        EventC( const EventService* gcf, const std::string& name );

        /**
         * A EventC is copyable by value.
         */
        EventC(const EventC& other);

        /**
         * A EventC is assignable.
         */
        EventC& operator=(const EventC& other);

        ~EventC();

        /**
         * Add a datasource argument to the Event.
         * @param a A DataSource which contents are consulted each time
         * when emit() is called.
         */
        EventC& arg( DataSourceBase::shared_ptr a );

        /**
         * Add a constant argument to the Event.
         * @param a A value of which a copy is made and this value is used each time
         * in emit().
         */
        template< class ArgT >
        EventC& argC( const ArgT a )
        {
            return this->arg( DataSourceBase::shared_ptr(new ConstantDataSource<ArgT>( a )) );
        }

        /**
         * Add an argument by reference to the Event.
         * @param a A value of which the reference is used and re-read each time
         * the event is emitted. Thus if the contents of the source of \a a changes,
         * the emit() will use the new contents.
         */
        template< class ArgT >
        EventC& arg( ArgT& a )
        {
            return this->arg( DataSourceBase::shared_ptr(new ReferenceDataSource<ArgT>( a )) );
        }

        /**
         * emit the contained event.
         */
        void emit();
    };
}

#endif
