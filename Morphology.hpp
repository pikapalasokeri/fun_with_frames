#ifndef _MORPHOLOGY_HPP_
#define _MORPHOLOGY_HPP_

#include "Types.hpp"
#include "Simd.hpp"

class MorphologyMask3x3
{
public:
  MorphologyMask3x3(const u8 v)
    : m_v00(v),
      m_v01(v),
      m_v02(v),
      m_v10(v),
      m_v11(v),
      m_v12(v),
      m_v20(v),
      m_v21(v),
      m_v22(v)
    {}

  MorphologyMask3x3(const u8 v00, const u8 v01, const u8 v02,
                    const u8 v10, const u8 v11, const u8 v12,
                    const u8 v20, const u8 v21, const u8 v22)
    : m_v00(v00),
      m_v01(v01),
      m_v02(v02),
      m_v10(v10),
      m_v11(v11),
      m_v12(v12),
      m_v20(v20),
      m_v21(v21),
      m_v22(v22)
  {}

  const u8 m_v00;
  const u8 m_v01;
  const u8 m_v02;
  const u8 m_v10;
  const u8 m_v11;
  const u8 m_v12;
  const u8 m_v20;
  const u8 m_v21;
  const u8 m_v22;
};

void erode(const MorphologyMask3x3& mask, const u8* src, u8* dst)
{
  const bool scalarUse00 = mask.m_v00 != 0U;
  const bool scalarUse01 = mask.m_v01 != 0U;
  const bool scalarUse02 = mask.m_v02 != 0U;
  const bool scalarUse10 = mask.m_v10 != 0U;
  const bool scalarUse11 = mask.m_v11 != 0U;
  const bool scalarUse12 = mask.m_v12 != 0U;
  const bool scalarUse20 = mask.m_v20 != 0U;
  const bool scalarUse21 = mask.m_v21 != 0U;
  const bool scalarUse22 = mask.m_v22 != 0U;
  const u8x16 zero = setU8x16(0U);
  const u8x16 use00 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v00), zero));
  const u8x16 use01 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v01), zero));
  const u8x16 use02 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v02), zero));
  const u8x16 use10 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v10), zero));
  const u8x16 use11 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v11), zero));
  const u8x16 use12 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v12), zero));
  const u8x16 use20 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v20), zero));
  const u8x16 use21 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v21), zero));
  const u8x16 use22 = notU8x16(cmpEqU8x16(setU8x16(mask.m_v22), zero));

  for (int i = 0; i < FRAME_HEIGHT - 2; ++i)
  {
    const u8* row0 = &src[i * FRAME_WIDTH];
    const u8* row1 = &src[(i + 1) * FRAME_WIDTH];
    const u8* row2 = &src[(i + 2) * FRAME_WIDTH];
    u8* dstRow = &dst[(i+1) * FRAME_WIDTH];
    int j = 0;
    for (; j <= FRAME_WIDTH - 16 - 2; j += 16)
    {
      const u8x16 hit00 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row0[j + 0]), zero), use00);
      const u8x16 hit01 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row0[j + 1]), zero), use01);
      const u8x16 hit02 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row0[j + 2]), zero), use02);
      const u8x16 hit10 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row1[j + 0]), zero), use10);
      const u8x16 hit11 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row1[j + 1]), zero), use11);
      const u8x16 hit12 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row1[j + 2]), zero), use12);
      const u8x16 hit20 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row2[j + 0]), zero), use20);
      const u8x16 hit21 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row2[j + 1]), zero), use21);
      const u8x16 hit22 = andNotU8x16(cmpEqU8x16(uloadU8x16(&row2[j + 2]), zero), use22);
      const u8x16 res1 = andU8x16(hit00, hit01);
      const u8x16 res2 = andU8x16(hit02, hit10);
      const u8x16 res3 = andU8x16(hit11, hit12);
      const u8x16 res4 = andU8x16(hit20, hit21);
      const u8x16 resRes1 = andU8x16(res1, res2);
      const u8x16 resRes2 = andU8x16(res3, res4);
      const u8x16 resResRes1 = andU8x16(resRes1, resRes2);
      const u8x16 final = andU8x16(resResRes1, hit22);
      ustoreU8x16(final, &dstRow[j + 1]);
    }
    for (; j < FRAME_WIDTH - 2; ++j)
    {
      const bool hit00 = (row0[j + 0] != 0U) && scalarUse00;
      const bool hit01 = (row0[j + 1] != 0U) && scalarUse01;
      const bool hit02 = (row0[j + 2] != 0U) && scalarUse02;
      const bool hit10 = (row1[j + 0] != 0U) && scalarUse10;
      const bool hit11 = (row1[j + 1] != 0U) && scalarUse11;
      const bool hit12 = (row1[j + 2] != 0U) && scalarUse12;
      const bool hit20 = (row2[j + 0] != 0U) && scalarUse20;
      const bool hit21 = (row2[j + 1] != 0U) && scalarUse21;
      const bool hit22 = (row2[j + 2] != 0U) && scalarUse22;
      dstRow[j + 1] =
        hit00 && hit01 && hit02 &&
        hit10 && hit11 && hit12 &&
        hit20 && hit21 && hit22;
    }
  }
}

#endif
