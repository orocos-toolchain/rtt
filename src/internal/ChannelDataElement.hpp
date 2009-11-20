/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ChannelDataElement.hpp

                        ChannelDataElement.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CHANNEL_DATA_ELEMENT_HPP
#define ORO_CHANNEL_DATA_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "../base/DataObjectInterface.hpp"

namespace RTT { namespace internal {

    /** A connection element that stores a single data sample
     */
    template<typename T>
    class ChannelDataElement : public base::ChannelElement<T>
    {
        bool written, mread;
        typename base::DataObjectInterface<T>::shared_ptr data;

    public:
        typedef typename base::ChannelElement<T>::param_t param_t;
        typedef typename base::ChannelElement<T>::reference_t reference_t;

        ChannelDataElement(typename base::DataObjectInterface<T>::shared_ptr sample)
            : written(false), mread(false), data(sample) {}

        /** Update the data sample stored in this element.
         * It always returns true. */
        virtual bool write(param_t sample)
        {
            data->Set(sample);
            written = true;
            mread = false;
            return this->signal();
        }

        /** Reads the last sample given to write()
         *
         * @return false if no sample has ever been written, true otherwise
         */
        virtual FlowStatus read(reference_t sample)
        {
            if (written)
            {
                data->Get(sample);
                if ( !mread ) {
                    mread = true;
                    return NewData;
                }
                return OldData;
            }
            return NoData;
        }

        /** Resets the stored sample. After clear() has been called, read()
         * returns false
         */
        virtual void clear()
        {
            written = false;
            mread = false;
            base::ChannelElement<T>::clear();
        }

        virtual bool data_sample(param_t sample)
        {
            data->data_sample(sample);
            return base::ChannelElement<T>::data_sample(sample);
        }

    };
}}

#endif

