#ifndef ORO_NA_HPP
#define ORO_NA_HPP

#include <boost/type_traits.hpp>

namespace ORO_Execution
{
    namespace detail
    {
        /**
         * This class is used to return a 'default' value
         * when no value is available ('Not Available').
         */
        template<class T>
        struct NA
        {
            typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type value_type;
            static value_type na;
        };

        template<class T>
        typename NA<T>::value_type NA<T>::na;
    }
}
#endif
