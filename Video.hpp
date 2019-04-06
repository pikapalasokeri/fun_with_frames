#ifndef _VIDEO_HPP_
#define _VIDEO_HPP_
#include "FrameSize.hpp"
#include <sys/types.h>

#define DEVICE_NAME "/dev/video0"
#define UV_FRAME_SIZE (FRAME_SIZE / 4)
#define MMAP_BUFFERS 3

struct Buffer
{
  void *start;
  size_t length;
};

struct CameraContext
{
  int camFd;
  int numBuffers;
  Buffer buffers[MMAP_BUFFERS];
};

CameraContext initStream();
int waitForFrame(const CameraContext& cameraContext, unsigned char* outData);
void deinitCamera(const CameraContext& cameraContext);

#endif
