#ifndef PROPERTYBAGINTROSPECTOR
#define PROPERTYBAGINTROSPECTOR

#include "PropertyBag.hpp"
#include <stack>

namespace ORO_CoreLib
{

    /**
     * @brief This class fills up a given bag with
     * the results of an introspection.
     */
    class PropertyBagIntrospector
        : public PropertyIntrospection
    {
        std::stack<PropertyBag*> mystack;
    public:
        /**
         * @brief Create a new Introspector, saving the results in a bag.
         *
         * One can choose to save the given properties or a clone.
         *
         * @param bag The bag to safe the results in.
         * @param clone True if the bag must contain clones, false (default)
         * if the bag must contain the original properties.
         */
        PropertyBagIntrospector( PropertyBag& bag )
        {
            mystack.push( &bag );
        }

        virtual ~PropertyBagIntrospector()
        {
            mystack.pop();
        }

        /**
         * Reset to do a new introspection.
         * @param new_bag The new bag to save the results in.
         */
        void reset( PropertyBag& new_bag)
        {
            mystack.pop();
            mystack.push( &new_bag );
        }

        virtual void introspect(const Property<bool> &v)
        {
            mystack.top()->add( v.clone() );
        }
        
        virtual void introspect(const Property<char> &v)
        {
            mystack.top()->add( v.clone() );
        }
        
        virtual void introspect(const Property<int> &v)
        {
            mystack.top()->add( v.clone() );
        }
        
        virtual void introspect(const Property<double> &v)
        {
            mystack.top()->add( v.clone() );
        }
			
        virtual void introspect(const Property<std::string> &v)
        {
            mystack.top()->add( v.clone() );
        }
        
        virtual void introspect(const Property<PropertyBag> &v)
        {
            PropertyBag* cur_bag = mystack.top();
            Property<PropertyBag>* bag_cl
                = new Property<PropertyBag>( v.getName(),
                                             v.getDescription(),
                                             PropertyBag( v.get().getType() ) );
            cur_bag->add( bag_cl );

            mystack.push( &bag_cl->value() );
            this->introspect( v.get() );
            mystack.pop();
        }

        virtual void introspect( const PropertyBag& v )
        {
            for (
                 PropertyBag::const_iterator i = v.getProperties().begin();
                 i != v.getProperties().end();
                 i++ )
                {
                    (*i)->identify(this);
                }
        }
    };
}

#endif
