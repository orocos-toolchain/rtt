#ifndef GLOBAL_FACTORY_HPP
#define GLOBAL_FACTORY_HPP

#include "CommandFactoryInterface.hpp"
#include "GlobalCommandFactory.hpp"
#include "DataSourceFactory.hpp"


namespace ORO_Execution
{

    /**
     * This factory is used by the Parser ( and sub-Parsers )
     * to retrieve existing commands and data structures.
     * It merely stores the GlobalCommandFactory and GlobalDataSourceFactory
     * for a given application.
     */
    class GlobalFactory
    {
        GlobalCommandFactory commandFact;
        GlobalDataSourceFactory dataFact;

    public:
        const GlobalCommandFactory& commandFactory() const
        {
            return commandFact;
        }

        GlobalCommandFactory& commandFactory()
        {
            return commandFact;
        }

        GlobalDataSourceFactory& dataFactory()
        {
            return dataFact;
        }

        const GlobalDataSourceFactory& dataFactory() const
        {
            return dataFact;
        }

    };

}

#endif
