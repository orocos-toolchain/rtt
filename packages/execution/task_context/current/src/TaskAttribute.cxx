#include "execution/TaskAttribute.hpp"

namespace ORO_Execution
{
  TaskAttributeBase::~TaskAttributeBase()
  {
  }

  CommandInterface* TaskAttributeBase::assignCommand(
    DataSourceBase*, bool ) const
  {
    return 0;
  }

  CommandInterface* TaskAttributeBase::assignIndexCommand(
                                                   DataSourceBase*,
                                                   DataSourceBase* ) const
  {
    return 0;
  }

}
