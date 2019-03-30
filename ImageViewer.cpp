#include "ImageViewer.hpp"
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <mutex>
#include <iostream>

ImageViewer::ImageViewer(const int width, const int height)
  : m_imageLabel(new QLabel)
{
  m_imageLabel->setBackgroundRole(QPalette::Base);
  m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  m_imageLabel->setScaledContents(true);
  resize(width, height);
  setCentralWidget(m_imageLabel);
}

void ImageViewer::newFrame(QImage image)
{
  std::cout << "Received signal" << std::endl;
  m_image = image;
  m_imageLabel->setPixmap(QPixmap::fromImage(m_image));

  std::cout << "New pixmap was set" << std::endl;
}
