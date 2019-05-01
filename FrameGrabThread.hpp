#ifndef _FRAME_GRAB_THREAD_HPP_
#define _FRAME_GRAB_THREAD_HPP_

#include <QThread>
#include <QImage>
#include "Video.hpp"
#include "Timer.hpp"

class FrameGrabThread : public QThread
{
  Q_OBJECT;
signals:
  void signalNewFrame(QImage);
public:
  FrameGrabThread(CameraContext cameraContext);
  void run();
  void stopGrabbing();
private:
  CameraContext m_camCtx;
  bool m_keepGrabbing;
  Timer m_timerJpegDecode;
  Timer m_timerYuvRgb;
  Timer m_timerMovement;
  Timer m_timerErode;
};

#endif
