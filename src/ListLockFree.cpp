/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  ListLockFree.cxx 

                        ListLockFree.cxx -  description
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
 
 

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "ListLockFree.hpp"


void intrusive_ptr_add_ref( RTT::detail::IntrusiveStorage* p ) {
    atomic_inc( &p->ref );
}

void intrusive_ptr_release( RTT::detail::IntrusiveStorage* p ) {
    if (atomic_dec_and_test( &p->ref ) )
        delete p;
}

namespace RTT {
    // Force an instantiation, so that the compiler checks the syntax.
    template class ListLockFree<double>;
}
