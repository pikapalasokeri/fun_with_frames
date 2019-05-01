#include "FrameGrabThread.hpp"
#include "Video.hpp"
#include "ImageProcessing.hpp"
#include "FrameSize.hpp"
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
    m_keepGrabbing(true),
    m_timerJpegDecode("Jpeg decode"),
    m_timerYuvRgb("Yuv to rgb conversion"),
    m_timerMovement("Movement"),
    m_timerErode("Erode")
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
    m_timerJpegDecode.start();
    const int retDecode = decode_jpeg_raw(s_frameDataBuffer,
                                          size,
                                          itype,
                                          ctypeIgnored,
                                          FRAME_WIDTH,
                                          FRAME_HEIGHT,
                                          s_rawY,
                                          s_rawU,
                                          s_rawV); // 420 format.
    m_timerJpegDecode.stop();

    m_timerYuvRgb.start();
    convertYuv420ToRgb888(s_rawY, s_rawU, s_rawV, FRAME_WIDTH, FRAME_HEIGHT, s_rgbBuffer);
    m_timerYuvRgb.stop();

    m_timerMovement.start();
    movementAddFrame(s_rawY);
    m_timerErode.start();
    movementErodeMask();
    m_timerErode.stop();
    movementVisualizeMask(s_rgbBuffer);
    m_timerMovement.stop();

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
