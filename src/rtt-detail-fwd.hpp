#ifndef RTT_DETAIL_FWD_HPP
#define RTT_DETAIL_FWD_HPP

namespace RTT {
    /**
     * Base classes used by higher level code.
     */
    namespace base {}
    /**
     * Classes which contain all implementation code for the RTT.
     */
    namespace internal {}
    /**
     * Classes which form the dynamic interface of
     * the TaskContext.
     */
    namespace interface {}
    /**
     * Extra classes strictly not needed to build a control application, but
     * are handy in some cases, or serve as an example on how to extend the RTT.
     */
    namespace extras {}
    /**
     * Classes for reading/writing properties to/from file or data stream.
     */
    namespace marsh {}
    /**
     * OS abstractions.
     */
    namespace os {}
    /**
     * Parser code for the Orocos real-time scripting language.
     */
    namespace scripting {}
    /**
     * Classes for describing and handling user data types.
     */
    namespace types {}
    /**
     * Classes and functions for creating plugins.
     */
    namespace plugin {}
    /**
     * CORBA (OmniORB/TAO) code for network data transport.
     */
    namespace corba {}

    /**
     * Implementation specific classes, not intended for normal users.  
     *
     * The detail namespace is intended for use in the .cpp files of
     * the RTT (or any advanced user of it). The prefered workflow is
     * to include headers from various sub-packages of RTT \b and
     * include this header.  By writing 'using namespace detail;', all
     * the classes from the include headers become then visible in the
     * current namespace.
     */
    namespace detail {
        using namespace base;
        using namespace internal;
        using namespace interface;
        using namespace extras;
        using namespace marsh;
        using namespace os;
        using namespace scripting;
        using namespace types;
        using namespace plugin;
        using namespace corba;
    }

}

#endif
