#ifndef BITS_H
#define BITS_H
#include <stdint.h>

#if defined(USE_UINT32_T)
typedef uint32_t packed_t;
#elif defined(USE_UINT16_T)
typedef uint16_t packed_t;
#elif defined(USE_UINT8_T)
typedef uint8_t packed_t;
#else
typedef uint64_t packed_t;
#endif

#define EMPTY_MASK ((packed_t) 0)
#define MASK(base_enum) 1 << (base_enum)

#define BITS (size_t) 8
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define PACK_MASK(dst, idx) (dst) |= (1 << idx)
#define PACK_CMP(dst, a, b, offset) \
    (dst) |= ((uint8_t) ((a) ^ (b)) != 0) << offset


#endif
