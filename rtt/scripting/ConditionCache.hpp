/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ConditionCache.hpp

                        ConditionCache.hpp -  description
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


#ifndef CONDITION_CACHE_HPP
#define CONDITION_CACHE_HPP

#include "ConditionInterface.hpp"
#include "../internal/DataSources.hpp"

namespace RTT
{ namespace scripting {


    /**
     * A conditional that evaluates and caches another Condition.
     * In order to read the cached value, use ConditionBoolDataSource.
     * By default, and after a reset, the cached condition returned in getResult() returns false.
     */
    class RTT_SCRIPTING_API ConditionCache
        : public ConditionInterface
    {
        boost::shared_ptr<ConditionInterface> mc;
        internal::AssignableDataSource<bool>::shared_ptr result;
    public:
        ConditionCache( ConditionInterface* c, internal::AssignableDataSource<bool>::shared_ptr ds = internal::AssignableDataSource<bool>::shared_ptr() )
            : mc(c), result( ds ? ds : new internal::ValueDataSource<bool>(false) )
        {
        }

        virtual ~ConditionCache()
        {
        }

        virtual bool evaluate()
        {
            result->set( mc->evaluate() );
            return result->value();
        }

        virtual void reset() {
            result->set( false );
        }

        internal::DataSource<bool>::shared_ptr getResult() { return result; }

        virtual ConditionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        {
            return new ConditionCache( mc->copy(alreadyCloned), result->copy(alreadyCloned) );
        }

        /**
         * A clone will cache the same condition.
         */
        virtual ConditionInterface* clone() const
        {
            return new ConditionCache( mc->clone(), result );
        }
    };
}}

#endif
