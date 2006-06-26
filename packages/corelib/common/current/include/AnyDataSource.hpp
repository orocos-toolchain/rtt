/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  AnyDataSource.hpp 

                        AnyDataSource.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_CORELIB_ANY_DATASOURCE_HPP
#define ORO_CORELIB_ANY_DATASOURCE_HPP

#include "DataSource.hpp"
#include "Logger.hpp"
#include <tao/Any.h>

namespace ORO_CoreLib
{
    namespace detail {
        /**
         * A DataSource which wraps another DataSource into a CORBA::Any.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_ptr>
        {
            DataSourceBase::const_ptr source;
        public:
            AnyDataSource( const DataSourceBase* orig )
                : source(orig)
            {}

            DataSource<CORBA::Any_ptr>::result_t get() const
            {
                Logger::log() << Logger::Debug << "AnyDataSource::get() returns Corba::Any of "<< source->getType() <<Logger::endl;
                return source->getAny();
            }

            DataSource<CORBA::Any_ptr>::result_t value() const
            {
                return source->createAny();
            }

            virtual AnyDataSource* clone() const 
            {
                return new AnyDataSource(source.get());
            }

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const 
            {
                return new AnyDataSource( source->copy( alreadyCloned ) );
            }
        };
    }
}

#endif
