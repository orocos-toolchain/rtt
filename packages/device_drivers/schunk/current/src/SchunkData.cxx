// Jan Veldeman
// 2-8-2002

#include "corelib/SchunkData.hpp"

namespace ORO_DeviceDriver
{

    inline void SchunkData::setOverloaded( bool overload )
    {
        _overload = overload;
    }

    inline bool SchunkData::getOverloaded()
    {
        return _overload;
    }

    inline void SchunkData::setValue( int idx, int value )
    {
        data[ idx ] = value;
    }

    inline int SchunkData::getXForce()
    {
        return data[ 0 ];
    }

    inline int SchunkData::getYForce()
    {
        return data[ 1 ];
    }

    inline int SchunkData::getZForce()
    {
        return data[ 2 ];
    }

    inline int SchunkData::getXTorque()
    {
        return data[ 3 ];
    }

    inline int SchunkData::getYTorque()
    {
        return data[ 4 ];
    }

    inline int SchunkData::getZTorque()
    {
        return data[ 5 ];
    }

    SchunkData::~SchunkData()
    {}

} // namespace

