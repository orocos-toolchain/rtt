/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  PropertyOperation.hpp 

                        PropertyOperation.hpp -  description
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
 
 
#ifndef PROPERTYOPERATION_HPP
#define PROPERTYOPERATION_HPP

//#include <Property.hpp>
#include "OperationAcceptor.hpp"

namespace ORO_CoreLib
{

    template < class T>
        class Property;

    class PropertyBase;

    namespace detail
    {
        /**
         * A property operation folowing the command/comply software pattern.
         * It stores a pointer to incentor and complier. A subclass
         * must implement the operation itself.
         */
        class PropertyOperation
        {
            protected:
                /**
                 * Storage mechanism. It should be thread-local...
                 * If two PropertyOperations operate concurrently
                 * on the same StorageType, it will fail.
                 * Making it local to the Operation is not possible,
                 * Because then, a PropertyOperation for each 
                 * Property<T> type must be provided. The 
                 * PropertyOperation can not be templated
                 * because then it is inserted again in the PropertyBase
                 * class which must remain template free.
                 */
                template< class StoreType >
                    struct Storage
                    {
                        static StoreType* incentor;
                        static const StoreType* complier;
                    };
        };

        template< class T>
            T* PropertyOperation::Storage<T>::incentor = 0;
        template< class T>
            const T* PropertyOperation::Storage<T>::complier = 0;

        /**
         * This operation fills one Property with another of the same type.
         */
        class FillOperation : public PropertyOperation
        {
            PropertyBase* incentor;
            public:
                /**
                 * A binary operation. Operate on the Incentor if the complier agrees
                 * to cooperate with the operation.
                 */
                template< class T >
                    bool command( Property<T>& _incentor, const OperationAcceptor* _complier)
                    {
                        //Storage< Property<T> >::incentor =  &_incentor;
                        // we loose type info here !
                        // I can't find another way to do it thread-safe.
                        // But when walking this path, a simple
                        // dynamic_cast on _complier is actually enough to find out
                        // (but then we bypass the _compliers agreement to the op.)
                        incentor = &_incentor; 
                        return _complier->comply(this);
                    }
                /**
                 * Comply with the FillOperation. This function uses the
                 * efficient full-type update function. 
                 */
                template< class T>
                    bool comply( const Property<T>* _complier )
                    {
                        Property<T>* _incentor;
                        // we got the complier type
                        //Storage< Property<T> >::complier = _complier;
                        //if (Storage< Property<T> >::incentor != 0 && Storage< Property<T> >::complier != 0)
                        if ( (_incentor = dynamic_cast< Property<T>* >( incentor ) ) && _complier !=0 )
                            _incentor->update( *_complier );
                        else
                        {
                            //Storage< Property<T> >::incentor = 0;
                            //Storage< Property<T> >::complier = 0;
                            //std::cout <<"******************failure"<<std::endl;
                            return false;
                        }
                        //Storage< Property<T> >::incentor = 0;
                        //Storage< Property<T> >::complier = 0;
                        //std::cout <<"*******************success"<<std::endl;
                        return true;
                    }

                /**
                 * Deny the fill operation.
                 */
                void deny()
                {
                    //std::cout <<"******************denied"<<std::endl;
                    // do some optional stuff.
                }
        };

        /**
         * This property makes a deep copy of one Property to a Property of the same type.
         */
        class DeepCopyOperation : public PropertyOperation
        {
            public:
                /**
                 * A binary operation. Operate on the Incentor if the complier agrees
                 * to cooperate with the operation.
                 */
                template< class T >
                    bool command( Property<T>& _incentor, const OperationAcceptor* _complier)
                    {
                        Storage< Property<T> >::incentor =  &_incentor;
                        return _complier->comply(this);
                    }
                /**
                 * Comply with the DeepCopyOperation.
                 */
                template< class T>
                    bool comply( const Property<T>* _complier )
                    {
                        Storage< Property<T> >::complier = _complier;
                        if (Storage< Property<T> >::incentor != 0 && Storage< Property<T> >::complier != 0)
                            Storage< Property<T> >::incentor->copy(*Storage< Property<T> >::complier);
                        else
                        {
                            Storage< Property<T> >::incentor = 0;
                            Storage< Property<T> >::complier = 0;
                            return false;
                        }
                        Storage< Property<T> >::incentor = 0;
                        Storage< Property<T> >::complier = 0;
                        return true;
                    }

                /**
                  template< class T>
                  bool comply( T& _complier )
                  {
                  Storage< Property<T> >::incentor = 0;
                  return false;
                  }
                  */

                /**
                 * Deny the copy operation.
                 */
                void deny()
                {
                    //std::cout <<"******************denied"<<std::endl;
                }
        };

#if 0
        class SerializeOperation : public PropertyOperation
        {
            public:


                template< class T>
                    void serialize( Property<T>& prop)
                    {}

                template< class T>
                    bool comply( Property<T>& _complier )
                    {
                        Storage< Property<T> >::complier = &_complier;
                        return Operation()(Storage< Property<T> >::incentor, Storage< Property<T> >::complier);
                    }

                template< class T>
                    bool comply( T& _complier )
                    {
                        return false;
                    }

                void deny()
                {
                    fail();
                }

                virtual void fail()
                {}
        };

#endif
    }
}



#endif

