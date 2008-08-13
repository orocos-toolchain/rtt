/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  PropertySequence.cxx

                        PropertySequence.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "PropertySequence.hpp"
#include "Property.hpp"

namespace RTT
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
