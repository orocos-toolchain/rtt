#include "execution/GlobalCommandFactory.hpp"

#include <cassert>

namespace ORO_Execution {

  bool GlobalCommandFactory::hasCommand(
    const std::string& objectname,
    const std::string& com )
  {
    const CommandFactoryInterface* o = getObjectFactory( objectname );
    if ( o ) return o->hasCommand( com );
    else return false;
  };

  void GlobalCommandFactory::registerObject(
    const std::string& objectname,
    CommandFactoryInterface* cif )
  {
    assert( cif );
    mdata[objectname] = cif;
  };

  GlobalCommandFactory::GlobalCommandFactory()
  {
  };

  /* search for an object a return its factory */
  const CommandFactoryInterface * GlobalCommandFactory::getObjectFactory(
    const std::string& objectname) const
  {
    const_iter_t r = mdata.find( objectname );
    if ( r == mdata.end() ) return 0;
    else return r->second;
  };

  void GlobalCommandFactory::unregisterObject( const std::string& objectname )
  {
    mdata.erase( objectname );
  }

    std::vector<std::string> GlobalCommandFactory::getObjectList() const
    {
        std::vector<std::string> mlist;
        for (const_iter_t r = mdata.begin(); r != mdata.end(); ++r )
            mlist.push_back( r->first );
        return mlist;
    }
            
}
