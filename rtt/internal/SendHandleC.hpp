/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SendHandleC.hpp

                        SendHandleC.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_EXECUTION_SENDHANDLEC_HPP
#define ORO_EXECUTION_SENDHANDLEC_HPP

#include <string>
#include <boost/make_shared.hpp>
#include "DataSources.hpp"
#include "../Attribute.hpp"
#include "../rtt-fwd.hpp"
#include "../SendStatus.hpp"

namespace RTT
{ namespace internal {

    /**
     * A template-less SendHandle manager.
     * It is created using a SendHandle data source, obtained from
     * a OperationFactoryPart::produceSend(), the part itself and
     * any arguments that must be provided to collect the results.
     *
     * It is used by OperationCallerC to pass on responsibility to collect
     * results to a separate object.
     */
    class RTT_API SendHandleC
    {
        /**
         * The 'd' pointer pattern.
         * It's hiding our factory code.
         */
        class D;
        D* d;
        /**
         * This data source will do a collect/collectIfDone when
         * being evaluated().
         */
        internal::DataSource<SendStatus>::shared_ptr s;
        /**
         * Stores the blocking/non blocking flag for collect/collectIfDone.
         */
        internal::AssignableDataSource<bool>::shared_ptr b;

	/**
	 * Stores the OperationInterfacePart pointer contained in this SendHandle
	 */
	OperationInterfacePart* orp;


    public:
        /**
         * The default constructor.
         * Make a copy from another SendHandleC object
         * in order to make it usable.
         */
        SendHandleC();

        /**
         * The constructor from a SendHandle data source and an operation part.
         * The SendHandle is obtained after a send.
         */
        SendHandleC( base::DataSourceBase::shared_ptr handle, OperationInterfacePart* ofp, const std::string& name );

        /**
         * A SendHandleC is copyable by value.
         */
        SendHandleC(const SendHandleC& other);

        /**
         * A SendHandleC is assignable.
         */
        SendHandleC& operator=(const SendHandleC& other);

        ~SendHandleC();

        /**
         * Add a datasource argument to the SendHandle.
         * @param a A DataSource which contents are filled in in case
         * of collect() or collectIfDone()
         */
        SendHandleC& arg( base::DataSourceBase::shared_ptr a );

        /**
         * Add an argument by reference to the OperationCaller.
         * @param a A value of which the reference is used and re-read each time
         * the method is called. Thus if the contents of the source of \a a changes,
         * call() will use the new contents.
         */
        template< class ArgT >
        SendHandleC& arg( ArgT& a )
        {
            return this->arg(base::DataSourceBase::shared_ptr( new ReferenceDataSource<ArgT>( a ) ) );
        }

        /**
         * Collect the contained method.
         */
        SendStatus collect();

        /**
         * Collect the contained method.
         */
        SendStatus collectIfDone();

        /**
         * Checks if this handle is ready for collecting, will throw
         * if not so. Otherwise, does nothing.
         * @throw invalid_handle_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         */
        void check();

        /**
         * Returns true if this handle is ready for execution.
         */
        bool ready() const;

        /**
         * Get the contained data source for send handle.
         */
        base::DataSourceBase::shared_ptr getSendHandleDataSource();

	/**
	 * Get the contained OperationInterfacePart for SendHandle
	 */
	OperationInterfacePart* getOrp();

    };
}}

#endif
