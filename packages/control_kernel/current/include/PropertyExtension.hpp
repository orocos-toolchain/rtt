/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  PropertyExtension.hpp 

                        PropertyExtension.hpp -  description
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
 
 
#ifndef MOTIONPROPERTYEXTENSION_HPP
#define MOTIONPROPERTYEXTENSION_HPP

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include "KernelInterfaces.hpp"
#include "ComponentConfigurator.hpp"

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    
    class PropertyExtension;

    /**
     * @brief A Component using properties can be extended with this interface
     * to export them to the user or update them with user defined values.
     * @see PropertyExtension
     */
    class PropertyComponentInterface
        :public detail::ComponentAspectInterface<PropertyExtension>
    {
        friend class PropertyExtension;
        friend class ComponentConfigurator;
    public:
        /**
         * The Default constructor 
         */
        PropertyComponentInterface( const std::string& _name )
            : detail::ComponentAspectInterface<PropertyExtension>(_name + std::string("::Property") ),
              master(0), localStore(_name,"Component Properties")
        {
        }

        virtual bool enableAspect( PropertyExtension* ext);

        virtual void disableAspect();

        /**
         * Destructor.
         */
        virtual ~PropertyComponentInterface()
        {
        }

        /**
         * Update the Component's properties with a bag.
         */
        virtual bool updateProperties( const PropertyBag& bag)
        {
            return true;
        }
        
        /**
         * Export the Component's properties into a bag.
         */
        virtual void exportProperties( PropertyBag& bag )
        {}
    private:
        /**
         * Used by the PropertyExtension to access the components property bag.
         */
        Property<PropertyBag>& getLocalStore() { return localStore; }

        /**
         * The Kernel we belong to.
         */
        PropertyExtension* master;

        /**
         * This bag is used by the PropertyExtension to store
         * the properties of the component itself. It is as such
         * not owned by the component, but used by the PropertyExtension as
         * temporary storage. Virtually anything may be stored in here.
         */
        Property<PropertyBag> localStore;
    };

    /**
     * @brief A ControlKernel Extension for Properties.
     * It will configure all components in the Kernel with the properties found in the
     * cpf files.
     * 
     * The properties of the PropertyExtension itself contain the filenames 
     * of the component property files.
     * @see PropertyComponentInterface
     */
    class PropertyExtension
        : public detail::ExtensionInterface
    {
        public:
        typedef PropertyComponentInterface CommonBase;
        typedef unsigned int KeyType;

        /**
         * Create a PropertyExtension.
         */
        PropertyExtension(KernelBaseFunction* _base = 0 );

        virtual ~PropertyExtension();

        virtual bool updateProperties(const PropertyBag& bag);
        
        /**
         * Read the XML cpf files and try to configure all the components.
         */
        virtual bool initialize();

        virtual void step();

        /**
         * If needed, save the components properties.
         */
        virtual void finalize();

        virtual TaskInterface* getTask() const;

        virtual void setTask( TaskInterface* task );

        /**
         * Used by the PropertyComponentInterface to register itself to
         * this Extension.
         */
        bool addComponent(PropertyComponentInterface* comp);

        /**
         * Used by the PropertyComponentInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(PropertyComponentInterface* comp);

        bool configureComponent(const std::string& filename, PropertyComponentInterface* target);
        private:
        /**
         * Mapping of Component name to component instances.
         */
        typedef std::map<std::string, PropertyComponentInterface*> CompMap;
        
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
        
        /**
         * Property. True if components properties must be saved afterwards.
         */
        Property<bool> save_props;

        /**
         * Set to true if a component needs to be configured when loaded,
         * Set to false if a component needs to be configured when the kernel is started.
         */
        Property<bool> configureOnLoad;

        /**
         * The base kernel of this extension.
         */
        KernelBaseFunction* base;
    };
}        
#endif
