#ifndef ORO_TOOLKIT_HPP
#define ORO_TOOLKIT_HPP

#include <string>
#include <vector>

namespace ORO_CoreLib
{

    /**
     * This interface defines how additional toolkits
     * are loaded into Orocos.
     * A ToolkitPlugin defines additional user data types for a Toolkit.
     */
    class ToolkitPlugin
    {
    public:
        virtual ~ToolkitPlugin() {}

        /**
         * Implement this method to add types to the 
         * Orocos type system.
         */
        virtual bool loadTypes() = 0;

        /**
         * Implement this method to load Scripting
         * operators on types, such as '+', '*', ...
         */
        virtual bool loadOperators() = 0;

        /**
         * Each plugin must have a unique name.
         */
        virtual std::string getName() = 0;
    };

    /**
     * This class allows the Orocos Real-Time Toolkit to be extended
     * with additional toolkits (ToolkitPlugin), such as for kinematics or any
     * library foreign to Orocos.
     */
    class Toolkit
    {
        static std::vector<ToolkitPlugin*> Tools;
    public:
        /**
         * Import a plugin into Orocos.
         * One may try to load the same plugin multiple
         * times from different places. Only the first
         * import will succeed. Multiple imports will not
         * be considered as an error.
         */
        static void Import( ToolkitPlugin& tkp );

        /**
         * Get the names of all imported tools.
         */
        static std::vector<std::string> getTools();

        /**
         * Check if a tool with given name was already imported.
         */
        static bool hasTool( const std::string& toolname );
    };

}

#endif
