#ifndef PI_PROPERTIES_MARSHALLER
#define PI_PROPERTIES_MARSHALLER

#include <string>

namespace ORO_CoreLib
{

    class PropertyBag;
	template<typename T> class Property;

	/**
     * @brief Used for converting properties to a format.
	 * An interface which all classes which serialize property bags
	 * should implement.
	 *
	 * @see Demarshaller
	 * @see Property
	 * @see PropertyBag
	 */
    class Marshaller
    {
        public:
			/**
			 * Serialize a property of type bool.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<bool> &v) = 0;
			
			/**
			 * Serialize a property of type char.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<char> &v) = 0;
			
			/**
			 * Serialize a property of type int.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<int> &v) = 0;
			
			/**
			 * Serialize a property of type double.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<double> &v) = 0;
			
			/**
			 * Serialize a property of type string.
			 * @param v The property to be serialized.
			 */
			virtual void serialize(const Property<std::string> &v) = 0;
			
			/**
			 * Serialize a property of type property bag. This enables
			 * nested property bags.
			 * @param v The property to be serialized.
			 */
            virtual void serialize(const Property<PropertyBag> &v) = 0;
			
			/**
			 * Serialize the contents of a property bag.
			 * @param v The property bag to be serialized.
			 */
            virtual void serialize(const PropertyBag &v) = 0;

            /**
             * Flush all buffers.
             * Instructs the Marshaller to flush any remaining buffered
             * data.
             */
            virtual void flush() = 0;
	};

	/**
     * @brief Used for extracting properties from a format.
	 * An interface which all classes which deserialize data
	 * should implement.
	 *
	 * @see Marshaller
	 * @see Property
	 * @see PropertyBag
	 */
	class Demarshaller
	{
		public:
			/**
			 * Deserialize data to a property bag.
			 * @param v The property bag which will contain the results
             *          of the deserialization.
			 * @see PropertyBag
			 */
            virtual bool deserialize(PropertyBag &v) = 0;
    };
} // Namespace ORO_CoreLib
#endif
