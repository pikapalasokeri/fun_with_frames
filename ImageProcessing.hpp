#ifndef _IMAGE_PROCESSING_HPP_
#define _IMAGE_PROCESSING_HPP_

void convertYuv420ToRgb888(const unsigned char* rawY,
                           const unsigned char* rawU,
                           const unsigned char* rawV,
                           const int width,
                           const int height,
                           unsigned char* outRgbBuffer);

void movementAddFrame(unsigned char* frameGray);
void movementErodeMask();
void movementVisualizeMask(unsigned char* outRgbBuffer);

#endif
