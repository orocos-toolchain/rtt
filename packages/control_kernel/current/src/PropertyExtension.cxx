/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  PropertyExtension.cxx 

                        PropertyExtension.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#pragma implementation
#include "control_kernel/PropertyExtension.hpp"
#include <corelib/PropertyComposition.hpp>
#include <corelib/PropertyBag.hpp>

namespace ORO_ControlKernel
{
    using namespace std;

    bool PropertyComponentInterface::enableAspect( PropertyExtension* ext)
    {
        if (master == 0)
            {
                master = ext;
                return master->addComponent( this );
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

    PropertyExtension::PropertyExtension(ControlKernelInterface* _base ) 
        : detail::ExtensionInterface(_base, "Property"),
          save_props("SaveProperties","Not implemented yet.",false),
          configureOnLoad("ConfigureOnLoad","Configure the component when loaded," \
                          "instead of when the kernel is started", true),
          base(_base)
    {
    }

    PropertyExtension::~PropertyExtension()
    {
        for( CompNames::iterator it = componentFileNames.begin();
             it != componentFileNames.end(); ++it )
            delete *it;
    }

//     TaskInterface* PropertyExtension::getTask() const
//     {
//         return base->getTask();
//     }
    
//     void PropertyExtension::setTask( TaskInterface* task )
//     {
//         base->setTask( task );
//     }

    bool PropertyExtension::updateProperties(const PropertyBag& bag)
    {
        composeProperty(bag, save_props);
        composeProperty(bag, configureOnLoad);
            
        // build new list of present component config files
        for( CompNames::iterator it = componentFileNames.begin();
             it != componentFileNames.end(); ++it )
            delete *it;
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
                cout <<"sequence \"PropertyFiles\" not found !"<< endl;
                return false;
            }
        return true;
    }
        
    bool PropertyExtension::initialize()
    {
        if (configureOnLoad)
            return true; // All is done.

        cout << "PropertyExtension::initialize on start." << endl;
        // read xml file for each component, if we know it.
        for ( CompNames::iterator it = componentFileNames.begin(); it!= componentFileNames.end(); ++it)
            {
                CompMap::iterator tg = myMap.find( (*it)->getName() );
                if ( tg == myMap.end() )
                    {
                        cout << "Component "<<(*it)->getName() << " not found !"<<endl;
                        continue;
                    }
                configureComponent( (*it)->value(), tg->second );
            }
        return true;
    }

    bool PropertyExtension::configureComponent(const std::string& filename, PropertyComponentInterface* target)
    {
        ComponentConfigurator cc;
        if ( !cc.configure( filename, target) )
            {
                cout << "Component "<< target->getName()
                     << " does not accept its properties."<< endl 
                     << "Fix your config file first."<< endl;
                return false;
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
        
    bool PropertyExtension::addComponent(PropertyComponentInterface* comp)
    {
        if ( myMap.count(comp->getLocalStore().getName() ) != 0  )
            return false;
        myMap[ comp->getLocalStore().getName() ] = comp;
        if ( configureOnLoad )
            {
                for ( CompNames::iterator it = componentFileNames.begin(); it!= componentFileNames.end(); ++it)
                    if ( (*it)->getName() == comp->getLocalStore().getName() )
                        return configureComponent( (*it)->value(), comp );
                // reached when not found
                //cerr << "Warning : No property file for "<<comp->getLocalStore().getName()<<endl;
            }
        return true;
    }

    void PropertyExtension::removeComponent(PropertyComponentInterface* comp)
    {
        myMap.erase( comp->getLocalStore().getName() );
    }


}
