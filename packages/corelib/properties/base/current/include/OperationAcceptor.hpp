/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  OperationAcceptor.hpp 

                        OperationAcceptor.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef OPERATIONACCEPTOR_HPP
#define OPERATIONACCEPTOR_HPP

namespace ORO_CoreLib
{
    namespace detail
    {
        class FillOperation;
        class DeepCopyOperation;
        class PropertyOperation;

        /**
         * The OperationAcceptor can be requested to comply with a
         * certain operation. It can deny() or comply(this) as
         * a result.
         * This class denies every request by
         * default. You can specialize this function to accept
         * requests for certain operations.
         */
        class OperationAcceptor
        {
            public:
                /**
                 * Existing operations, need to be overloaded in
                 * the subclass to implement the specific operation. 
                 * Each operation defaults to deny.
                 * @group comply 
                 * @{
                 */
                //virtual bool comply( PropertyOperation* op ) const;
                virtual bool comply( FillOperation* op ) const;
                virtual bool comply( DeepCopyOperation* op ) const;
                /**
                 * @}
                 */

                /**
                 * Default deny for unknown operations.
                 */
                /*
                   template < class T >
                   bool comply( T* op ) const
                   {
                   op->deny(); 
                   return false; 
                   }
                   */
        };
    }
}

#endif

