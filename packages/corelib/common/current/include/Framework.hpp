
/**
 * These are some example functions which you can use
 * to setup and run a realtime system. It creates some
 * threads with some common sense default values.
 * The ZeroTimeThread, ZeroLatencyThread and CompletionProcessor
 * are created, initialized and started.
 *
 * @note These functions are purely for demonstartion purposes
 *       You are encouraged to write your own setup functions
 *       tuned for your own needs.
 */
#ifdef __cplusplus
extern "C" 
{
#endif
    /**
     * Creates and initializes the ZeroTimeThread, ZeroLatencyThread 
     * and CompletionProcessor.
     */
    void initFramework(void);

    /**
     * Starts the ZeroTimeThread, ZeroLatencyThread and CompletionProcessor.
     */
    void startFramework(void);

    /**
     * Stops the ZeroTimeThread, ZeroLatencyThread and CompletionProcessor.
     */
    void stopFramework(void);

    /**
     * Destroys the ZeroTimeThread, ZeroLatencyThread and CompletionProcessor.
     */
    void cleanupFramework(void);
#ifdef __cplusplus
}
#endif
