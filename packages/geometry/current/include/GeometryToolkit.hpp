#ifndef ORO_GEOMETRY_TOOLKIT_HPP
#define ORO_GEOMETRY_TOOLKIT_HPP

#include <corelib/Toolkit.hpp>

namespace ORO_Geometry
{

    /**
     * This interface defines the types of the geometry package.
     */
    class GeometryToolkitPlugin
       : public ORO_CoreLib::ToolkitPlugin
    {
    public:
        virtual std::string getName();

        virtual bool loadTypes();
        virtual bool loadOperators();
    };

    /**
     * The single global instance of the Geometry Toolkit.
     */
    extern GeometryToolkitPlugin GeometryToolkit;
}

#endif
