
// Some info about this hack :
// For LXRT, we should include <rtai_comedi.h>
// but this file is horribly broken, thus
// we use the signatures of the linux/comedilib.h
// functions but _link_ against libkcomedilib_lxrt.so

#include <pkgconf/system.h>
#ifdef OROPKG_OS_LXRT
    #define __KERNEL__
    extern "C"
    {
#include <linux/comedilib.h>
    }
    #undef __KERNEL__
#else
// userspace gnu/linux:
    #include <comedilib.h>
#endif

