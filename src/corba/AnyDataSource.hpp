/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:24 CET 2008  AnyDataSource.hpp

                        AnyDataSource.hpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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


#ifndef ORO_CORBA_ANYDATASOURCE_HPP
#define ORO_CORBA_ANYDATASOURCE_HPP

#include "DataSource.hpp"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/Version.h>
#if TAO_MAJOR_VERSION == 1 && TAO_MINOR_VERSION <= 4
#include <tao/Any.h>
#else // TAO 1.5 and higher
#include <tao/AnyTypeCode/Any.h>
#endif
#endif

namespace RTT
{
    namespace Corba
    {
        /**
         * A DataSource which holds an any value and
         * returns it in its get() method. It can not be changed after creation.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_var>
        {
            /**
             * The any.
             */
            CORBA::Any_var mdata;

        public:
            /**
             * Use shared_ptr.
             */
            ~AnyDataSource();

            typedef boost::intrusive_ptr<AnyDataSource> shared_ptr;

            AnyDataSource( CORBA::Any_ptr value );

            DataSource<CORBA::Any_var>::result_t get() const;

            DataSource<CORBA::Any_var>::result_t value() const;

            virtual AnyDataSource* clone() const;

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;

            virtual int serverProtocol() const;

        };
    }
}
#endif

