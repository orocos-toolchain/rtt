/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  DataSources.hpp

                        DataSources.hpp -  description
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


#ifndef RTT_INTERNAL_ACTIONALIASASSIGNABLEDATASOURCE_HPP
#define RTT_INTERNAL_ACTIONALIASASSIGNABLEDATASOURCE_HPP

#include "AssignableDataSource.hpp"

namespace RTT
{
    namespace internal {
            /**
             * An AssignableDataSource which is used to execute an action
             * and then return the value of another DataSource.
             * @param T The result data type of get().
             */
            template<typename T>
            class ActionAliasAssignableDataSource
                : public AssignableDataSource<T>
            {
                base::ActionInterface* action;
                typename AssignableDataSource<T>::shared_ptr alias;
            public:
                typedef boost::intrusive_ptr<ActionAliasDataSource<T> > shared_ptr;

                ActionAliasAssignableDataSource(base::ActionInterface* act, AssignableDataSource<T>* ds)
                : action(act), alias(ds)
                  {}

                ~ActionAliasAssignableDataSource() { delete action;}

                bool evaluate() const {
                    // since get() may return a copy, we override evaluate() to
                    // call alias->get() with alias->evaluate().
                    action->readArguments();
                    bool r = action->execute();
                    action->reset();
                    // alias may only be evaluated after action was executed.
                    alias->evaluate();
                    return r;
                }

                typename DataSource<T>::result_t get() const
                {
                    action->readArguments();
                    action->execute();
                    action->reset();
                    return alias->get();
                }

                typename DataSource<T>::result_t value() const
                {
                    return alias->value();
                }

                void set( typename AssignableDataSource<T>::param_t t ) {
                    alias->set( t );
                }

                typename AssignableDataSource<T>::reference_t set()
                {
                    return alias->set();
                }

                typename AssignableDataSource<T>::const_reference_t rvalue() const
                {
                    return alias->rvalue();
                }

                virtual void reset() { alias->reset(); }

                virtual ActionAliasAssignableDataSource<T>* clone() const {
                    return new ActionAliasAssignableDataSource(action, alias.get());
                }
                virtual ActionAliasAssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                    return new ActionAliasAssignableDataSource( action->copy(alreadyCloned), alias->copy(alreadyCloned) );
                }
            };
    }
}
#endif


