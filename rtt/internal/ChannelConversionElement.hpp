#ifndef ORO_CHANNEL_CONVERSION_ELEMENT_HPP
#define ORO_CHANNEL_CONVERSION_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "DataSource.hpp"

namespace RTT { namespace internal {

    /** A connection element that accepts one kind of data and
     * returns another kind, provided that one is convertible
     * to the other. The RTT constructor interface in the type
     * system is used for that.
     */
        template<typename T_In, typename T_Out>
        class ChannelConversionElement : 
        public virtual base::ChannelElement<T_In>,
        public virtual base::ChannelElement<T_Out>
    {
        typename internal::DataSource<T_Out>::shared_ptr out_storage;
        typename internal::DataSource<T_In>::shared_ptr in_storage;
    public:
        typedef typename boost::intrusive_ptr< base::ChannelElement<T_In> > input_shared_ptr;
        typedef typename boost::intrusive_ptr< base::ChannelElement<T_Out> > output_shared_ptr;

        output_shared_ptr getOutput()
        {
            assert( boost::static_pointer_cast< base::ChannelElement<T_Out> >(
                    base::ChannelElementBase::getOutput())
                    == boost::dynamic_pointer_cast< base::ChannelElement<T_Out> >(
                       base::ChannelElementBase::getOutput()));
             return boost::static_pointer_cast< base::ChannelElement<T_Out> >(
                    base::ChannelElementBase::getOutput());
        }

        input_shared_ptr getInput()
        {
            assert( boost::static_pointer_cast< base::ChannelElement<T_In> >(
                    base::ChannelElementBase::getInput())
                    == boost::dynamic_pointer_cast< base::ChannelElement<T_In> >(
                       base::ChannelElementBase::getInput()));
            return boost::static_pointer_cast< base::ChannelElement<T_In> >(
                    base::ChannelElementBase::getInput());
        }

        typedef typename base::ChannelElement<T_In>::param_t param_t;
        typedef typename base::ChannelElement<T_Out>::reference_t reference_t;

        ChannelConversionElement(DataSource<T_Out> out) : out_storage(out) {}

        /** Only a read() can start a conversion. No write may be done to this element*/
        virtual bool write(param_t sample)
        {
            return false;
        }

        /** Return converted data.
         *
         * @return FlowStatus
         */
        virtual FlowStatus read(reference_t sample, bool copy_old_data)
        {
            input_shared_ptr input = this->getInput();
            if (input) {
                FlowStatus fs = input->read( in_storage, copy_old_data); // copy to in_storage
                if (fs)
                	sample= out_storage.getTypeInfo()->convert(in_storage);
                return fs;
            } else
                return NoData;
        }

        /** Resets the stored sample. After clear() has been called, read()
         * returns false
         */
        virtual void clear()
        {
            base::ChannelElement<T_In>::clear();
        }

        virtual bool data_sample(param_t sample)
        {
            return base::ChannelElement<T_Out>::data_sample(sample);
        }

    };
}}

#endif

