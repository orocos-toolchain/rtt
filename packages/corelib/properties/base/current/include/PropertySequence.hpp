#ifndef PI_PROPERTY_SEQUENCE_HPP
#define PI_PROPERTY_SEQUENCE_HPP

#include "PropertyBase.hpp"

namespace ORO_CoreLib
{
	/**
	 * @brief A container for a sequence of properties of the same type Property<T> [EXPERIMENTAL]
     * 
	 * This class groups properties of equal types into a single
	 * container. It is used for optimised access to fixed type contents. It
     * is thus very much like a vector.
     *
     * A PropertySequence object can handed to a Marshaller object
	 * which will serialize the contents of the PropertySequence.
     *
     * All operations on a PropertySequence are non recursive. The PropertySequence
     * itself is not aware of the possible hierarchical structure. You need
     * to use the global scope functions for handling hierarchical structures.
     *
     * A PropertySequence is not the owner of the properties within. It defines
     * a group of properties, nothing more. When this group of properties
     * contains another PropertySequence, this sequence is considered as a 'pointer'
     * to other properties. Even such a sequence is not owned by the PropertySequence.
     *
     * Constructing PropertySequences.
     * It can happen in an application that a PropertySequence is filled with
     * dynamically created Property objects. The sequence is not responsible for
     * cleaning up these objects once they are no longer needed. The application
     * which uses the PropertySequence has to find a way of deleting the possible
     * dangling pointers.
     *
     * The elements in a PropertySequence are <em>unordered</em>. Operations on the
     * sequence may change the relative order of the elements.
     * 
	 * @see PropertyBag
     * @param T The type of the sequence.
     * @note THIS CLASS IS EXPERIMENTAL AND SHOULD NOT BE USED BY THE UNWARY
	 */
    template<class T>
    class PropertySequence
    {
        public:
			typedef std::vector<PropertyBase*> PropertyContainerType;
			typedef PropertyContainerType::iterator iterator;
            typedef PropertyContainerType::const_iterator const_iterator;

			/**
			 * The default constructor.
			 */
            PropertySequence( )
                : _properties(), type("type_less")
            {}

			/**
			 * The typed constructor.
             *
             * @param type The type of PropertySequence.
			 */
            PropertySequence( const std::string& _type)
                : _properties(), type(_type)
            {}

            /**
             * The copy constructor.
             * The copy constructor of the PropertySequence makes
             * non-deep copies of its elements.
             */
            PropertySequence( const PropertySequence& orig)
                : _properties( orig.getProperties() ), type( orig.getType() )
            {
#if 0
                const_iterator i( orig.getProperties().begin() );
                while ( i != orig.getProperties().end() )
                {
                    add( (*i) );
                    ++i;
                }
#endif
            }

			/**
			 * Add a property to the container.
			 * @param p Pointer to the property to be added.
			 */
            void add(PropertyBase *p)
            {
                _properties.push_back(p);
            }
            
			/**
			 * Remove a property from the container.
			 * @param p Pointer to the property to be removed.
			 */
			void remove(PropertyBase *p)
            {
                iterator i = _properties.begin();
                i = _properties.end();
				i = std::find(_properties.begin(), _properties.end(), p);
                if ( i != _properties.end() )
                    _properties.erase(i);
            }

            /**
             * Removes all PropertyBases from this sequence, without deleting
             * them. This is a not recursive function.
             */
            void clear()
            {
                _properties.clear();
            }
                

			/**
			 * List all properties.
			 * @param names The container which will be used to store all
			 * available property names in.
			 */
            void list(std::vector<std::string> &names) const
            {
                for (
                    const_iterator i = _properties.begin();
                    i != _properties.end();
                    i++ )
                {
					names.push_back( (*i)->getName() );
                }
            }

            /**
             * Find the PropertyBase with name <name>.
             *
             * @param  name The name of the property to search for.
             * @return The PropertyBase with this name, zero
             *         if it does not exist.
             */
            PropertyBase* find(const std::string& name) const
            {
				const_iterator i( std::find_if(_properties.begin(), _properties.end(), std::bind2nd(FindProp(), name ) ) );
                if ( i != _properties.end() )
                    return ( *i );
                return 0;
            }
            
            /**
             * This assignment assigns all
             * PropertyBases of another sequence in this sequence, making
             * an exact copy or the original.
             */
            PropertySequence& operator=(const PropertySequence& orig)
            {
                _properties.clear();

                const_iterator i = orig.getProperties().begin();
                while (i != orig.getProperties().end() )
                {
                    add( (*i) );
                    ++i;
                }
                return *this;
            }

            /**
             * The update operator. 
             * It updates this sequence so it contains all PropertyBases
             * of another sequence, removing own PropertyBases if duplicate names
             * exist in the source sequence.
             */
            PropertySequence& operator<<=(const PropertySequence& source)
            {
                //iterate over orig, update or clone PropertyBases
                const_iterator it(source.getProperties().begin());
                while ( it != source.getProperties().end() )
                {
                    PropertyBase* mine = find( (*it)->getName() );
                    if (mine != 0)
                        remove(mine);
                    add( (*it) );
                    ++it;
                }
                return *this;
            }
            
            const std::string& getType() const { return type;}

			const PropertyContainerType& getProperties() const { return _properties; }

        protected:
			PropertyContainerType _properties;

            /**
             * A function object for finding a Property by name.
             */
            struct FindProp : public std::binary_function<const PropertyBase*,const std::string, bool>
            {
                bool operator()(const PropertyBase* b1, const std::string& b2) const { return b1->getName() == b2; }
            };

            const std::string type;
    };


    /**
     * Some helper functions to retrieve PropertyBases from
     * PropertySequences.
     *
     * @param sequence The sequence to look for a Property.
     * @param nameSequence A sequence of names, separated by
     *        a double colon indicating the path in the sequence to a property,
     *        omitting the name of the <sequence> itself.
     */
    PropertyBase* find(const PropertySequence& sequence, const std::string& nameSequence, const std::string& separator = std::string("::") );


    /**
     * @group SequenceOperations These functions operate on the contents of sequences,
     *        possibly modifying, deleting or creating new Property objects.
     * @{
     */
    
    /**
     * This function refreshes the values of the properties in one PropertySequence with
     * the values of the properties of another PropertySequence.
     * No new properties will be created.
     * 
     * You can use this function to update the properties of a fixed sequence.
     */
    void refreshProperties(PropertySequence& target, const PropertySequence& source);
    
    /**
     * @brief This function updates the values of Property objects of one Sequence with the 
     * values of Property objects of another sequence. 
     * It creates new Property instances using if a Property
     * is not present in the target and class copy on that Property.
     * 
     * You can use this function to add a copy of the contents of a property sequence.
     */
    void copyProperties(PropertySequence& target, const PropertySequence& source);
        
    /**
     * This function iterates over a PropertySequence and deletes all Property objects in
     * it without recursion.
     */
    void deleteProperties(PropertySequence& target);

    /**
     * @brief This function flattens a PropertySequence recursively.
     *
     * The names of the Propety objects of the included sequences are
     * placed in this sequence with the included sequence's name prefixed. If the Property object
     * in that sequence is also a sequence, the same operation is performed recursively.
     * So any sequence in <target> will show up at the
     * root of <target> with the path prefixed.
     */
    void flattenPropertySequence(PropertySequence& target, const std::string& separator="::");
    
    /**
     * @}
     */

    /**
     * Updating a sequence is actually refreshing the sequence.
     */
    inline
    void update(PropertySequence& a, const PropertySequence& b)
    {
        refreshProperties(a,b);
    }

    /**
     * Copying a sequence is actually making a deep copy of the sequence.
     */
    inline
    void copy(PropertySequence& a, const PropertySequence& b)
    {
        std::cout << "Copy Sequence "<<std::endl;
        copyProperties(a,b);
    }

} // Namespace ORO_CoreLib
#endif
