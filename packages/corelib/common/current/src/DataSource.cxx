#include "corelib/DataSource.hpp"

namespace ORO_CoreLib
{
  DataSourceBase::~DataSourceBase()
  {
  }

  void DataSourceBase::reset()
  {
  }
}

void intrusive_ptr_add_ref( ORO_CoreLib::DataSourceBase* p )
{
  p->ref();
};

void intrusive_ptr_release( ORO_CoreLib::DataSourceBase* p )
{
  p->deref();
};
