#ifndef ORO_REALTIME_TOOLKIT_HPP
#define ORO_REALTIME_TOOLKIT_HPP

#include <corelib/Toolkit.hpp>

namespace ORO_CoreLib
{

    /**
     * This interface defines the types of the realTime package.
     */
    class RealTimeToolkitPlugin
       : public ORO_CoreLib::ToolkitPlugin
    {
    public:
        virtual std::string getName();

        virtual bool loadTypes();
        virtual bool loadOperators();
    };

    /**
     * The single global instance of the RealTime Toolkit.
     */
    extern RealTimeToolkitPlugin RealTimeToolkit;
}

#endif
