/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:24 CEST 2004  ComponentStateInterface.hpp 

                        ComponentStateInterface.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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

#ifndef COMPONENT_STATE_INTERFACE_HPP
#define COMPONENT_STATE_INTERFACE_HPP

namespace ORO_ControlKernel {

    class ComponentBaseInterface;

    namespace detail {

        /**
         * Groups all component type-specific
         * operations. For internal NextGenKernel use only.
         */
        struct ComponentStateInterface {
            virtual ~ComponentStateInterface() {}
            // Produce own DataObjects
            virtual bool load()   = 0;
            virtual bool reload() = 0;
            virtual bool unload() = 0;

            // Read external DataObjects
            virtual bool startup() = 0;
            virtual bool restart() = 0;
            virtual bool shutdown() = 0;

            // Create/Remove DataObjects
            virtual void create() = 0;
            virtual void erase() = 0;

            virtual bool select() = 0;
            virtual ComponentBaseInterface* component() = 0;
        };

        template <class Kernel, class Component>
        struct ComponentC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            ComponentC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadComponent( c );
            }

            bool select(){
                return c->select( k );
            }

            // unload/load
            bool reload(){
                return k->reloadComponent( c );
            }

            bool startup(){
                return k->startComponent(c);
            }

            // shutdown/startup
            bool restart() {
                return k->stopComponent(c) && k->startComponent(c);
            }

            bool shutdown() {
                return k->stopComponent(c);
            }

            bool unload(){
                return k->unloadComponent( c );
            }

            void create() {
                c->createDataObject( k );
            }

            void erase() {
                // do not erase yet, we must use smart pointers...
            }

            ComponentBaseInterface* component() {
                return c;
            }
        };
    }
}

#endif
