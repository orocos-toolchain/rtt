/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  AssignCommand.hpp

                        AssignCommand.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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

#include "DataSource.hpp"

#ifndef ORO_ASSIGNCOMMAND_HPP
#define ORO_ASSIGNCOMMAND_HPP

#include "../base/ActionInterface.hpp"

namespace RTT
{
    namespace internal {

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
            : public base::ActionInterface
        {
        public:
            typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
            typedef typename DataSource<S>::shared_ptr RHSSource;
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
            }

            bool execute()
            {
                // we ignore evaluate, which return value is legacy
                // we always assign the current state of rhs
                rhs->evaluate();
                lhs->set( rhs->rvalue() );
                return true;
            }

            void reset() {
                rhs->reset();
            }

            virtual base::ActionInterface* clone() const
            {
                return new AssignCommand( lhs.get(), rhs.get() );
            }

            virtual base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                return new AssignCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
            }
        };
    }
}

#endif

