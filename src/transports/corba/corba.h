#ifndef ORO_CORBA_SUPPORT_HH
#define ORO_CORBA_SUPPORT_HH

#include "rtt-corba-config.h"
#if CORBA_IS_TAO
#include <ace/SString.h>
#include <tao/corba.h>
#include <tao/PortableServer/PS_ForwardC.h>
#define CORBA_SERVANT(f) f ## "S.h"
#define CORBA_EXCEPTION_INFO(x) x._info().c_str()
#else
#include <omniORB4/CORBA.h>
#include <omniORB4/POA.h>
namespace corba {
    typedef Any* Any_ptr;
}
#define ACE_THROW_SPEC(x) throw x
#define CORBA_EXCEPTION_INFO(x) x._name()
#endif

#endif
