#include "execution/CommandExecFunction.hpp"

namespace ORO_Execution
{

    namespace {
        struct ValidFCondition
            : public ORO_CoreLib::ConditionInterface
        {
            DataSource<bool>::shared_ptr _a;
        public:
            ValidFCondition( DataSource<bool>* a)
                : _a( a )
            {}

            bool evaluate()
            {
                return _a->get();
            }

            ORO_CoreLib::ConditionInterface* clone() const
            {
                return new ValidFCondition( _a.get() );
            }

            ORO_CoreLib::ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
            {
                return new ValidFCondition( _a->copy( alreadyCloned ) );
            }
        };
    }

    ORO_CoreLib::ConditionInterface* CommandExecFunction::createValidCondition() const
    {
        return new ValidFCondition( maccept.get() );
    }

}
