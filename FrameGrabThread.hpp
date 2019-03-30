#ifndef _FRAME_GRAB_THREAD_HPP_
#define _FRAME_GRAB_THREAD_HPP_

#include <QThread>
#include <QImage>
#include "Video.hpp"

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
};

#endif
