#ifndef FUNCTIONFACTORY_HPP
#define FUNCTIONFACTORY_HPP

#include "CommandFactoryInterface.hpp"
#include "corelib/Property.hpp"
#include <map>
#include <string>

namespace ORO_Execution
{
    class Processor;

    using ORO_CoreLib::Property;
    using ORO_CoreLib::PropertyBag;

    /**
     * A CommandFactory which delivers commands which execute
     * a FunctionGraph in a Processor.
     */
    class FunctionFactory
        : public CommandFactoryInterface
    {
        typedef std::map<std::string, FunctionGraph*> map_t;
        map_t funcmap;
        Processor* proc;
    public:
        FunctionFactory(Processor* procs);
        ~FunctionFactory();

        void addFunction(const std::string& name, FunctionGraph* f ) ;

        bool hasCommand(const std::string& com) const ;

        std::vector<std::string> getCommandList() const;

        std::string getDescription( const std::string& com ) const;

        ORO_CoreLib::PropertyBag
        getArgumentSpec( const std::string& command ) const;

        std::vector< ArgumentDescription > getArgumentList( const std::string& command ) const;

        ComCon create( const std::string& command,
                       const ORO_CoreLib::PropertyBag& args,
                       bool asyn = true ) const;

        ComCon create(const std::string& command,
                      const std::vector<DataSourceBase*>& args,
                      bool asyn = true ) const;
    };
}

#endif
