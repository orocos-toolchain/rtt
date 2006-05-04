#ifndef ORO_CORELIB_DATASOURCES_HPP
#define ORO_CORELIB_DATASOURCES_HPP

#include "DataSource.hpp"

namespace ORO_CoreLib
{

    /**
     * A simple, yet very useful DataSource, which keeps a value, and
     * returns it in its get() method. 
     * This is an AssignableDataSource, which
     * thus can be changed. 
     * @param T The result data type of get().
     */
    template<typename T>
    class ValueDataSource
        : public AssignableDataSource<T>
    {
        typename DataSource<T>::value_t mdata;
    protected:
        /**
         * Use shared_ptr.
         */
        ~ValueDataSource();
    public:
        typedef boost::intrusive_ptr<ValueDataSource<T> > shared_ptr;

        ValueDataSource( T data );

        ValueDataSource( );

        typename DataSource<T>::result_t get() const;

        typename DataSource<T>::result_t value() const;

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set();

        virtual ValueDataSource<T>* clone() const;

        virtual ValueDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replace ) const;
    };

    /**
     * Specialisation for const std::string& to keep capacity when set( ... ) is called.
     */
    template<>
    void ValueDataSource<const std::string&>::set(  AssignableDataSource<const std::string&>::param_t t );


    /**
     * A DataSource which holds a constant value and
     * returns it in its get() method. It can not be changed after creation.
     * @param T Any type of data, except being a non-const reference.
     */
    template<typename T>
    class ConstantDataSource
        : public DataSource<T>
    {
        /**
         * Assure that mdata is const, such that T is forced
         * to not be a non-const reference.
         */
        typename boost::add_const<typename DataSource<T>::value_t>::type mdata;
    protected:
        /**
         * Use shared_ptr.
         */
        ~ConstantDataSource();
    public:
        typedef boost::intrusive_ptr< ConstantDataSource<T> > shared_ptr;

        ConstantDataSource( T value );

        typename DataSource<T>::result_t get() const;

        typename DataSource<T>::result_t value() const;

        virtual ConstantDataSource<T>* clone() const;

        virtual ConstantDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

    /**
     * A DataSource which is used to manipulate a reference to an
     * external value.
     * @param T The result data type of get().
     */
    template<typename T>
    class ReferenceDataSource
        : public AssignableDataSource<T>
    {
        // a reference to a value_t
        typename AssignableDataSource<T>::reference_t mref;
    protected:
        /**
         * Use shared_ptr.
         */
        ~ReferenceDataSource();
    public:
        typedef boost::intrusive_ptr<ReferenceDataSource<T> > shared_ptr;

        ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref );

        typename DataSource<T>::result_t get() const;

        typename DataSource<T>::result_t value() const;

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set();

        virtual ReferenceDataSource<T>* clone() const;

        virtual ReferenceDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

}

#include "DataSources.inl"

#endif

