 
#ifndef GLOBALCOMMANDFACTORYINTERFACE_HPP
#define GLOBALCOMMANDFACTORYINTERFACE_HPP

#include "CommandFactoryInterface.hpp"

namespace ORO_Execution
	{

    /**
     * @brief This interface describes a global command factory.
     *
     * This factory has the collection of all CommandFactories.
     */
    class GlobalCommandFactoryInterface
    {
    public:
        virtual ~GlobalCommandFactoryInterface() {}

      /**
       *  Check whether the factory registered with a given object,
       *  contains a given command.
       *
       * @param objectname a registered object
       * @param	com a command
       */
      virtual bool hasCommand(const std::string& objectname,const std::string& com) = 0;

      /**
       * Search for an object and return its factory.
       * @param objectname name of the object.
       * @return the requested factory, or 0, indicating no factory
       * has been registered under that name..
       */
      virtual const CommandFactoryInterface* getObjectFactory(
        const std::string& objectname ) const = 0;
    };
}

#endif
