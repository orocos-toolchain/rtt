#include "execution/DataSource.hpp"

namespace ORO_Execution
{
    DataSourceBase::~DataSourceBase()
    {
    }
    
    void DataSourceBase::reset()
    {
    }
}

void intrusive_ptr_add_ref( ORO_Execution::DataSourceBase* p )
{
  p->ref();
};

void intrusive_ptr_release( ORO_Execution::DataSourceBase* p )
{
  p->deref();
};
