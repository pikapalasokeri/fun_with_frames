#include "FrameGrabThread.hpp"
#include "Video.hpp"
#include "ImageProcessing.hpp"
extern "C"
{
#include <mjpegtools/jpegutils.h>
#include <mjpegtools/yuv4mpeg.h>
}
#include <QImage>
#include <iostream>

namespace
{
  unsigned char s_frameDataBuffer[(FRAME_SIZE * 3)];
  unsigned char s_rawY[FRAME_SIZE];
  unsigned char s_rawU[UV_FRAME_SIZE];
  unsigned char s_rawV[UV_FRAME_SIZE];
  unsigned char s_rgbBuffer[FRAME_SIZE * 3];
}

FrameGrabThread::FrameGrabThread(CameraContext cameraContext)
  : m_camCtx(cameraContext),
    m_keepGrabbing(true)
{
}

void FrameGrabThread::run()
{
  while (m_keepGrabbing)
  {
    std::cout << "Waiting for frame." << std::endl;
    const int size = waitForFrame(m_camCtx, s_frameDataBuffer);
    std::cout << "Got frame." << std::endl;
    const int itype = Y4M_ILACE_NONE;
    const int ctypeIgnored = 0;
    const int retDecode = decode_jpeg_raw(s_frameDataBuffer,
                                          size,
                                          itype,
                                          ctypeIgnored,
                                          FRAME_WIDTH,
                                          FRAME_HEIGHT,
                                          s_rawY,
                                          s_rawU,
                                          s_rawV); // 420 format.
    convertYuv420ToRgb888(s_rawY, s_rawU, s_rawV, FRAME_WIDTH, FRAME_HEIGHT, s_rgbBuffer);
    QImage image(s_rgbBuffer, FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_RGB888);
    std::cout << "Decoded frame." << std::endl;
    emit signalNewFrame(image);
    std::cout << "Emitted signal." << std::endl;
  }
  std::cout << "Returning from framegrabber" << std::endl;
}

void FrameGrabThread::stopGrabbing()
{
  std::cout << "Stopping grabbing hopefully" << std::endl;
  m_keepGrabbing = false;
}
