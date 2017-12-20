/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ExecutableInterface.hpp

                        ExecutableInterface.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef EXECUTABLE_INTERFACE_HPP
#define EXECUTABLE_INTERFACE_HPP

#include "../rtt-fwd.hpp"
#include "../rtt-config.h"

#include <cassert>

namespace RTT
{
    namespace base {

        /**
         * Objects that implement this interface are to be
         * executed in the ExecutionEngine. One can insert
         * and remove objects that implement this interface
         * safely in and out the ExecutionEngine.
         *
         * Be careful that all these functions may be called
         * from a hard real-time context.
         */
        class RTT_API ExecutableInterface
        {
        protected:
            ExecutionEngine* engine;
        public:
            /**
             * Called by the ExecutionEngine \a ee or before
             * synchronous execution to tell
             * this object it is being loaded. The engine
             * pointer is set first and the user's loading() function
             * is called next.
             * @param ee The pointer to the engine calling us.
             */
            void loaded(ExecutionEngine* ee) {
                if (!engine) {
                    engine = ee;
                    this->loading();
                }
                assert(ee == engine);
            }

            /**
             * Called by the ExecutionEngine \a ee or after
             * synchronous execution to tell
             * this object it is being unloaded. The
             * user's loading() function
             * is called first and the engine pointer is cleared next.
             */
            void unloaded() {
                if (engine) {
                    this->unloading();
                    engine = 0;
                }
            }

            ExecutableInterface() : engine(0) {}
            virtual ~ExecutableInterface() {}

            /**
             * Informs this object that it got loaded
             * in an ExecutionEngine.
             * Called by load() after the engine pointer is set.
             * @crt
             */
            virtual void loading() {}

            /**
             * Returns true if this object is loaded in an ExecutionEngine.
             * @return true if so.
             */
            bool isLoaded() { return engine != 0; }

            /**
             * Returns the ExecutionEngine this object is loaded into or
             * null otherwise.
             * @return
             */
            ExecutionEngine* getEngine() { return engine; }

            /**
             * Executes a piece of functionality.
             * Called by the ExecutionEngine directly.
             * @return false if this object should no longer be executed.
             * @crt
             */
            virtual bool execute() = 0;

            /**
             * Informs this object that it got unloaded
             * from an ExecutionEngine.
             * Called by unload() before the engine pointer is cleared.
             * @crt
             */
            virtual void unloading() {}
        };
    }
}
#endif
