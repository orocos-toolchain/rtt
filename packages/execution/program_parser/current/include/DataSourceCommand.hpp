#ifndef DATASOURCECOMMAND_HPP
#define DATASOURCECOMMAND_HPP

#include "DataSource.hpp"

namespace ORO_CoreLib
{
  class CommandInterface;
}

namespace ORO_Execution
{
  using ORO_CoreLib::CommandInterface;

  /**
   * A class that wraps a Command in a DataSource<bool>
   * interface.
   */
  class DataSourceCommand
    : public DataSource<bool>
  {
    CommandInterface* comm;
  public:
    /**
     * DataSourceCommand takes ownership of the command you pass
     * it.
     */
      DataSourceCommand( CommandInterface* c );
      DataSourceCommand( const DataSourceCommand& orig );
      ~DataSourceCommand();
      bool get() const;
      void reset();
      CommandInterface* command() const;
      virtual DataSourceCommand* clone() const;
      virtual DataSourceCommand* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned );
  };
}

#endif
