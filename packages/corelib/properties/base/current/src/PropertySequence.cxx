
#include "corelib/PropertySequence.hpp"
#include "corelib/Property.hpp"

namespace ORO_CoreLib
{

    template< class T>
    void refreshProperties(PropertySequence<T>& target, const PropertySequence<T>& source)
    {
        //iterate over source, update Ts
        typename PropertySequence<T>::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            T* mine = target.find( (*it)->getName() );
            if (mine != 0)
            {
                //std::cout <<"*******************refresh"<<std::endl;
                mine->update( (*it) );
            }
            ++it;
        }
    }

    template< class T>
    void copyProperties(PropertySequence<T>& target, const PropertySequence<T>& source)
    {
        // Make a full deep copy.
        //iterate over source, update or clone PropertyBases
        typename PropertySequence<T>::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            T* mine = target.find( (*it)->getName() );
            // This is the mistake, the clone created a 'symlink' to the
            // exiting one and we copy into the original instead of 
            // a clone().
            if (mine != 0)
                mine->copy( (*it) );  // no need to make new one, just copy over existing one.
            else
            {
                // step 1 : clone a new instance (non deep copy)
                T* temp = (*it)->create();  // 1. XXX bag refers to props
                // step 2 : deep copy clone with original.
                temp->copy( *it );                     // 2. XXX make copy in refers !
                // step 3 : add result to target bag.
                target.add( temp );
            }
            ++it;
        }
    }

    template< class T>
    void deleteProperties(PropertySequence<T>& target)
    {
        //iterate over target, delete PropertyBases
        typename PropertySequence<T>::const_iterator it( target.getProperties().begin() );
        while ( it != target.getProperties().end() )
        {
            delete (*it);
            ++it;
        }
        target.clear();
    }

}
