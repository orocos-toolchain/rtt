#ifndef MOTIONPROPERTYEXTENSION_HPP
#define MOTIONPROPERTYEXTENSION_HPP

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include <corelib/PropertyComposition.hpp>
#include "KernelInterfaces.hpp"
#include "ComponentConfigurator.hpp"

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    
    class PropertyExtension;

    /**
     * A Component using properties can be extended with this interface
     * to export them to the user or update them with user defined values.
     */
    class PropertyComponentInterface
        :public ComponentAspectInterface<PropertyExtension>
    {
        friend class PropertyExtension;
        friend class ComponentConfigurator;
    public:
        /**
         * The Default constructor 
         */
        PropertyComponentInterface( const std::string& _name )
            : ComponentAspectInterface<PropertyExtension>(_name + std::string("::Property") ),
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
         * Get the name of the component with the aspect.
         */
        const std::string& getName() const
        {
            return aspectName;
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
     * A ControlKernel Extension for Properties.
     * It will configure all components in the Kernel with the properties found in the
     * cpf files. 
     * The properties of the PropertyExtension itself contain the filenames 
     * of the component property files.
     */
    class PropertyExtension
        : public ExtensionInterface
    {
        public:
        typedef PropertyComponentInterface CommonBase;
        typedef unsigned int KeyType;

        /**
         * Create a PropertyExtension.
         */
        PropertyExtension(KernelBaseFunction* _base ) 
            : ExtensionInterface("Property"),
              // marshaller(out), demarshaller(in),
              save_props("SaveProperties","",false), generator("Generator","filename"),
              estimator("Estimator","filename"), controller("Controller","filename"), sensor("Sensor","filename"),
              effector("Effector","filename"), base(_base)
        {
        }

        virtual ~PropertyExtension()
        {
        }

        virtual bool updateProperties(const PropertyBag& bag)
        {
            composeProperty(bag, save_props);
            
            // build new list of present component config files
            componentFileNames.clear();
            if ( composeProperty(bag, generator ) )
                componentFileNames.push_back( &generator );
            if ( composeProperty(bag, estimator ) )
                componentFileNames.push_back( &estimator );
            if ( composeProperty(bag, controller ) )
                componentFileNames.push_back( &controller );
            if ( composeProperty(bag, sensor ) )
                componentFileNames.push_back( &sensor );
            if ( composeProperty(bag, effector ) )
                componentFileNames.push_back( &effector );
            return true;
        }
        
        /**
         * Read the XML cpf files and try to configure all the components.
         */
        virtual bool initialize()
        {
            cout << "PropertyExtension::initialize" << endl;
            // read xml file for each component, if we know it.
            for ( CompNames::iterator it = componentFileNames.begin(); it!= componentFileNames.end(); ++it)
            {
                CompMap::iterator tg = myMap.find( (*it)->getName() );
                if ( tg == myMap.end() )
                {
                    cout << "Component "<<(*it)->getName() << " does not support properties !"<<endl;
                    continue;
                }
                PropertyComponentInterface* target = tg->second;

                ComponentConfigurator cc;
                if ( !cc.configure( (*it)->value(), target) && base )
                {
                    cout << "Aborting initialize ! Fix your config file first."<<endl;
                    return false;
                }
            }
            return true;
        }

        virtual void step()
        {
            // do nothing ?
        }

        /**
         * If needed, save the components properties.
         */
        virtual void finalize()
        {
            if ( save_props )
            {
                std::cout << "Need to save props !"<<std::endl;
                /*
                // iterate over components
                std::map<std::string, PropertyComponentInterface*>::iterator comp_it = myMap.begin();
                while ( comp_it != myMap.end() )
                {
                    // collect properties
                }
                // serialize and cleanup
                marshaller.serialize( allProps );
                newProps.clear();
                */
            }
        }
        
        /**
         * Used by the PropertyComponentInterface to register itself to
         * this Extension.
         */
        void addComponent(PropertyComponentInterface* comp)
        {
            myMap[ comp->getLocalStore().getName() ] = comp;
        }

        /**
         * Used by the PropertyComponentInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(PropertyComponentInterface* comp)
        {
            myMap.erase( comp->getLocalStore().getName() );
        }

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
         * Property. Filename of the generator component's property file.
         */
        Property<std::string> generator;
        /**
         * Property. Filename of the estimator component's property file.
         */
        Property<std::string> estimator;
        /**
         * Property. Filename of the controller component's property file.
         */
        Property<std::string> controller;
        /**
         * Property. Filename of the sensor component's property file.
         */
        Property<std::string> sensor;
        /**
         * Property. Filename of the effector component's property file.
         */
        Property<std::string> effector;

        /**
         * The base kernel of this extension.
         */
        KernelBaseFunction* base;
    };
}        
#endif
