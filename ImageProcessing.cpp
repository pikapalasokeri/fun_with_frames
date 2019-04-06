#include "ImageProcessing.hpp"

namespace
{
  int saturatePixel(const int v)
  {
    const int tooSmall = v >> 31; // Fill with sign bit
    const int tooLarge = ~((v > 255) - 1); // take advantage of two's complement
    const int keepValue = ~(tooSmall ^ tooLarge);
    const int ret = (tooLarge & 0x000000FF) | (keepValue & v);
    return ret;
  }

  void yuvToRgb(const unsigned char y,
                const int prer,
                const int preg,
                const int preb,
                unsigned char* outRgb)
  {
    outRgb[0] = static_cast<unsigned char>(saturatePixel(y + prer));
    outRgb[1] = static_cast<unsigned char>(saturatePixel(y + preg));
    outRgb[2] = static_cast<unsigned char>(saturatePixel(y + preb));
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
      const int cb = rawU[uIx] - 128;
      const int cr = rawV[vIx] - 128;
      const int precomputedR = cr + (cr >> 2) + (cr >> 3) + (cr >> 5);
      const int precomputedG = - ((cb >> 2) + (cb >> 4) + (cb >> 5)) - ((cr >> 1) + (cr >> 3) + (cr >> 4) + (cr >> 5));
      const int precomputedB = cb + (cb >> 1) + (cb >> 2) + (cb >> 6);

      yuvToRgb(y1, precomputedR, precomputedG, precomputedB, &outRgbBuffer[3*(rgbIx1)]);
      yuvToRgb(y2, precomputedR, precomputedG, precomputedB, &outRgbBuffer[3*(rgbIx1 + 1)]);
      yuvToRgb(y3, precomputedR, precomputedG, precomputedB, &outRgbBuffer[3*(rgbIx2)]);
      yuvToRgb(y4, precomputedR, precomputedG, precomputedB, &outRgbBuffer[3*(rgbIx2 + 1)]);
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
