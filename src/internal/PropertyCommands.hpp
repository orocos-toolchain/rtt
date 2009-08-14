/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyCommands.hpp

                        PropertyCommands.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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

#ifndef ORO_PROPERTY_HPP
// #error "Do not include this file directly, but only through Property.hpp"
#else

#ifndef ORO_CORELIB_PROPERTY_COMMANDS
#define ORO_CORELIB_PROPERTY_COMMANDS

#include "../base/ActionInterface.hpp"
#include "DataSource.hpp"


namespace RTT
{

    namespace detail {

        /**
         * Update a Property<T>.
         * This command is not guaranteed real-time.
         */
        template<class T>
        class UpdatePropertyCommand
            : public ActionInterface
        {
            Property<T>* target;
            const Property<T>* source;
        public:
            UpdatePropertyCommand( Property<T>* tgt, const Property<T>* src)
                : target(tgt), source(src) {}

            void readArguments() {
            }

            bool execute()
            {
                return target->update( *source );
            }

            virtual UpdatePropertyCommand<T>* clone() const {
                return new UpdatePropertyCommand<T>(target, source);
            }
        };

        /**
         * Copy a Property<T>.
         * This command is not real-time.
         */
        template<class T>
        class CopyPropertyCommand
            : public ActionInterface
        {
            Property<T>* target;
            const Property<T>* source;
        public:
            CopyPropertyCommand( Property<T>* tgt, const Property<T>* src)
                : target(tgt), source(src) {}

            void readArguments() {
            }

            bool execute()
            {
                return target->copy( *source );
            }

            virtual CopyPropertyCommand<T>* clone() const {
                return new CopyPropertyCommand<T>(target, source);
            }
        };

        /**
         * Refresh a Property<T>.
         * This command is guaranteed real-time.
         */
        template<class T>
        class RefreshPropertyCommand
            : public ActionInterface
        {
            Property<T>* target;
            const Property<T>* source;
        public:
            RefreshPropertyCommand( Property<T>* tgt, const Property<T>* src)
                : target(tgt), source(src) {}

            void readArguments() {
                source->get();
            }

            bool execute()
            {
                return target->refresh( *source );
            }

            virtual RefreshPropertyCommand<T>* clone() const {
                return new RefreshPropertyCommand<T>(target, source);
            }
        };

        /**
         * Refresh a Property<T> with a DataSource<S>.
         * This command is guaranteed real-time
         */
        template<class T>
        class RefreshPropertyFromDSCommand
            : public ActionInterface
        {
            Property<T>* target;
            typename DataSource<T>::shared_ptr source;
        public:
            RefreshPropertyFromDSCommand( Property<T>* tgt, DataSource<T>* src)
                : target(tgt), source(src) {}

            void readArguments() {
                source->evaluate();
            }

            bool execute()
            {
                return target->refresh( *source );
            }

            virtual RefreshPropertyFromDSCommand<T>* clone() const {
                return new RefreshPropertyFromDSCommand<T>(target, source.get() );
            }

            virtual RefreshPropertyFromDSCommand<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new RefreshPropertyFromDSCommand<T>( target, source->copy(alreadyCloned) );
            }
        };

    }

}

#endif
#endif
