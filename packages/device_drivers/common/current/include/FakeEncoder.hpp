#ifndef FAKEENCODER_HPP
#define FAKEENCODER_HPP


namespace ORO_DeviceDriver
{
    /**
     * @brief A Dummy Encoder implementation
     */
    class FakeEncoder
        : public ORO_DeviceInterface::EncoderInterface
    {
        int turn, pos;
    public:
        FakeEncoder() : turn(0), pos(0) {}
        virtual ~FakeEncoder() {}

        /**
         * Get the position within the current turn.
         */
        virtual int positionGet() const { return pos; }

        /**
         * Get the current turn.
         */
        virtual int turnGet() const { return turn; }
           
        /**
         * Set the position within the current turn.
         */
        virtual void positionSet( int p) { pos = p; }

        /**
         * Set the current turn.
         */
        virtual void turnSet( int t ) { turn = t; }

        /**
         * Return the position resolution. This number
         * can be negative or positive and denotes the
         * the maximal or minimal value positionGet().
         */
        virtual int resolution() const { return 4096; }

        /**
         * Returns true if after a positive turn increment,
         * position increments positively.
         * Meaning from 0 to |resolution()| or from
         * resolution() to zero if resolution() < 0
         */
        virtual bool upcounting() const { return true; }
    };
}

#endif
