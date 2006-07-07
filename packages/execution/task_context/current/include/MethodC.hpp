/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  MethodC.hpp 

                        MethodC.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_METHODC_HPP
#define ORO_EXECUTION_METHODC_HPP

#include <string>
#include <corelib/DataSources.hpp>
#include <corelib/Attribute.hpp>

namespace ORO_Execution
{
    class GlobalMemberFactory;
    class MethodRepository;

    /**
     * A user friendly method to a TaskContext.
     */
    class MethodC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;
        ORO_CoreLib::DataSourceBase::shared_ptr m;
    public:
        /**
         * The default constructor.
         * Make a copy from another MethodC object
         * in order to make it usable.
         */
        MethodC();

        /**
         * The constructor.
         * @see GlobalMethodFactory
         */
        MethodC( const GlobalMemberFactory* gcf, const std::string& obj, const std::string& name);

        /**
         * The constructor.
         * @see MethodRepository
         */
        MethodC( const MethodRepository* mr, const std::string& name);

        /**
         * A MethodC is copyable by value.
         */
        MethodC(const MethodC& other);

        /**
         * A MethodC is assignable.
         */
        MethodC& operator=(const MethodC& other);

        ~MethodC();

        /**
         * Add a datasource argument to the Method.
         * @param a A DataSource which contents are consulted each time
         * when execute() is called.
         */
        MethodC& arg( ORO_CoreLib::DataSourceBase::shared_ptr a );

        /**
         * Add a constant argument to the Method.
         * @param a A value of which a copy is made and this value is used each time
         * in execute().
         */
        template< class ArgT >
        MethodC& argC( const ArgT a )
        {
            return this->arg(ORO_CoreLib::DataSourceBase::shared_ptr( new ORO_CoreLib::ConstantDataSource<ArgT>( a ) ) );
        }

        /**
         * Add an argument by reference to the Method.
         * @param a A value of which the reference is used and re-read each time
         * the method is executed. Thus if the contents of the source of \a a changes,
         * execute() will use the new contents.
         */
        template< class ArgT >
        MethodC& arg( ArgT& a )
        {
            return this->arg(ORO_CoreLib::DataSourceBase::shared_ptr( new ORO_CoreLib::ReferenceDataSource<ArgT>( a ) ) );
        }

        /**
         * Store the result of the method in a task's attribute.
         * @param r A task attribute in which the result is stored.
         */
        MethodC& ret( ORO_CoreLib::AttributeBase* r );

        /**
         * Store the result of the method in variable.
         * @param r A reference to the variable in which the result is stored.
         */
        template< class RetT >
        MethodC& ret( RetT& r )
        {
            // this is semantically valid wrt Attribute::copy().
            ORO_CoreLib::AttributeBase* ta = new ORO_CoreLib::Attribute<RetT>( new ORO_CoreLib::ReferenceDataSource<RetT>(r));
            this->ret( ta );
            delete ta;
            return *this;
        }

        /**
         * Execute the contained method.
         */
        bool execute();

        /**
         * Reset the method.
         * Required before invoking execute() a second time.
         */
        void reset();
    };
}

#endif
