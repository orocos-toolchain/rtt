/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  PropertySequence.cxx 

                        PropertySequence.cxx -  description
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

#pragma implementation
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
