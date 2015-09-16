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

#include <boost/bind.hpp>
#ifndef USE_CPP11
#include <boost/lambda/lambda.hpp>
#endif

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
             return ChannelElementBase::narrow<T>(ChannelElementBase::getOutput().get());
        }

        shared_ptr getInput()
        {
            return ChannelElementBase::narrow<T>(ChannelElementBase::getInput().get());
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
        virtual FlowStatus write(param_t sample)
        {
            typename ChannelElement<T>::shared_ptr output = getOutput();
            if (output)
                return output->write(sample);
            return NotConnected;
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

        /**
         * Overwritten implementation of MultipleInputsChannelElementBase::removeInput() that resets the current channel on removal.
         */
        virtual void removeInput(ChannelElementBase *input)
        {
            if (input == cur_input) {
                cur_input.reset();
            }
            MultipleInputsChannelElementBase::removeInput(input);
        }

        /**
         * Overwritten implementation of MultipleInputsChannelElementBase::data_sample() that gets a sample from the currently selected input.
         */
        virtual value_t data_sample()
        {
            typename ChannelElement<T>::shared_ptr input = cur_input;
            if (input) {
                return input->data_sample();
            }
            return value_t();
        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        virtual FlowStatus read(reference_t sample, bool copy_old_data)
        {
            FlowStatus result = NoData;
            // read and iterate if necessary.
#ifdef USE_CPP11
            select_reader_channel( bind( &MultipleInputsChannelElement<T>::do_read, this, boost::ref(sample), boost::ref(result), _1, _2), copy_old_data );
#else
            select_reader_channel( boost::bind( &MultipleInputsChannelElement<T>::do_read, this, boost::ref(sample), boost::ref(result), boost::lambda::_1, boost::lambda::_2), copy_old_data );
#endif
            return result;
        }

    private:
        bool do_read(reference_t sample, FlowStatus& result, bool copy_old_data, typename ChannelElement<T>::shared_ptr& input)
        {
            assert( result != NewData );
            if ( input ) {
                FlowStatus tresult = input->read(sample, copy_old_data);
                // the result trickery is for not overwriting OldData with NoData.
                if (tresult == NewData) {
                    result = tresult;
                    return true;
                }
                // stores OldData result
                if (tresult > result)
                    result = tresult;
            }
            return false;
        }

        /**
         * Selects a connection as the current channel
         * if pred(connection) is true. It will first check
         * the current channel ( getCurrentChannel() ), if that
         * does not satisfy pred, iterate over \b all connections.
         * If none satisfy pred, the current channel remains unchanged.
         * @param pred
         */
        template<typename Pred>
        void select_reader_channel(Pred pred, bool copy_old_data) {
            RTT::os::SharedMutexLock lock(inputs_lock);
            typename ChannelElement<T>::shared_ptr new_input =
                find_if(pred, copy_old_data);

            if (new_input)
            {
                // We don't clear the current channel (to get it to NoData state), because there is a race
                // between find_if and this line. We have to accept (in other parts of the code) that eventually,
                // all channels return 'OldData'.
                cur_input = new_input;
            }
        }

        template<typename Pred>
        typename ChannelElement<T>::shared_ptr find_if(Pred pred, bool copy_old_data) {
            // We only copy OldData in the initial read of the current channel.
            // if it has no new data, the search over the other channels starts,
            // but no old data is needed.
            if ( cur_input )
                if ( pred( copy_old_data, cur_input ) )
                    return cur_input;

            Inputs::iterator result;
            for (result = inputs.begin(); result != inputs.end(); ++result) {
                if (*result == cur_input) continue;
                typename ChannelElement<T>::shared_ptr input = (*result)->narrow<T>();
                assert(input);
                if ( pred(false, input) == true)
                    return input;
            }
            return typename ChannelElement<T>::shared_ptr();
        }

    private:
        typename ChannelElement<T>::shared_ptr cur_input;
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
            bool result = true;
            for(Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            {
                typename ChannelElement<T>::shared_ptr output = (*it)->narrow<T>();
                if (!output->data_sample(sample)) {
                    result = false;
                    // TODO: disconnect channel
                }
            }
            return result;
        }

        /** Writes a new sample on this connection. \a sample is the sample to
         * write. Writes the sample to all connected channels
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual FlowStatus write(param_t sample)
        {
            RTT::os::SharedMutexLock lock(outputs_lock);
            if (outputs.empty()) return NotConnected;
            FlowStatus result = WriteSuccess;
            for(Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            {
                typename ChannelElement<T>::shared_ptr output = (*it)->narrow<T>();
                FlowStatus fs = output->write(sample);
                if (fs != WriteSuccess) {
                    if (isMandatory(output.get())) {
                        if (fs == NotConnected)
                            result = NotConnected;
                        else if (fs == WriteFailure && result == WriteSuccess)
                            result = WriteFailure;
                    }

                    // TODO: disconnect channel if fs == NotConnected
                }
            }
            return result;
        }
    };

    /** A typed version of MultipleInputsMultipleOutputsChannelElementBase.
     */
    template<typename T>
    class MultipleInputsMultipleOutputsChannelElement : public MultipleInputsMultipleOutputsChannelElementBase, public MultipleInputsChannelElement<T>, public MultipleOutputsChannelElement<T>
    {
    public:
        using typename ChannelElement<T>::value_t;
        typedef boost::intrusive_ptr< MultipleInputsMultipleOutputsChannelElement<T> > shared_ptr;
        using typename ChannelElement<T>::param_t;
        using typename ChannelElement<T>::reference_t;

        using MultipleInputsChannelElement<T>::data_sample;
        using MultipleOutputsChannelElement<T>::data_sample;

        using MultipleInputsMultipleOutputsChannelElementBase::disconnect;
    };
}}

#endif

