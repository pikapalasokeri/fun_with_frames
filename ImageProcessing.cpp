#include "ImageProcessing.hpp"

namespace
{
  template<typename T>
  const T& clamp(const T& v, const T& low, const T& high)
  {
    return (v > high ? high : (v < low ? low : v));
  }

  void yuvToRgb(const unsigned char y,
                const unsigned char u,
                const unsigned char v,
                unsigned char* outRgb)
  {
    const int cy = y;
    const int cb = u - 128;
    const int cr = v - 128;
    outRgb[0] = static_cast<unsigned char>(clamp(cy + cr + (cr >> 2) + (cr >> 3) + (cr >> 5), 0, 255));
    outRgb[1] = static_cast<unsigned char>(clamp(cy - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5)), 0, 255));
    outRgb[2] = static_cast<unsigned char>(clamp(cy + cb + (cb >> 1) + (cb >> 2) + (cb >> 6), 0, 255));
  }
}

void convertYuv420ToRgb888(const unsigned char* rawY,
                           const unsigned char* rawU,
                           const unsigned char* rawV,
                           const int width,
                           const int height,
                           unsigned char* outRgbBuffer)
{
  const int stride = width;
  int yIx1 = 0;
  int yIx2 = stride;
  int uIx = 0;
  int vIx = 0;
  int rgbIx1 = 0;
  int rgbIx2 = stride;
  for (int i = 0; i < height / 2; ++i)
  {
    for (int j = 0; j < width / 2; ++j)
    {
      const unsigned char y1 = rawY[yIx1];
      const unsigned char y2 = rawY[yIx1 + 1];
      const unsigned char y3 = rawY[yIx2];
      const unsigned char y4 = rawY[yIx2 + 1];
      const unsigned char u = rawU[uIx];
      const unsigned char v = rawV[vIx];
      yuvToRgb(y1, u, v, &outRgbBuffer[3*(rgbIx1)]);
      yuvToRgb(y2, u, v, &outRgbBuffer[3*(rgbIx1 + 1)]);
      yuvToRgb(y3, u, v, &outRgbBuffer[3*(rgbIx2)]);
      yuvToRgb(y4, u, v, &outRgbBuffer[3*(rgbIx2 + 1)]);
      yIx1 += 2;
      yIx2 += 2;
      ++uIx;
      ++vIx;
      rgbIx1 += 2;
      rgbIx2 += 2;
    }
    rgbIx1 += stride;
    rgbIx2 += stride;
    yIx1 += stride;
    yIx2 += stride;
  }
}
