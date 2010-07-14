#ifndef EXECUTABLE_INTERFACE_HPP
#define EXECUTABLE_INTERFACE_HPP

#include "../rtt-fwd.hpp"
#include "../rtt-config.h"

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
             * Called by the ExecutionEngine \a ee to tell
             * this object it is being loaded. The engine
             * pointer is set first and the user's loading() function
             * is called next.
             * @param ee The pointer to the engine calling us.
             */
            void loaded(ExecutionEngine* ee) { engine = ee; this->loading();}

            /**
             * Called by the ExecutionEngine \a ee to tell
             * this object it is being unloaded. The
             * user's loading() function
             * is called first and the engine pointer is cleared next.
             */
            void unloaded() { this->unloading(); engine = 0;}

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
