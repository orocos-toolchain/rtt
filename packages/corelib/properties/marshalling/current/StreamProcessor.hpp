#ifndef STREAMPROCESSOR_HPP
#define STREAMPROCESSOR_HPP

namespace ORO_CoreLib
{

    /**
     * An interface for setting and getting a
     * stream object. This can be a file, a string buffer,...
     */
    template<class Stream>
    class StreamProcessor
    {
        public:
        /**
         * Create a StreamProcessor.
         * @param _s The stream to be processed.
         */
        StreamProcessor( Stream& _s )
            :s(&_s)
        {}
        
        /**
         * Set a new Stream.
         * @param _s The stream to be processed.
         */
        void setStream( Stream& _s)
        {
            s = &_s;
        }

        /**
         * Get the current Stream
         * @return The current Stream.
         */
        Stream& getStream()
        {
            return *s;
        }
        
        protected:
        Stream* s;
    };

}

#endif
