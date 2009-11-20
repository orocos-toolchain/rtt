/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  InputPortSource.hpp

                        InputPortSource.hpp -  description
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


#ifndef ORO_INPUT_PORT_SOURCE_HPP
#define ORO_INPUT_PORT_SOURCE_HPP

#include "Channels.hpp"
#include "../base/PortInterface.hpp"

namespace RTT
{ namespace internal {

    /** This class represents a read port using the data source interface.
     * Beware that, depending on the connection used, ports actually change
     * their state when read. For instance, a buffer connection *will* lose one
     * element when get() or evaluate() are called. For that reason, it is
     * considered bad practice to use a data source on a read port that is bound
     * to a buffer connection.
     *
     * This class should not be used directly in normal code. What you would
     * usually do is create a new read port using OutputPort::antiClone() and
     * call base::InputPortInterface::getDataSource() to get the corresponding data
     * source.  This is your duty to destroy the port when it is not needed
     * anymore.
     */
    template<typename T>
    class InputPortSource : public DataSource<T>
    {
        InputPort<T>& port;
        mutable T mvalue;

    public:
        InputPortSource(InputPort<T>& port)
            : port(port), mvalue() { }

        void reset() { port.clear(); }
        bool evaluate() const
        {
            return port.read(mvalue) == NewData;
        }

        typename DataSource<T>::result_t value() const
        { return mvalue; }
        typename DataSource<T>::result_t get() const
        {
            if ( evaluate() )
                return value();
            else
                return T();
        }
        DataSource<T>* clone() const
        { return new InputPortSource<T>(port); }
        DataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        { return const_cast<InputPortSource<T>*>(this); }
    };
}}

#endif

