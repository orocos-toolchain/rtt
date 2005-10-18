/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  GlobalCommandFactory.cxx 

                        GlobalCommandFactory.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "execution/GlobalCommandFactory.hpp"
#include "execution/CommandFactoryComposite.hpp"
#include "execution/ExecutionEngine.hpp"

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
    if ( mdata.count( objectname ) != 0 )
        cif = new CommandFactoryComposite( mdata[objectname], cif );
    mdata[objectname] = cif;
  };

  GlobalCommandFactory::GlobalCommandFactory(ExecutionEngine* ee)
      :eproc(ee)
  {
  }

  GlobalCommandFactory::~GlobalCommandFactory()
  {
      for (iter_t r = mdata.begin(); r != mdata.end(); ++r )
          delete r->second;
  }

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
      if ( mdata.count( objectname ) ) {
          delete mdata[objectname];
          mdata.erase( objectname );
      }
  }

    std::vector<std::string> GlobalCommandFactory::getObjectList() const
    {
        std::vector<std::string> mlist;
        for (const_iter_t r = mdata.begin(); r != mdata.end(); ++r )
            mlist.push_back( r->first );
        return mlist;
    }

    Processor* GlobalCommandFactory::getCommandProcessor() const {
        return eproc->getCommandProcessor();
    }
}
