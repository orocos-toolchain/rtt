#include "control_kernel/ExecutionExtension.hpp"

namespace ORO_ControlKernel
{
  bool ExecutionComponentInterface::enableAspect( ExecutionExtension* ext )
  {
    master = ext;
    commandfactory = createCommandFactory();
    if ( commandfactory )
      master->commandFactory().registerObject( name, commandfactory );
    dataSourceFactory = createDataSourceFactory();
    if ( dataSourceFactory )
      master->dataFactory().registerObject( name, dataSourceFactory );
    return true;
  }

  bool ExecutionExtension::updateProperties( const PropertyBag& bag )
  {
    // no properties yet :)
    return true;
  }

  void ExecutionComponentInterface::disableAspect()
  {
    master->commandFactory().unregisterObject( name );
    master->dataFactory().unregisterObject( name );
  }

  ExecutionComponentInterface::~ExecutionComponentInterface()
  {
  }

  CommandFactoryInterface* ExecutionComponentInterface::createCommandFactory()
  {
    return 0;
  }

  DataSourceFactory* ExecutionComponentInterface::createDataSourceFactory()
  {
    return 0;
  }
}

