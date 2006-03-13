/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyCommands.hpp 

                        PropertyCommands.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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
 
#ifndef ORO_PROPERTY_HPP
// #error "Do not include this file directly, but only through Property.hpp"
#else
 
#ifndef ORO_CORELIB_PROPERTY_COMMANDS
#define ORO_CORELIB_PROPERTY_COMMANDS

#include "CommandInterface.hpp"
#include "DataSource.hpp"


namespace ORO_CoreLib
{

    namespace detail {

        /**
         * Update a Property<T>.
         */
        template<class T, class S = T>
        class UpdatePropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<S>* source;
        public:
            UpdatePropertyCommand( Property<T>* tgt, const Property<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                return target->update( *source );
            }

            virtual UpdatePropertyCommand<T,S>* clone() const {
                return new UpdatePropertyCommand<T,S>(target, source);
            }
        };

        /**
         * Copy a Property<T>.
         */
        template<class T, class S = T>
        class CopyPropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<S>* source;
        public:
            CopyPropertyCommand( Property<T>* tgt, const Property<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                return target->copy( *source );
            }

            virtual CopyPropertyCommand<T,S>* clone() const {
                return new CopyPropertyCommand<T,S>(target, source);
            }
        };

        /**
         * Refresh a Property<T>.
         */
        template<class T, class S = T>
        class RefreshPropertyCommand
            : public CommandInterface
        {
            Property<T>* target;
            const Property<S>* source;
        public:
            RefreshPropertyCommand( Property<T>* tgt, const Property<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                return target->refresh( *source );
            }

            virtual RefreshPropertyCommand<T,S>* clone() const {
                return new RefreshPropertyCommand<T,S>(target, source);
            }
        };

        /**
         * Refresh a Property<T> with a DataSource<S>.
         */
        template<class T, class S = T>
        class RefreshPropertyFromDSCommand
            : public CommandInterface
        {
            Property<T>* target;
            typename DataSource<S>::shared_ptr source;
        public:
            RefreshPropertyFromDSCommand( Property<T>* tgt, DataSource<S>* src)
                : target(tgt), source(src) {}
            bool execute()
            {
                return target->refresh( *source );
            }

            virtual RefreshPropertyFromDSCommand<T,S>* clone() const {
                return new RefreshPropertyFromDSCommand<T,S>(target, source.get() );
            }

            virtual RefreshPropertyFromDSCommand<T,S>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new RefreshPropertyFromDSCommand<T,S>( target, source->copy(alreadyCloned) );
            }
        };

    }

}

#endif
#endif
