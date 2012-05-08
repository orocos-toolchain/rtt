#ifndef ORO_CHANNEL_CONVERSION_ELEMENT_HPP
#define ORO_CHANNEL_CONVERSION_ELEMENT_HPP

#include "../base/ChannelElement.hpp"

namespace RTT { namespace internal {

    /** A connection element that accepts one kind of data and
     * returns another kind, provided that one is convertable
     * to the other. The RTT constructor interface in the type
     * system is used for that.
     */
        template<typename T_In, typename T_Out>
        class ChannelConversionElement : 
        public virtual base::ChannelElement<T_In>,
        public virtual base::ChannelElement<T_Out>
    {
        DataSource<T_Out>::shared_ptr mconverter;
    public:
        typedef boost::intrusive_ptr< ChannelElement<T_In> > input_shared_ptr;
        typedef boost::intrusive_ptr< ChannelElement<T_Out> > output_shared_ptr;
        typedef typename boost::call_traits<T>::param_type param_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        shared_ptr getOutput()
        {
            assert( boost::static_pointer_cast< base::ChannelElement<T_Out> >(
                    ChannelElementBase::getOutput())
                    == boost::dynamic_pointer_cast< base::ChannelElement<T_Out> >(
                       ChannelElementBase::getOutput()));
             return boost::static_pointer_cast< base::ChannelElement<T_Out> >(
                    ChannelElementBase::getOutput());
        }

        shared_ptr getInput()
        {
            assert( boost::static_pointer_cast< base::ChannelElement<T_In> >(
                    ChannelElementBase::getInput())
                    == boost::dynamic_pointer_cast< base::ChannelElement<T_In> >(
                       ChannelElementBase::getInput()));
            return boost::static_pointer_cast< base::ChannelElement<T_In> >(
                    ChannelElementBase::getInput());
        }

        typedef typename base::ChannelElement<T_In>::param_t param_t;
        typedef typename base::ChannelElement<T_Out>::reference_t reference_t;

        ChannelConversionElement(DataSource<T_Out>::shared_ptr conv) : mconverter(conv) {}

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
                    sample = converter->get(); // copy
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

