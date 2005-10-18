
#ifndef CONDITIONINVERT_HPP
#define CONDITIONINVERT_HPP

#include "ConditionInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates to the inverse of another Condition.
     */
    class ConditionInvert
        : public ConditionInterface
    {
        ConditionInterface* mc;
    public:
        ConditionInvert(ConditionInterface* c)
            : mc(c)
        {
        }

        virtual ~ConditionInvert()
        {
            delete mc;
        }

        virtual bool evaluate()
        {
            return ! mc->evaluate();
        }

        virtual ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
            return new ConditionInvert( mc->copy(alreadyCloned) );
        }

        virtual ConditionInterface* clone() const
        {
            return new ConditionInvert( mc->clone() );
        }
    };

}

#endif
