/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  GlobalsRepository.hpp

                        GlobalsRepository.hpp -  description
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


#ifndef ORO_GLOBALS_REPOSITORY_HPP
#define ORO_GLOBALS_REPOSITORY_HPP

#include "rtt/ConfigurationInterface.hpp"

namespace RTT {
    namespace types {
        /**
         * An RTT::ConfigurationInterface for holding global variables.
         * The RTT uses this to export enumerations to scripting,
         * but users may extend this as well for their own applications.
         *
         * It is recommended to use the setValue() function to add
         * globals, such that they become owned by the GlobalsRepository.
         */
        class RTT_API GlobalsRepository
            : public RTT::ConfigurationInterface
        {
        public:
            typedef boost::shared_ptr<GlobalsRepository> shared_ptr;

            static shared_ptr Instance();
            /**
             * Frees all globals.
             */
            static void Release();
        private:
            static shared_ptr mInstance;
        };            
    }
}

#endif
