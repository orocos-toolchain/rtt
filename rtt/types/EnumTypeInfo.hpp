#include "../rtt-config.h"
#include "Types.hpp"
#include "TemplateTypeInfo.hpp"
#include "TemplateConstructor.hpp"

namespace RTT
{
    namespace types
    {

        /**
         * Type information for Enum types for which they are convertible
         * to int.
         *
         * @see StructTypeInfo
         */
        template<class T>
        struct EnumTypeInfo: public TemplateTypeInfo<T, false>
        {
        private:
            static int enum_to_int(T e)
            {
                return (int) e;
            }

            static T int_to_enum(int i)
            {
                return (T) i;
            }
        public:
            EnumTypeInfo(const char* type) :
                TemplateTypeInfo<T, false> (type)
            {
                if (!Types()->type("int")) {
                    log(Error) << "Failed to register enum<-> int conversion because type int is not known in type system."<<endlog();
                } else {
                    Types()->type("int")->addConstructor(newConstructor(
                            &EnumTypeInfo<T>::enum_to_int, true));
                }
                this->addConstructor( newConstructor( &EnumTypeInfo<T>::int_to_enum, true) );
            }

            /**
             * Composition also checks if source is an int, and if so, converts
             * it to the enum.
             */
            virtual bool composeType(base::DataSourceBase::shared_ptr source,
                    base::DataSourceBase::shared_ptr result) const
            {
                // First, try a plain update.
                if (result->update(source.get()))
                    return true;
                // try conversion from int to enum:
                internal::DataSource<int>::shared_ptr ds =
                        internal::DataSource<int>::narrow( source.get() );
                if (ds)
                {
                    typename internal::AssignableDataSource<T>::shared_ptr menum =
                            internal::AssignableDataSource<T>::narrow( source.get() );
                    assert(menum);
                    menum->set( (T)ds->get() );
                    return true;
                }
                return false;
            }
        };
    }
}
