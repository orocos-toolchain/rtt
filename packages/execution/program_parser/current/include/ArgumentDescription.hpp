#ifndef ARGUMENTDESCRIPTION_HPP
#define ARGUMENTDESCRIPTION_HPP

#include <string>

namespace ORO_Execution
{
    /**
     * @brief Description of one Argument of a Command.
     */
    struct ArgumentDescription
    {
        ArgumentDescription (const std::string _name, const std::string _desc)
            : name( _name), description(_desc) {}
        std::string name;
        std::string description;
    };
}

#endif
