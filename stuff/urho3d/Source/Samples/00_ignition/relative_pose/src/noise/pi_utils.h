
#ifndef __PI_UTILS_H_
#define __PI_UTILS_H_

#include "pi_random.h"
#include "pi_fixed.h"



static inline Sint64 isqrt(Sint64 a)
{
	// replace with cast from sqrt below which is between x7.3 (win32, Debug) & x15 (x64, Release) times faster
	return static_cast<int64_t>(sqrt(static_cast<double>(a)));
}

static inline Sint64 isqrt(fixed v)
{
	Sint64 ret = 0;
	Sint64 s;
	Sint64 ret_sq = -v.v - 1;
	for (s = 62; s >= 0; s -= 2) {
		Sint64 b;
		ret += ret;
		b = ret_sq + ((2 * ret + 1) << s);
		if (b < 0) {
			ret_sq = b;
			ret++;
		}
	}
	return ret;
}





#endif














