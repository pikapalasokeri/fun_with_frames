#ifndef _MORPHOLOGY_HPP_
#define _MORPHOLOGY_HPP_

#include "Types.hpp"

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

u8 erode(const MorphologyMask3x3& mask, const u8* row0, const u8* row1, const u8* row2)
{
  // For all pixels in mask overlap:
  // if mask value is 0 then ignore that mask position
  // else the corresponding pixel value must be nonzero.
  return ((mask.m_v00 == 0U || row0[0] != 0U) &&
          (mask.m_v01 == 0U || row0[1] != 0U) &&
          (mask.m_v02 == 0U || row0[2] != 0U) &&
          (mask.m_v10 == 0U || row1[0] != 0U) &&
          (mask.m_v11 == 0U || row1[1] != 0U) &&
          (mask.m_v12 == 0U || row1[2] != 0U) &&
          (mask.m_v20 == 0U || row2[0] != 0U) &&
          (mask.m_v21 == 0U || row2[1] != 0U) &&
          (mask.m_v22 == 0U || row2[2] != 0U)) ? 1U : 0U;
}

#endif
