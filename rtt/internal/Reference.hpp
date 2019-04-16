#ifndef RTT_INTERNAL_REFERENCE_HPP
#define RTT_INTERNAL_REFERENCE_HPP

#include "../base/DataSourceBase.hpp"

namespace RTT 
{
    namespace internal {
        /**
         * Object that may receive a reference to some data by means of a
         * pointer or data source.
         * Used by ReferenceDataSource in order to allow to modify the reference
         * after creation of the data source.
         */
        class Reference 
        {
        public:
            virtual ~Reference() {};

            /**
             * Sets the reference to a given pointer. The pointer must have the
             * same type as the data referenced. No type checking is done, so
             * make sure you get this right.
             */
            virtual void setReference(void* ref) = 0;
            /**
             * Sets the reference to the data contained in the data source.
             * @param dsb Must be an assignable data source of the same type
             * of the data. The shared pointer is not necessarily stored, so
             * keep track of the lifetime of \a dsb, that it lives longer
             * than this object.
	     * @return false if setting failed. More precisely
	     * this might fail if dynamic_cast'ing the argument to the
	     * Reference DataSource fails.
	     * @note This method will also call evaluate on the dsb to
	     * assure the correct value is used.
             */
            virtual bool setReference(base::DataSourceBase::shared_ptr dsb) = 0;
        };
    }
}

#endif
