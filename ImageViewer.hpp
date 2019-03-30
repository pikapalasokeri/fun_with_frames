#ifndef _IMAGE_VIEWER_HPP_
#define _IMAGE_VIEWER_HPP_

#include <QMainWindow>
#include <QLabel>

class ImageViewer : public QMainWindow
{
  Q_OBJECT;
public slots:
  void newFrame(QImage image);
public:
  ImageViewer(const int width, const int height);
private:
  QImage m_image;
  QLabel* m_imageLabel;
};

#endif
