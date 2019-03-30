#ifndef _VIDEO_HPP_
#define _VIDEO_HPP_
#include <sys/types.h>


// Change these to your liking...
// or modify the program to take them cmd arguments!
#define DEVICE_NAME "/dev/video0"
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT)
#define UV_FRAME_SIZE (FRAME_SIZE / 4)

// This determines the number of "working" buffers we
// tell the device that it can use. I guess 3 is an OK
// amount? Maybe try less or more if it runs badly.
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
