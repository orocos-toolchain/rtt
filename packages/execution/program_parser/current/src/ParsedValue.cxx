#include "execution/ParsedValue.hpp"

namespace ORO_Execution
{
  ParsedValueBase::~ParsedValueBase()
  {
  }

  CommandInterface* ParsedValueBase::assignCommand(
    DataSourceBase*, bool ) const
  {
    return 0;
  }

  CommandInterface* ParsedValueBase::assignIndexCommand(
                                                   DataSourceBase*,
                                                   DataSourceBase* ) const
  {
    return 0;
  }

}
