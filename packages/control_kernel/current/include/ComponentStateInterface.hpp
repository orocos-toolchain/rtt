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
    namespace detail {

        /**
         * Groups all component type-specific
         * operations. For internal NextGenKernel use only.
         */
        struct ComponentStateInterface {
            // Produce own DataObjects
            virtual bool load()   = 0;
            virtual bool reload() = 0;
            virtual bool unload() = 0;
            // Read external DataObjects
            virtual bool startup() = 0;
            virtual bool restart() = 0;
            virtual bool shutdown() = 0;
        };

        template <class Kernel, class Component>
        struct SensorC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            SensorC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadSensor( c );
            }

            // unload/load
            bool reload(){
                return k->reloadSensor( c );
            }

            bool startup(){
                return k->selectSensor(c);
            }

            // shutdown/startup
            bool restart() {
                return k->selectSensor("DefaultSensor") && k->selectSensor(c);
            }

            bool shutdown() {
                return k->selectSensor("DefaultSensor");
            }

            bool unload(){
                return k->unloadSensor( c );
            }
        };
        template <class Kernel, class Component>
        struct EstimatorC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            EstimatorC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadEstimator( c );
            }

            // unload/load
            bool reload(){
                return k->reloadEstimator( c );
            }

            bool startup(){
                return k->selectEstimator(c);
            }

            // shutdown/startup
            bool restart() {
                return k->selectEstimator("DefaultEstimator") && k->selectEstimator(c);
            }

            bool shutdown() {
                return k->selectEstimator("DefaultEstimator");
            }

            bool unload(){
                return k->unloadEstimator( c );
            }
        };
        template <class Kernel, class Component>
        struct GeneratorC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            GeneratorC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadGenerator( c );
            }

            // unload/load
            bool reload(){
                return k->reloadGenerator( c );
            }

            bool startup(){
                return k->selectGenerator(c);
            }

            // shutdown/startup
            bool restart() {
                return k->selectGenerator("DefaultGenerator") && k->selectGenerator(c);
            }

            bool shutdown() {
                return k->selectGenerator("DefaultGenerator");
            }

            bool unload(){
                return k->unloadGenerator( c );
            }
        };
        template <class Kernel, class Component>
        struct ControllerC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            ControllerC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadController( c );
            }

            // unload/load
            bool reload(){
                return k->reloadController( c );
            }

            bool startup(){
                return k->selectController(c);
            }

            // shutdown/startup
            bool restart() {
                return k->selectController("DefaultController") && k->selectController(c);
            }

            bool shutdown() {
                return k->selectController("DefaultController");
            }

            bool unload(){
                return k->unloadController( c );
            }
        };
        template <class Kernel, class Component>
        struct EffectorC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            EffectorC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadEffector( c );
            }

            // unload/load
            bool reload(){
                return k->reloadEffector( c );
            }

            bool startup(){
                return k->selectEffector(c);
            }

            // shutdown/startup
            bool restart() {
                return k->selectEffector("DefaultEffector") && k->selectEffector(c);
            }

            bool shutdown() {
                return k->selectEffector("DefaultEffector");
            }

            bool unload(){
                return k->unloadEffector( c );
            }
        };
        template <class Kernel, class Component>
        struct SupportC : public ComponentStateInterface
        {
            Component* c;
            Kernel*    k;
            SupportC(Kernel* _k, Component* _c) : c(_c), k(_k) {}

            bool load(){
                return k->loadSupport( c );
            }

            // unload/load
            bool reload(){
                return k->reloadSupport( c );
            }

            bool startup(){
                return true; //k->selectSupport(c);
            }

            // shutdown/startup
            bool restart() {
                return true; //k->deselectSupport(c) && k->selectSupport(c);
            }

            bool shutdown() {
                return true; //k->deselectSupport(c);
            }

            bool unload(){
                return k->unloadSupport( c );
            }
        };
    }
}

#endif
