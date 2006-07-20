#ifndef ORO_NA_HPP
#define ORO_NA_HPP

#include <boost/type_traits.hpp>

namespace RTT
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
        public:
            typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type value_type;
            static T na() { return Gna; }
        private:
            static value_type Gna;
        };

        /**
         * Specialisation to return a void.
         */
        template<>
        struct NA<void>
        {
            typedef void value_type;
            static void na() {};
        };

        template<class T>
        typename NA<T>::value_type NA<T>::Gna;
    }
}
#endif
