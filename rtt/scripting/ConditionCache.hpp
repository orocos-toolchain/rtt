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
