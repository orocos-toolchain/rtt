#ifndef PROPERTYOPERATION_HPP
#define PROPERTYOPERATION_HPP

//#include <Property.hpp>
#include "OperationAcceptor.hpp"

namespace ORO_CoreLib
{

    template < class T>
        class Property;

    namespace detail
    {
        /**
         * A property operation folowing the command/comply software pattern.
         */
        class PropertyOperation
        {
            protected:
                /**
                 * Storage mechanism
                 */
                template< class StoreType >
                    struct Storage
                    {
                        static StoreType* incentor;
                        static const StoreType* complier;
                    };

            public:

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
                 * Comply with the FillOperation.
                 */
                template< class T>
                    bool comply( const Property<T>* _complier )
                    {
                        Storage< Property<T> >::complier = _complier;
                        if (Storage< Property<T> >::incentor != 0 && Storage< Property<T> >::complier != 0)
                            Storage< Property<T> >::incentor->update( *Storage< Property<T> >::complier );
                        else
                        {
                            Storage< Property<T> >::incentor = 0;
                            Storage< Property<T> >::complier = 0;
                            //std::cout <<"******************failure"<<std::endl;
                            return false;
                        }
                        Storage< Property<T> >::incentor = 0;
                        Storage< Property<T> >::complier = 0;
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

