#ifndef PI_PROPERTYBASE
#define PI_PROPERTYBASE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>

#include "OperationAcceptor.hpp"
#include "PropertyIntrospection.hpp"
#include "PropertyMutatingIntrospection.hpp"

namespace ORO_CoreLib
{

    using namespace detail;
	class Marshaller;

	/**
	 * Virtual base class for all property classes.
	 *
	 */
    class PropertyBase
        : public OperationAcceptor
    {
        public:
			
			/**
			 * The default constructor.
			 */
            PropertyBase()
            {}

			/**
			 * The default destructor.
			 */
            virtual ~PropertyBase()
            {}

			/**
			 * The constructor.
			 * @param name The name which will be used to refer to this
			 * property.
			 * @param description A more elaborate description of the
			 * property.
			 */
            PropertyBase( std::string name, std::string description)
                    : _name(name), _description(description)
            {}

			/**
			 * Get the name of the property.
			 * @return name of the property.
			 */
            const std::string& getName() const
            {
                return _name;
            }

			/**
			 * Set the name of the property.
			 * @param name The name of the property.
			 */
            void setName(const std::string& name)
            {
                _name = name;
            }

			/**
			 * Get a description of the property.
			 * @return description of the property.
			 */
            const std::string& getDescription() const
            {
                return _description;
            }

			/**
			 * Set the description of the property.
			 * @param desc The description of the property.
			 */
            void setDescription(const std::string& desc)
            {
                _description = desc;
            }

            /**
             * A call on this method will lead to a call to the PropertyIntrospection
             * interface identifying this Property's proper type.
             */
            virtual void identify( PropertyIntrospection* pi) const = 0;

						virtual void mutate( PropertyMutatingIntrospection * pmi) = 0;

            /**
             * Updates the value of the Property with the value of
             * another Property.
             */
            virtual bool update(const PropertyBase* other) = 0;

            /**
             * Copies the value, name and description of another
             * Property into this property.
             */
            virtual bool copy(const PropertyBase* other) = 0;

            /**
             * Deliver an identical clone of this PropertyBase. The
             * original may be deleted and the clone can be transparantly
             * used in its place or vice versa.
             */
            virtual PropertyBase* clone() const = 0;

            /**
             * Create a new default instance of the PropertyBase.
             * This is a factory method to 'make something of the same type'.
             * The new PropertyBase has the same name and description as this.
             */
            virtual PropertyBase* create() const = 0;
        protected:
            /**
             * A short name for this PropertyBase.
             */
            std::string _name;

            /**
             * A lengthy description for this PropertyBase.
             */
            std::string _description;
        private:

    };
} // namespace ORO_CoreLib
#endif
