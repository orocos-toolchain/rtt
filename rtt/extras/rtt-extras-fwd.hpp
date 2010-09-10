#ifndef ORO_RTT_extras_FWD_HPP
#define ORO_RTT_extras_FWD_HPP

namespace RTT {
    namespace extras {
        class ConfigurationInterface;
        class Configurator;
        class FileDescriptorActivity;
        class IRQActivity;
        class PeriodicActivity;
        class SequentialActivity;
        class SimulationActivity;
        class SimulationThread;
        class SlaveActivity;
        class TimerThread;
        struct Provider;
        struct RT_INTR;
        template <unsigned S, class T>
        struct MultiVector;
        template<typename T>
        class ReadOnlyPointer;
        template<typename T>
        struct ROPtrInternal;
    }
    namespace detail {
        using namespace extras;
    }
}
#endif
