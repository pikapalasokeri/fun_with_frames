#include <stdlib.h>
#include <sys/types.h>

#include <thread>
#include <iostream>
#include <QApplication>
#include "ImageViewer.hpp"
#include "Video.hpp"
#include "FrameGrabThread.hpp"

unsigned char s_intermediateBuffer[(FRAME_SIZE * 3)];
bool s_keepGrabbing = true;
CameraContext s_camCtx;

int main(int argc, char **argv)
{
  s_camCtx = initStream();

  QApplication app(argc, argv);
  ImageViewer imageViewer(FRAME_WIDTH * 2, FRAME_HEIGHT * 2);
  imageViewer.show();

  // Capture all the frames!
  FrameGrabThread frameGrabThread(s_camCtx);
  QObject::connect(&frameGrabThread, SIGNAL(signalNewFrame(QImage)),
                   &imageViewer, SLOT(newFrame(QImage)),
                   Qt::QueuedConnection);
  frameGrabThread.start();
  const int ret = app.exec();
  frameGrabThread.stopGrabbing();
  frameGrabThread.wait();
  deinitCamera(s_camCtx);
  return ret;
}
