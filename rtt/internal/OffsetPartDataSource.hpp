#ifndef ORO_OFFSETPARTDATASOURCE_HPP_
#define ORO_OFFSETPARTDATASOURCE_HPP_

namespace RTT
{
    namespace internal
    {
        /**
         * Calculates an offset to an element in memory, based on a type size and an index.
         * OffsetDataSources can be chained one to the other, in order to locate subsequences
         * of subsequences.
         * This data source is used in conjunction with OffsetPartDataSource or other data sources
         * in ordert to manipulate a (sub)element in a (sub)sequence somewhere in memory.
         * Setting this structure up is intricate and only meant for expert type system developers.
         */
        class OffsetDataSource
        : public DataSource<unsigned int>
        {
        public:
            typedef unsigned int offset_type;

            ~OffsetDataSource() {}

            typedef boost::intrusive_ptr<OffsetDataSource > shared_ptr;

            /**
             * The offset is size times the index plus any parent offset.
             * @param size   The size of the element referenced to by index.
             * @param index  Datasource pointing to the index for use in operator[]
             * @param parent_offset The offset to which the parent may be subject itself.
             */
            OffsetDataSource( offset_type size,
                              DataSource<unsigned int>::shared_ptr index,
                              DataSource<offset_type>::shared_ptr parent_offset)
                : msize(size), mindex(index), mparent_offset(parent_offset)
            {
            }

            DataSource<offset_type>::result_t get() const
            {
                // calculates the location of the element sequence.
                if (mparent_offset)
                    return msize * mindex->get() + mparent_offset->get();
                return msize * mindex->get();
            }

            DataSource<offset_type>::result_t value() const
            {
                return get();
            }

            virtual OffsetDataSource* clone() const {
                return new OffsetDataSource(msize, mindex, mparent_offset);
            }

            virtual OffsetDataSource* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const {
                // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
                if ( replace[this] != 0 ) {
                    assert ( dynamic_cast<OffsetDataSource*>( replace[this] ) == static_cast<OffsetDataSource*>( replace[this] ) );
                    return static_cast<OffsetDataSource*>( replace[this] );
                }
                // Other pieces in the code rely on insertion in the map :
                replace[this] = new OffsetDataSource(msize, mindex->copy(replace), mparent_offset->copy(replace));
                // return this instead of a copy.
                return static_cast<OffsetDataSource*>(replace[this]);

            }
        private:
            // size of one element.
            const unsigned int msize;
            // [] index
            DataSource<unsigned int>::shared_ptr mindex;
            // parent offset
            DataSource<offset_type>::shared_ptr mparent_offset;
        };

        /**
         * A DataSource which is used to manipulate a reference to a
         * part of a data source holding a sequence of elements.
         *
         * It supports referencing to elements of sequences in sequences.
         *
         * It requires to know which 'top level' element it is reading
         * parts of, this is called the \a parent. It also requires proper
         * offset calculation which takes into account a shift in memory
         * location in case the one of the higher level elements is also a
         * sequence. The offset calculation is done by an OffsetDataSource.
         *
         *
         * @param T The data type of an element.
         */
        template<typename T>
        class OffsetPartDataSource
        : public AssignableDataSource<T>
        {
            // a reference to a value_t
            typename AssignableDataSource<T>::value_t* mref;
            // [] index
            DataSource<unsigned int>::shared_ptr mindex;
            // parent data source, for updating after set().
            base::DataSourceBase::shared_ptr mparent;
            // offset in memory, calculated by external means.
            DataSource<unsigned int>::shared_ptr moffset;
        public:
            ~OffsetPartDataSource();

            typedef boost::intrusive_ptr<OffsetPartDataSource<T> > shared_ptr;

            /**
             * The part is constructed from a reference to an existing array of elements and
             * an index for accessing the n'th element.
             * @param ref    Reference to the first element of the sequence
             * @param index  Datasource pointing to the index for use in operator[]
             * @param parent Parent data source for sending updated() messages. This should not be a part itself, but a ValueDataSource. May \b not be null.
             * @param offset The offset to the requested element, relative to \a ref, to use in calls to set() and get().
             */
            OffsetPartDataSource( typename AssignableDataSource<T>::reference_t ref,
                                    DataSource<unsigned int>::shared_ptr index,
                                    base::DataSourceBase::shared_ptr parent,
                                    DataSource<unsigned int>::shared_ptr offset)
                : mref(&ref), mindex(index), mparent(parent), moffset( offset)
            {
            }

            typename DataSource<T>::result_t get() const
            {
                // calculates the location of the element sequence.
                return static_cast<T*>( static_cast<void*>(mref) + moffset->get() )[ mindex->get() ];
            }

            typename DataSource<T>::result_t value() const
            {
                return get();
            }

            void set( typename AssignableDataSource<T>::param_t t )
            {
                set() = t;
                updated();
            }

            typename AssignableDataSource<T>::reference_t set()
            {
                // calculates the location of the element sequence.
                return static_cast<T*>( static_cast<void*>(mref) + moffset->get() )[ mindex->get() ];
            }

            typename AssignableDataSource<T>::const_reference_t rvalue() const
            {
                // calculates the location of the element sequence.
                return static_cast<T*>( static_cast<void*>(mref) + moffset->get() )[ mindex->get() ];
            }

            void updated() {
                mparent->updated();
            }

            /**
             * Overloaded to pass on our own parent to the type system,
             * such that offsets can be correctly calculated.
             *
             * @return The parent object that holds all data.
             */
            virtual shared_ptr getParent() {
                return mparent;
            }


            virtual OffsetPartDataSource<T>* clone() const {
                return new OffsetPartDataSource<T>(mref, mindex, mparent, moffset);
            }

            virtual OffsetPartDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const {
                // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
                if ( replace[this] != 0 ) {
                    assert ( dynamic_cast<OffsetPartDataSource<T>*>( replace[this] ) == static_cast<OffsetPartDataSource<T>*>( replace[this] ) );
                    return static_cast<OffsetPartDataSource<T>*>( replace[this] );
                }
                // Other pieces in the code rely on insertion in the map :
                replace[this] = new OffsetPartDataSource<T>(mref, mindex->copy(replace), mparent->copy(replace), moffset->copy(replace));
                // return this instead of a copy.
                return static_cast<OffsetPartDataSource<T>*>(replace[this]);

            }
        };

    }
}

#endif /* ORO_PARTDATASOURCE_HPP_ */
