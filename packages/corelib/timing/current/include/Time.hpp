#ifndef TIME_HPP
#define TIME_HPP

namespace ORO_CoreLib
{
	
typedef double Seconds;
typedef long secs;
typedef long msecs;
typedef long usecs;
typedef long long nsecs;

const long MSECS_IN_SECS = 1000;
const long USECS_IN_SECS = 1000 * MSECS_IN_SECS;
const long NSECS_IN_SECS = 1000 * USECS_IN_SECS;

const long USECS_IN_MSECS = 1000;
const long NSECS_IN_MSECS = 1000 * USECS_IN_MSECS;

const long NSECS_IN_USECS = 1000;

inline msecs secs_to_msecs(const secs s) { return s * MSECS_IN_SECS; }
inline usecs secs_to_usecs(const secs s) { return s * USECS_IN_SECS; }
inline nsecs secs_to_nsecs(const secs s) { return s * NSECS_IN_SECS; }

inline usecs msecs_to_usecs(const msecs ms) { return ms * USECS_IN_MSECS; }
inline nsecs msecs_to_nsecs(const msecs ms) { return ms * NSECS_IN_MSECS; }

inline nsecs usecs_to_nsecs(const usecs us) { return us * NSECS_IN_USECS; }

}

#endif
