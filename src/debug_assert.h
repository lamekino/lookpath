#ifndef ASSERT_H
#define ASSERT_H
#include <assert.h>

#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]

#ifdef DEBUG
#define ASSERT(p) assert((p))
#else
#define ASSERT(_)
#endif

#endif
