/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  PropertyOperation.hpp 

                        PropertyOperation.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef PROPERTYOPERATION_HPP
#define PROPERTYOPERATION_HPP

#pragma interface

#include <vector>

namespace ORO_Geometry
{
    class Frame;
    class Rotation;
    class Vector;
    class Twist;
    class Wrench;
}

// I don't fully understand why forward decls don't work :
#include "pkgconf/system.h"
#ifdef OROPKG_GEOMETRY
#include "geometry/frames.h"
#endif

#include "PropertyBag.hpp"

namespace ORO_CoreLib
{

    template < class T>
        class Property;

    class PropertyBase;

    namespace detail
    {
        /**
         * A property operation folowing the visitor software pattern.
         * A subclass must specialise the comply method of the type it is..
         *
         * For each property-type you want to use, add a comply method in this
         * class. For example Property<Frame>, ... For technical reasons,
         * this class must remain template free.
         */
        class PropertyOperation
        {
        public:
            virtual ~PropertyOperation() {}

            virtual bool comply(const Property<int>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<unsigned int>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<char>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<bool>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<float>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<double>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<std::string>* ) 
            {
                return false;
            }

            virtual bool comply(const Property<std::vector<double> >* ) 
            {
                return false;
            }

            virtual bool comply(const Property<std::vector<int> >* ) 
            {
                return false;
            }

            virtual bool comply(const Property< PropertyBag >* ) 
            {
                return false;
            }

            virtual bool comply(const Property< ORO_Geometry::Frame >* ) 
            {
                return false;
            }

            virtual bool comply(const Property< ORO_Geometry::Rotation >* ) 
            {
                return false;
            }

            virtual bool comply(const Property< ORO_Geometry::Twist >* ) 
            {
                return false;
            }

            virtual bool comply(const Property< ORO_Geometry::Wrench >* ) 
            {
                return false;
            }

            virtual bool comply(const Property< ORO_Geometry::Vector >* ) 
            {
                return false;
            }

            /**
             * If none of the primitive types is used,
             * leave it to the Operation classes, which will
             * try to dynamic_cast<> the argument to their type.
             */
            virtual bool comply(const PropertyBase* ) = 0;
        };

        /**
         * This operation fills one Property with another of the same type.
         */
        template< class T>
        class FillOperation
            : public PropertyOperation
        {
            Property<T>* incentor;
            public:
            FillOperation( Property<T>* _incentor ) : incentor(_incentor) {}
            /**
             * A binary operation. Operate on the Incentor if the complier agrees
             * to cooperate with the operation.
             */
            bool command( const PropertyBase* _complier)
            {
                return _complier->accept(this);
            }

            using PropertyOperation::comply;

            /**
             * Comply with the FillOperation<T>. This function uses the
             * efficient full-type Property<T>::update function and specialises
             * the apropriate PropertyOperation base class method.
             */
            bool comply( const Property<T>* _complier )
            {
                incentor->update( *_complier );
                return true;
            }

            virtual bool comply(const PropertyBase* _complier )
            {
                const Property<T>* comp = dynamic_cast< const Property<T>* >( _complier );
                if ( comp ) {
                    incentor->update( *comp );
                    return true;
                }
                return false;
            }


        };

        /**
         * This operation makes a deep copy of one Property to a Property of the same type.
         */
        template< class T>
        class DeepCopyOperation
            : public PropertyOperation
        {
            Property<T>* incentor;
        public:
            DeepCopyOperation( Property<T>* _i ) : incentor(_i) {}
            /**
             * A binary operation. Operate on the Incentor if the complier agrees
             * to cooperate with the operation.
             */
            bool command( const PropertyBase* _complier)
            {
                return _complier->accept(this);
            }

            using PropertyOperation::comply;

            /**
             * Comply with the DeepCopyOperation.
             */
            bool comply( const Property<T>* _complier )
            {
                incentor->copy( _complier );
                return true;
            }

            virtual bool comply(const PropertyBase* _complier )
            {
                const Property<T>* comp = dynamic_cast< const Property<T>* >( _complier );
                if ( comp ) {
                    incentor->copy( *comp );
                    return true;
                }
                return false;
            }

        };

    }
}



#endif

