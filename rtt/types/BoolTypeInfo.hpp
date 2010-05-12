#include "TemplateTypeInfo.hpp"
#include <ostream>
#include <istream>

namespace RTT
{
    namespace types
    {

        /**
         * Write boolean as 'true' or 'false'.
         */
        struct BoolTypeInfo: public TemplateTypeInfo<bool>
        {
            BoolTypeInfo() :
                TemplateTypeInfo<bool> ("bool")
            {
            }

            virtual std::ostream& write(std::ostream& os, base::DataSourceBase::shared_ptr in) const
            {
#ifdef OS_HAVE_STREAMS
                internal::DataSource<bool>* d = internal::AdaptDataSource<bool> ()(in);
                if (d)
                    return os << boolalpha << d->value();
#endif
                return os;
            }

            virtual std::istream& read(std::istream& os, base::DataSourceBase::shared_ptr out) const
            {
#ifdef OS_HAVE_STREAMS
                internal::AssignableDataSource<bool>::shared_ptr d = internal::AdaptAssignableDataSource<bool> ()(out);
                if (d)
                {
                    boolalpha(os);
                    os >> d->set();
                    d->updated(); // because use of set().
                }
#endif
                return os;
            }
        };

    }
}
