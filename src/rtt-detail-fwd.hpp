#ifndef RTT_DETAIL_FWD_HPP
#define RTT_DETAIL_FWD_HPP

namespace RTT {
    namespace base {}
    namespace internal {}
    namespace interface {}
    namespace extras {}
    namespace marsh {}
    namespace os {}
    namespace scripting {}
    namespace types {}
    namespace plugin {}
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
