
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
