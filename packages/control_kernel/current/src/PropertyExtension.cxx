#include "control_kernel/PropertyExtension.hpp"
#include <corelib/PropertyComposition.hpp>
#include <corelib/PropertyBag.hpp>

namespace ORO_ControlKernel
{
    bool PropertyComponentInterface::enableAspect( PropertyExtension* ext)
    {
        if (master == 0)
            {
                master = ext;
                master->addComponent( this );
                return true;
            }
        return false;
    }

    void PropertyComponentInterface::disableAspect()
    {
        if (master !=0)
            {
                master->removeComponent( this );
                master = 0;
            }
    }

    PropertyExtension::PropertyExtension(KernelBaseFunction* _base ) 
        : detail::ExtensionInterface("Property"),
          save_props("SaveProperties","",false),
          base(_base)
    {
    }

    PropertyExtension::~PropertyExtension()
    {
    }

    TaskInterface* PropertyExtension::getTask() const
    {
        return base->getTask();
    }
    
    void PropertyExtension::setTask( TaskInterface* task )
    {
        base->setTask( task );
    }

    bool PropertyExtension::updateProperties(const PropertyBag& bag)
    {
        composeProperty(bag, save_props);
            
        // build new list of present component config files
        componentFileNames.clear();

        PropertyBase* res = bag.find("PropertyFiles");
        if ( res && dynamic_cast< Property< PropertyBag >*>( res ) )
            {
                PropertyBag& bag =
                    dynamic_cast<Property<PropertyBag>* >( res )->value();

                for( PropertyBag::iterator it = bag.getProperties().begin();
                     it !=bag.getProperties().end();
                     ++it)
                    if ( dynamic_cast< Property<string>*>( *it ) )
                        componentFileNames.push_back
                            ( new Property<std::string>( *dynamic_cast< Property<string>*>(*it) ) );
            }
        else
            {
                return false;
            }
        return true;
    }
        
    bool PropertyExtension::initialize()
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
                        cout << "Aborting initialize ! Component "<< (*it)->getName()
                             << " does not accept its properties."<< endl 
                             << "Fix your config file first."<< endl;
                        return false;
                    }
            }
        return true;
    }

    void PropertyExtension::step()
    {
        // do nothing ?
    }

    void PropertyExtension::finalize()
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
        
    void PropertyExtension::addComponent(PropertyComponentInterface* comp)
    {
        myMap[ comp->getLocalStore().getName() ] = comp;
    }

    void PropertyExtension::removeComponent(PropertyComponentInterface* comp)
    {
        myMap.erase( comp->getLocalStore().getName() );
    }


}
