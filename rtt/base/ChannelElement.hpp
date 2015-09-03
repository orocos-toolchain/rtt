/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  ChannelElement.hpp

                        ChannelElement.hpp -  description
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


#ifndef ORO_CHANNEL_ELEMENT_HPP
#define ORO_CHANNEL_ELEMENT_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>
#include "ChannelElementBase.hpp"
#include "../FlowStatus.hpp"
#include "../os/MutexLock.hpp"


namespace RTT { namespace base {


    /** A typed version of ChannelElementBase. It defines generic methods that are
     * type-specific (like write and read)
     */
    template<typename T>
    class ChannelElement : virtual public ChannelElementBase
    {
    public:
        typedef T value_t;
        typedef boost::intrusive_ptr< ChannelElement<T> > shared_ptr;
        typedef typename boost::call_traits<T>::param_type param_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        shared_ptr getOutput()
        {
             return ChannelElementBase::getOutput()->narrow<T>();
        }

        shared_ptr getInput()
        {
            return ChannelElementBase::getInput()->narrow<T>();
        }

        /**
         * Provides a data sample to initialize this connection.
         * This is used before the first write() in order to inform this
         * connection of the size of the data. As such enough storage
         * space can be allocated before the actual writing begins.
         *
         * @returns false if an error occured that requires the channel to be invalidated.
         */
        virtual bool data_sample(param_t sample)
        {
            typename ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< ChannelElement<T> >(getOutput());
            if (output)
                return output->data_sample(sample);
            return false;
        }

        virtual value_t data_sample()
        {
            typename ChannelElement<T>::shared_ptr input = boost::static_pointer_cast< ChannelElement<T> >(getInput());
            if (input)
                return input->data_sample();
            return value_t();
        }

        /** Writes a new sample on this connection. \a sample is the sample to
         * write. 
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool write(param_t sample)
        {
            typename ChannelElement<T>::shared_ptr output = getOutput();
            if (output)
                return output->write(sample);
            return false;
        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        virtual FlowStatus read(reference_t sample, bool copy_old_data)
        {
            typename ChannelElement<T>::shared_ptr input = this->getInput();
            if (input)
                return input->read(sample, copy_old_data);
            else
                return NoData;
        }
    };

    /** A typed version of MultipleInputsChannelElementBase.
     */
    template<typename T>
    class MultipleInputsChannelElement : virtual public MultipleInputsChannelElementBase, virtual public ChannelElement<T>
    {
    public:
        using typename ChannelElement<T>::value_t;
        typedef boost::intrusive_ptr< MultipleInputsChannelElement<T> > shared_ptr;
        using typename ChannelElement<T>::param_t;
        using typename ChannelElement<T>::reference_t;

//        virtual value_t data_sample()
//        {
//            typename ChannelElement<T>::shared_ptr input = boost::static_pointer_cast< ChannelElement<T> >(getInput());
//            if (input)
//                return input->data_sample();
//            return value_t();
//        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
//        virtual FlowStatus read(reference_t sample, bool copy_old_data)
//        {
//            typename ChannelElement<T>::shared_ptr input = this->getInput();
//            if (input)
//                return input->read(sample, copy_old_data);
//            else
//                return NoData;
//        }
    };

    /** A typed version of MultipleOutputsChannelElementBase.
     */
    template<typename T>
    class MultipleOutputsChannelElement : virtual public MultipleOutputsChannelElementBase, virtual public ChannelElement<T>
    {
    public:
        using typename ChannelElement<T>::value_t;
        typedef boost::intrusive_ptr< MultipleInputsChannelElement<T> > shared_ptr;
        using typename ChannelElement<T>::param_t;
        using typename ChannelElement<T>::reference_t;

        virtual bool data_sample(param_t sample)
        {
            RTT::os::SharedMutexLock lock(outputs_lock);
            if (outputs.empty()) return false;
            for(Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            {
                typename ChannelElement<T>::shared_ptr output = (*it)->narrow<T>();
                output->data_sample(sample);
            }
            return true;
        }

        /** Writes a new sample on this connection. \a sample is the sample to
         * write. Writes the sample to all connected channels
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool write(param_t sample)
        {
            RTT::os::SharedMutexLock lock(outputs_lock);
            if (outputs.empty()) return false;
            for(Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            {
                typename ChannelElement<T>::shared_ptr output = (*it)->narrow<T>();
                output->write(sample);
            }
            return true;
        }
    };

    /** A typed version of MultipleInputsMultipleOutputsChannelElementBase.
     */
    template<typename T>
    class MultipleInputsMultipleOutputsChannelElement : public MultipleInputsMultipleOutputsChannelElementBase, public MultipleInputsChannelElement<T>, public MultipleOutputsChannelElement<T>
    {
    public:
        using typename ChannelElement<T>::value_t;
        typedef boost::intrusive_ptr< MultipleInputsChannelElement<T> > shared_ptr;
        using typename ChannelElement<T>::param_t;
        using typename ChannelElement<T>::reference_t;

        using MultipleInputsMultipleOutputsChannelElementBase::disconnect;
    };
}}

#endif

