/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  GlobalMemberFactory.cxx 

                        GlobalMemberFactory.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include "execution/GlobalMemberFactory.hpp"

namespace ORO_Execution
{


      GlobalMemberFactory::GlobalMemberFactory()
      {
      }

      GlobalMemberFactory::~GlobalMemberFactory()
      {
          for (map_t::iterator r = mdata.begin(); r != mdata.end(); ++r )
              delete r->second;
      }

      MethodC GlobalMemberFactory::create(const std::string& object,
                     const std::string& command) const {
          return MethodC(this, object, command );
      }
                        
      bool GlobalMemberFactory::hasMember(const std::string& objectname,
                     const std::string& fun ) const
      {
          const MemberFactoryInterface* o = getObjectFactory( objectname );
          if ( o ) return o->hasMember( fun );
          else return false;
      }

      void GlobalMemberFactory::registerObject( const std::string& name,
                           const MemberFactoryInterface* fact )
      {
          if ( mdata.count( name ) != 0 )
              fact = new MemberFactoryComposite( mdata[name], fact );
          mdata[name] = fact;
      }

      void GlobalMemberFactory::unregisterObject( const std::string& name )
      {
          if ( mdata.count( name ) ) {
              delete mdata[ name ];
              mdata.erase( name );
          }
      }

      const MemberFactoryInterface* GlobalMemberFactory::getObjectFactory( const std::string& name ) const
      {
          map_t::const_iterator i = mdata.find( name );
          if ( i == mdata.end() ) return 0;
          else return i->second;
      }

      std::vector<std::string> GlobalMemberFactory::getObjectList() const
      {
          std::vector<std::string> mlist;
          for (map_t::const_iterator r = mdata.begin(); r != mdata.end(); ++r )
              mlist.push_back( r->first );
          return mlist;
      }

}
