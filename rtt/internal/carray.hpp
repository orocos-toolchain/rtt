#ifndef ORO_CARRAY_HPP_
#define ORO_CARRAY_HPP_

#include <boost/serialization/array.hpp>


namespace RTT
{
    namespace internal {

        /**
         * Wraps a C array such that we can return a C array
         * from a DataSource.
         * Inspired on boost::serialization::array.
         *
         * Default copy-constructible and assignable.
         * An assignment makes a 'shallow copy', all copies
         * refer to the same C array.
         *
         * You can also point to parts of arrays. An uninitialized
         * carray object returns null for both count() and address().
         *
         * @param T The data type of the array.
         */
        template<class T>
        class carray
        {
        public:
            typedef T value_type;

            /**
             * Create an C array wrapper.
             * @param t Pointer to first element of array.
             * @param s Length of array. If zero, the parameter \a t
             * will be ignored.
             */
            carray(value_type* t, std::size_t s) :
                m_t( s ? t : 0),
                m_element_count(s)
            {}

            carray() : m_t(0), m_element_count(0) {}

            /**
             * We are constructible from boost::serialization::array<T>
             * @param orig
             */
            carray( boost::serialization::array<T> const& orig)
            : m_t( orig.address() ), m_element_count( orig.count() ) {
                if (m_element_count == 0)
                    m_t = 0;
            }

            /**
             * The address of the first element of the array
             * @return null if count() == 0, the address otherwise.
             */
            value_type* address() const
            {
              return m_t;
            }

            /**
             * @return The number of elements in the array.
             */
            std::size_t count() const
            {
              return m_element_count;
            }

        private:
            value_type* m_t;
            std::size_t m_element_count;
        };
    }
}

#endif /* ORO_CARRAY_HPP_ */
