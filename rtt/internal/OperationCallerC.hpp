/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  OperationCallerC.hpp

                        OperationCallerC.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef ORO_EXECUTION_METHODC_HPP
#define ORO_EXECUTION_METHODC_HPP

#include <string>
#include "DataSources.hpp"
#include "../Attribute.hpp"
#include "../rtt-fwd.hpp"
#include "../SendStatus.hpp"
#include "SendHandleC.hpp"

namespace RTT
{ namespace internal {

    /**
     * A template-less manager for OperationCaller calls.
     */
    class RTT_API OperationCallerC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;
        base::DataSourceBase::shared_ptr m;
        base::DataSourceBase::shared_ptr s;
        OperationInterfacePart* ofp;
        std::string mname;

    public:
        /**
         * The default constructor.
         * Make a copy from another OperationCallerC object
         * in order to make it usable.
         */
        OperationCallerC();

        /**
         * The constructor.
         * @see Service
         */
        OperationCallerC( OperationInterfacePart* mr, const std::string& name, ExecutionEngine* caller);

        /**
         * A OperationCallerC is copyable by value.
         */
        OperationCallerC(const OperationCallerC& other);

        /**
         * A OperationCallerC is copyable by value, with assignment of a new caller.
         */
        OperationCallerC(const OperationCallerC& other, ExecutionEngine* caller);

        /**
         * A OperationCallerC is assignable.
         */
        OperationCallerC& operator=(const OperationCallerC& other);

        ~OperationCallerC();

        /**
         * Add a datasource argument to the OperationCaller.
         * @param a A DataSource which contents are consulted each time
         * when call() is called.
         */
        OperationCallerC& arg( base::DataSourceBase::shared_ptr a );

        /**
         * Add a constant argument to the OperationCaller.
         * @param a A value of which a copy is made and this value is used each time
         * in call().
         */
        template< class ArgT >
        OperationCallerC& argC( const ArgT a )
        {
            return this->arg(base::DataSourceBase::shared_ptr( new ConstantDataSource<ArgT>( a ) ) );
        }

        /**
         * Add an argument by reference to the OperationCaller.
         * @param a A value of which the reference is used and re-read each time
         * the method is called. Thus if the contents of the source of \a a changes,
         * call() will use the new contents.
         */
        template< class ArgT >
        OperationCallerC& arg( ArgT& a )
        {
            return this->arg(base::DataSourceBase::shared_ptr( new ReferenceDataSource<ArgT>( a ) ) );
        }

        /**
         * Store the result of the method in a task's attribute.
         * @param r A task attribute in which the result is stored.
         */
        OperationCallerC& ret( base::AttributeBase* r );

        /**
         * Store the result of the method in a DataSource.
         * @param r A data source in which the result is stored.
         */
        OperationCallerC& ret( base::DataSourceBase::shared_ptr r);

        /**
         * Store the result of the method in variable.
         * @param r A reference to the variable in which the result is stored.
         */
        template< class RetT >
        OperationCallerC& ret( RetT& r )
        {
            // this is semantically valid wrt Attribute::copy().
            base::AttributeBase* ta = new Attribute<RetT>("ret", new ReferenceDataSource<RetT>(r));
            this->ret( ta );
            delete ta;
            return *this;
        }

        /**
         * Call the contained method.
         */
        bool call();

        /**
         * Send the contained method. The returned SendHandleC is properly
         * constructed, but still requires the .arg() arguments.
         * The arguments to provide are the ones that collect requires
         * for the sent operation. Once they have been added to the
         * SendHandleC, you can collect() on that object.
         */
        SendHandleC send();

        /**
         * Checks if this method is ready for calling, will throw
         * if not so. Otherwise, does nothing.
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         */
        void check();

        /**
         * Returns true if this method is ready for execution.
         */
        bool ready() const;

        /**
         * Returns the factory which we use to produce the operation call.
         */
        OperationInterfacePart* getOrp() const;

        /**
         * Returns the name of the operation that will be called.
         */
        std::string const& getName() const;

        /**
         * Get the contained data source for 'call'.
         */
        base::DataSourceBase::shared_ptr getCallDataSource();
        /**
         * Get the contained data source for 'send'.
         */
        base::DataSourceBase::shared_ptr getSendDataSource();
        /**
         * Get the contained data source for send handle.
         */
        base::DataSourceBase::shared_ptr getSendHandleDataSource();
    };
}}

#endif
