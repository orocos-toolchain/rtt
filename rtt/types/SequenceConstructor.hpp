#ifndef SEQUENCECONSTRUCTOR_HPP_
#define SEQUENCECONSTRUCTOR_HPP_

#include "TypeBuilder.hpp"
#include "../internal/DataSources.hpp"
#include <vector>

namespace RTT
{
    namespace types
    {
        /**
         * Sequence constructor which takes the number of elements in the sequence
         */
        template<class T>
        struct sequence_ctor: public std::unary_function<int, const std::vector<
                T>&>
        {
            typedef const std::vector<T>& ( Signature)(int);
            mutable boost::shared_ptr<std::vector<T> > ptr;
            sequence_ctor() :
                ptr(new std::vector<T>())
            {
            }
            const std::vector<T>& operator()(int size) const
            {
                ptr->resize(size);
                return *(ptr);
            }
        };

        /**
         * See NArityDataSource which requires a function object like
         * this one.
         */
        template<class T>
        struct sequence_varargs_ctor
        {
            typedef const std::vector<T>& result_type;
            typedef T argument_type;
            result_type operator()(const std::vector<T>& args) const
            {
                return args;
            }
        };

        /**
         * Helper DataSource for constructing sequences with a variable number of
         * parameters.
         */
        template<class T>
        struct sequence_constructor_datasource {
            typedef internal::NArityDataSource<sequence_varargs_ctor<T> > type;
        };

        /**
         * Constructs an sequence with \a n elements, which are given upon
         * construction time.
         */
        template<class T>
        struct SequenceBuilder: public TypeBuilder
        {
            virtual base::DataSourceBase::shared_ptr build(const std::vector<
                    base::DataSourceBase::shared_ptr>& args) const
            {
                if (args.size() == 0)
                    return base::DataSourceBase::shared_ptr();
                typename sequence_constructor_datasource<T>::type::shared_ptr vds = new typename sequence_constructor_datasource<T>::type();
                for (unsigned int i = 0; i != args.size(); ++i)
                {
                    typename internal::DataSource<T>::shared_ptr dsd =
                            boost::dynamic_pointer_cast<internal::DataSource<T> >(
                                    args[i]);
                    if (dsd)
                        vds->add(dsd);
                    else
                        return base::DataSourceBase::shared_ptr();
                }
                return vds;
            }

        };

        /**
         * Constructs a sequence from the number of elements and a prototype
         * element for these elements.
         */
        template<class T>
        struct sequence_ctor2: public std::binary_function<int, T,
                const std::vector<T>&>
        {
            typedef const std::vector<T>& ( Signature)(int, T);
            mutable boost::shared_ptr<std::vector<T> > ptr;
            sequence_ctor2() :
                ptr(new std::vector<T>())
            {
            }
            const std::vector<T>& operator()(int size, T value) const
            {
                ptr->resize(size);
                ptr->assign(size, value);
                return *(ptr);
            }
        };

    }
}

#endif /* SEQUENCECONSTRUCTOR_HPP_ */
