
#include "corelib/ConditionOnce.hpp"

namespace ORO_CoreLib
{

    ConditionOnce::ConditionOnce(bool what) : ran( false ), _what(what)
    {}

    ConditionOnce::~ConditionOnce()
    {}

    bool ConditionOnce::evaluate()
    {
        if ( ran == true )
            return !_what;
        else 
            ran = true;
        return _what;
    }

    void ConditionOnce::reset()
    {
        ran = false;
    }

    ConditionInterface* ConditionOnce::clone() const
    {
        return new ConditionOnce( _what );
    }
}

