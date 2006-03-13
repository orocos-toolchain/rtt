/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  CommandC.hpp 

                        CommandC.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_COMMANDC_HPP
#define ORO_EXECUTION_COMMANDC_HPP

#include <string>
#include "DataSource.hpp"
#include <utility>

namespace ORO_CoreLib
{
    class CommandInterface;
    class ConditionInterface;
}

namespace ORO_Execution
{
    class GlobalCommandFactory;

    /**
     * A user friendly command to a TaskContext.
     */
    class CommandC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;
        std::pair<ORO_CoreLib::CommandInterface*,ORO_CoreLib::ConditionInterface*> cc;
    public:
        /**
         * The default constructor
         * Make a copy from another CommandC object
         * in order to make it usable.
         */
        CommandC();

        /**
         * The constructor.
         * @see GlobalCommandFactory
         */
        CommandC( const GlobalCommandFactory* gcf, const std::string& obj, const std::string& name, bool asyn);

        /**
         * A CommandC is copyable by value.
         */
        CommandC(const CommandC& other);

        /**
         * A CommandC is assignable.
         */
        CommandC& operator=( const CommandC& other );

        ~CommandC();

        /**
         * Add a datasource argument to the Command.
         * @param a A DataSource which contents are consulted each time
         * when execute() is called.
         */
        CommandC& arg( DataSourceBase::shared_ptr a );

        /**
         * Add a constant argument to the Command.
         * @param a A value of which a copy is made and this value is used each time
         * in execute().
         */
        template< class ArgT >
        CommandC& argC( const ArgT a )
        {
            return this->arg(DataSourceBase::shared_ptr( new ConstantDataSource<ArgT>( a ) ) );
        }

        /**
         * Add an argument by reference to the Command.
         * @param a A value of which the reference is used and re-read each time
         * the command is executed. Thus if the contents of the source of \a a changes,
         * execute() will use the new contents.
         */
        template< class ArgT >
        CommandC& arg( ArgT& a )
        {
            return this->arg(DataSourceBase::shared_ptr( new ReferenceDataSource<ArgT&>( a ) ) );
        }

        /**
         * Execute the contained command.
         */
        bool execute();

        /**
         * Evaluate if the command is done.
         * @retval true if accepted(), valid() was true and the
         * completion condition was true as well.
         * @retval false otherwise.
         */
        bool evaluate();

        /**
         * Check if the Command was accepted by the receiving task.
         * @retval true if accepted by the command processor.
         * @retval false otherwise.
         */
        bool accepted();

        /**
         * Check the return value of the Command when it is
         * executed.
         * @retval true if accepted() and the command returned true,
         * indicating that it is valid.
         * @retval false otherwise.
         */
        bool valid();

        /**
         * Reset the command.
         * Required before invoking execute() a second time.
         */
        void reset();
    };
}

#endif
