#include "execution/ConditionBool.hpp"

namespace ORO_Execution
{

    ConditionBool::ConditionBool() :
            state( false )
    {
    }

    ConditionBool::ConditionBool( bool value ) :
            state( value )
    {
    }

    ConditionBool::~ConditionBool()
    {
    }

    bool ConditionBool::evaluate()
    {
        return state;
    }

    void ConditionBool::setTrue()
    {
        state = true;
    }

    void ConditionBool::setFalse()
    {
        state = false;
    }

    ConditionInterface* ConditionBool::clone() const
    {
        return new ConditionBool( state );
    };
}
