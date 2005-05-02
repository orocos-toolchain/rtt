#include "execution/DataSourceCommand.hpp"

#include "corelib/CommandInterface.hpp"

namespace ORO_Execution
{
  DataSourceCommand::DataSourceCommand( CommandInterface* c )
    : comm( c )
  {
  }

  DataSourceCommand::DataSourceCommand( const DataSourceCommand& orig )
    : comm( orig.command()->clone() )
  {
  }

  DataSourceCommand::~DataSourceCommand()
  {
    delete comm;
  }

  bool DataSourceCommand::get() const
  {
    return comm->execute();
  }

  CommandInterface* DataSourceCommand::command() const
  {
      return comm;
  }

  void DataSourceCommand::reset()
  {
    comm->reset();
  }

  DataSourceCommand* DataSourceCommand::clone() const
  {
      return new DataSourceCommand( comm->clone() );
  }

  DataSourceCommand* DataSourceCommand::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned )
  {
      return new DataSourceCommand( comm->copy( alreadyCloned ) );
  }
}
