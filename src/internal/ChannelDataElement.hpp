#ifndef ORO_CHANNEL_DATA_ELEMENT_HPP
#define ORO_CHANNEL_DATA_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "../base/DataObjectInterface.hpp"

namespace RTT {
    /** A connection element that stores a single data sample
     */
    template<typename T>
    class ChannelDataElement : public ChannelElement<T>
    {
        bool written;
        typename DataObjectInterface<T>::shared_ptr data;

    public:
        typedef typename ChannelElement<T>::param_t param_t;
        typedef typename ChannelElement<T>::reference_t reference_t;

        ChannelDataElement(typename DataObjectInterface<T>::shared_ptr sample)
            : written(false), data(sample) {}

        /** Update the data sample stored in this element.
         * It always returns true. */
        virtual bool write(param_t sample)
        {
            data->Set(sample);
            written = true;
            return this->signal();
        }

        /** Reads the last sample given to write()
         *
         * @return false if no sample has ever been written, true otherwise
         */
        virtual bool read(reference_t sample)
        {
            if (written)
            {
                data->Get(sample);
                return true;
            }
            return false;
        }

        /** Resets the stored sample. After clear() has been called, read()
         * returns false
         */
        virtual void clear()
        {
            written = false;
            ChannelElement<T>::clear();
        }
    };
}

#endif

