/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  PropertyUpdaterExtension.hpp 

                        PropertyUpdaterExtension.hpp -  description
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
 
#ifndef PROPERTYUPDATEREXTENSION_HPP
#define PROPERTYUPDATEREXTENSION_HPP

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include <corelib/PropertyComposition.hpp>
//#include <lrl/frames.h>
#include <corelib/marshalling/SimpleMarshaller.hpp>
#include <corelib/marshalling/SimpleDemarshaller.hpp>
#include <corelib/RunnableInterface.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <control_kernel/ComponentInterfaces.hpp>
//#include <corelib/CPFDemarshaller.hpp>
//#include <xercesc/framework/LocalFileInputSource.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    
    class PropertyUpdaterExtension;

    /**
     * A Component using properties can be extended with this interface
     * to export them to the user or update them with user defined values.
     */
    class PropertyUpdaterComponentInterface
        :public detail::ComponentAspectInterface<PropertyUpdaterExtension>
    {
        friend class PropertyUpdaterExtension;
    public:
        /**
         * The Default constructor 
         */
        PropertyUpdaterComponentInterface( const std::string& _name )
            : detail::ComponentAspectInterface<PropertyUpdaterExtension>(_name + std::string("::PropertyUpdater") ),
              master(0), localStore(_name,"Component Runtime Properties")
        {
        }

        virtual bool enableAspect( PropertyUpdaterExtension* ext);

        virtual void disableAspect();

        /**
         * Destructor.
         */
        virtual ~PropertyUpdaterComponentInterface()
        {
        }

        /**
         * Register local properties so that an external application
         * can modify them.
         */
        virtual void registerProperties( PropertyBag& bag)
        {
        }
        
    private:
        /**
         * Used by the PropertyUpdaterExtension to access the components property bag.
         */
        Property<PropertyBag>& getLocalStore() { return localStore; }

        /**
         * The Kernel we belong to.
         */
        PropertyUpdaterExtension* master;

        /**
         * This bag is used by the PropertyUpdaterExtension to store
         * the properties of the component itself. It is as such
         * not owned by the component, but used by the PropertyUpdaterExtension as
         * temporary storage. Virtually anything may be stored in here.
         */
        Property<PropertyBag> localStore;
    };

    /**
     * A ControlKernel Extension for changing Properties at runtime.
     * It is used for updating properties at runtime, from another not realtime
     * source, like a GUI.
     */
    class PropertyUpdaterExtension
        : public detail::ExtensionInterface
    {
        public:
        typedef PropertyUpdaterComponentInterface CommonBase;
        typedef unsigned int KeyType;

        /**
         * Create a PropertyUpdaterExtension.
         */
        PropertyUpdaterExtension(ControlKernelInterface* k) 
            : detail::ExtensionInterface(k, "PropertyUpdater")
        {
            // create GUI
        }

        virtual ~PropertyUpdaterExtension()
        {
        }

        virtual bool updateProperties(const PropertyBag& bag)
        {
            return true;
        }
        
        virtual bool initialize()
        {
            return true;
        }

        virtual void step()
        {
            // show / update registered properties
        }

        virtual void finalize()
        {
        }
        
        /**
         * Used by the PropertyUpdaterComponentInterface to register itself to
         * this Extension.
         */
        void addComponent(PropertyUpdaterComponentInterface* comp)
        {
            myMap[ comp->getLocalStore().getName() ] = comp;
        }

        /**
         * Used by the PropertyUpdaterComponentInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(PropertyUpdaterComponentInterface* comp)
        {
            myMap.erase( comp->getLocalStore().getName() );
        }

        private:
        /**
         * Mapping of Component name to component instances.
         */
        typedef std::map<std::string, PropertyUpdaterComponentInterface*> CompMap;
        
        /**
         * A vector of Component names.
         */
        typedef std::vector< Property<std::string>* >              CompNames;
        
        /**
         * A mapping of every registered components name to its instance.
         */
        CompMap     myMap;
        /**
         * A list of all the component filenames.
         */
        CompNames   componentFileNames;
        
    };
}        
#endif
