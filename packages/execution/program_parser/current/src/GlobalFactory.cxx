
#include "execution/GlobalFactory.hpp"

namespace ORO_Execution
{

    const GlobalCommandFactory& GlobalFactory::commandFactory() const
    {
        return commandFact;
    }

    GlobalCommandFactory& GlobalFactory::commandFactory()
    {
        return commandFact;
    }

    GlobalDataSourceFactory& GlobalFactory::dataFactory()
    {
        return dataFact;
    }

    const GlobalDataSourceFactory& GlobalFactory::dataFactory() const
    {
        return dataFact;
    }

    GlobalMethodFactory& GlobalFactory::methodFactory()
    {
        return methodFact;
    }

    const GlobalMethodFactory& GlobalFactory::methodFactory() const
    {
        return methodFact;
    }


}
