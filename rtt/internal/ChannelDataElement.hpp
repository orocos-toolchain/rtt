/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ChannelDataElement.hpp

                        ChannelDataElement.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

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
#include "../ConnPolicy.hpp"

namespace RTT { namespace internal {

    /** A connection element that stores a single data sample
     */
    template<typename T>
    class ChannelDataElement : public base::ChannelElement<T>
    {
        typename base::DataObjectInterface<T>::shared_ptr data;
        const ConnPolicy policy;

    public:
        typedef typename base::ChannelElement<T>::value_t value_t;
        typedef typename base::ChannelElement<T>::param_t param_t;
        typedef typename base::ChannelElement<T>::reference_t reference_t;

        ChannelDataElement(typename base::DataObjectInterface<T>::shared_ptr sample, const ConnPolicy& policy = ConnPolicy())
            : data(sample), policy(policy) {}

        /** Update the data sample stored in this element.
         * It always returns true. */
        virtual WriteStatus write(param_t sample)
        {
            if (!data->Set(sample)) return WriteFailure;
            return this->signal() ? WriteSuccess : NotConnected;
        }

        /** Reads the last sample given to write()
         *
         * @return false if no sample has ever been written, true otherwise
         */
        virtual FlowStatus read(reference_t sample, bool copy_old_data)
        {
            return data->Get(sample, copy_old_data);
        }

        /** Resets the stored sample. After clear() has been called, read()
         * returns false
         */
        virtual void clear()
        {
            data->clear();
            base::ChannelElement<T>::clear();
        }

        virtual WriteStatus data_sample(param_t sample, bool reset = true)
        {
            if (!data->data_sample(sample, reset)) return WriteFailure;
            return base::ChannelElement<T>::data_sample(sample, reset);
        }

        virtual value_t data_sample()
        {
            return data->data_sample();
        }

        /** Returns a pointer to the ConnPolicy that has been used to construct the underlying data object.
        */
        virtual const ConnPolicy* getConnPolicy() const
        {
            return &policy;
        }

        virtual std::string getElementName() const
        {
            return "ChannelDataElement";
        };
    };
}}

#endif

