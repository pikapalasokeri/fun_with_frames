#ifndef _SIMD_HPP_
#define _SIMD_HPP_

#include "Macros.hpp"
#include "Types.hpp"
#include <immintrin.h>
#include <emmintrin.h>

typedef __m128i u8x16;

INLINE u8x16 setU8x16(const u8 value)
{
  return _mm_set1_epi8(value);
}

INLINE u8x16 cmpEqU8x16(const u8x16 v1, const u8x16 v2)
{
  return _mm_cmpeq_epi8(v1, v2);
}

INLINE u8x16 uloadU8x16(const u8* ptr)
{
  return _mm_loadu_si128(reinterpret_cast<const u8x16*>(ptr));
}

INLINE void ustoreU8x16(const u8x16 v, u8* dst)
{
  _mm_storeu_si128(reinterpret_cast<u8x16*>(dst), v);
}

INLINE u8x16 andNotU8x16(const u8x16 v1, const u8x16 v2)
{
  // (not v1) and v2
  return _mm_andnot_si128(v1, v2);
}

INLINE u8x16 andU8x16(const u8x16 v1, const u8x16 v2)
{
  return _mm_and_si128(v1, v2);
}

INLINE u8x16 notU8x16(const u8x16 v)
{
  // I'm not so sure about signed vs unsigned for 0xFF here...
  return _mm_andnot_si128(v, _mm_set1_epi8(0xFF));
}

#endif
