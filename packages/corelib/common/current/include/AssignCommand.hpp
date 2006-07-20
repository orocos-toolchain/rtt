/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  AssignCommand.hpp 

                        AssignCommand.hpp -  description
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
 
 

#ifndef ORO_ASSIGNCOMMAND_HPP
#define ORO_ASSIGNCOMMAND_HPP

#include "CommandInterface.hpp"
#include "DataSource.hpp"

namespace RTT
{
    namespace detail {

        /**
         * This is a command that will assign the value of an expression to
         * another  at runtime.  You pass it the AssignableDataSource that
         * you want to assign to, and the DataSource that you want to assign
         * on construction, and it will take care of the assignment.  Note
         * that both DataSource's need to be of a convertible type, and this
         * class needs that type as its template parameter..
         * @param T Target type
         * @param S Source type
         */
        template<typename T, typename S = T>
        class AssignCommand
            : public CommandInterface
        {
        public:
            typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
            typedef typename DataSource<S>::const_ptr RHSSource;
        private:
            LHSSource lhs;
            RHSSource rhs;
        public:
            /**
             * Assign \a r (rvalue) to \a l (lvalue);
             */
            AssignCommand( LHSSource l, RHSSource r )
                : lhs( l ), rhs( r )
            {
            }

            void readArguments() {
                rhs->evaluate();
            }

            bool execute()
            {
                lhs->set( rhs->value() );
                return true;
            }

            virtual CommandInterface* clone() const
            {
                return new AssignCommand( lhs.get(), rhs.get() );
            }

            virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new AssignCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
            }
        };

        /**
         * Assign the contents of one variable-size container to another.
         * This class checks for capacity and fails execution if not sufficient.
         * @param T Target type
         * @param S Source type
         * @param APred The 'valid assignment' predicate which checks at runtime if the
         * assignment may be made.
         */
        template<typename T, typename APred, typename S = T>
        class AssignContainerCommand
            : public CommandInterface
        {
            typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
            LHSSource lhs;
            typedef typename DataSource<S>::const_ptr RHSSource;
            RHSSource rhs;
        public:
            AssignContainerCommand( LHSSource l, RHSSource r )
                : lhs( l ), rhs( r )
            {
            }

            void readArguments() {
                rhs->evaluate();
            }

            bool execute()
            {
                if ( APred()( lhs->get(), rhs->value()) == false )
                    return false;
                lhs->set( rhs->value() );
                return true;
            }

            virtual CommandInterface* clone() const
            {
                return new AssignContainerCommand( lhs.get(), rhs.get() );
            }

            virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new AssignContainerCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
            }
        };

        /**
         * This command assigns an element of a container to a given value.
         * @param T The Container's full type.
         * @param Index The Container's index type.
         * @param SetType The Container's value type.
         * @param Pred The value type to set the container's value type to.
         *
         */
        template<typename T, typename Index, typename SetType, typename Pred>
        class AssignIndexCommand
            : public CommandInterface
        {
            typedef typename DataSource<Index>::shared_ptr IndexSource;
            IndexSource i;
            typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
            LHSSource lhs;
            typedef typename DataSource<SetType>::shared_ptr RHSSource;
            RHSSource rhs;
        public:
            AssignIndexCommand( LHSSource l, IndexSource index, RHSSource r)
                : i(index),lhs( l ), rhs( r )
            {
            }

            void readArguments() {
                rhs->evaluate();
            }

            bool execute()
            {
                Index ind = i->get();
                if ( Pred()( lhs->get(), ind) && &(lhs->set()) != 0 ) {
                    lhs->set()[ ind ] = rhs->value();
                    lhs->updated();
                    return true;
                }
                return false;
            }

            virtual CommandInterface* clone() const
            {
                return new AssignIndexCommand( lhs.get(), i.get(), rhs.get() );
            }

            virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new AssignIndexCommand( lhs->copy( alreadyCloned ), i->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
            }
        };

#ifdef OROINT_OS_CORBA
        /**
         * This is a command that will assign the CORBA::Any value of an expression to
         * an AssignableDatasource. The command will fail if the any was not convertible
         * to \a T. This command is not hard real-time and mainly used for distributed communication.
         * @param T The type of the AssignableDataSource to assign the Any to.
         */
        template<class T>
        class AssignAnyCommand
            : public CommandInterface
        {
        public:
            typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
            typedef typename DataSourceBase::const_ptr RHSSource;
        private:
            LHSSource lhs;
            RHSSource rhs;
        public:
            /**
             * Assign \a r (rvalue) to \a l (lvalue);
             */
            AssignAnyCommand( LHSSource l, RHSSource r )
                : lhs( l ), rhs( r )
            {
            }

            void readArguments() {
                rhs->evaluate();
            }

            bool execute()
            {
                CORBA::Any_var any = rhs->createAny();
                return lhs->update( *any  );
            }

            virtual CommandInterface* clone() const
            {
                return new AssignAnyCommand( lhs.get(), rhs.get() );
            }

            virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new AssignAnyCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
            }
        };

#endif

    }
}

#endif

