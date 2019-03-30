#include "Video.hpp"
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

#include <iostream>

namespace
{
  void xioctl(int fh, int request, void *arg)
  {
    int r;
    do
    {
      r = v4l2_ioctl(fh, request, arg);
    } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if (r == -1)
    {
      std::cerr << "Cam request failed " << errno << ", " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

CameraContext initStream()
{
  CameraContext context;
  // Open the device
  context.camFd = v4l2_open(DEVICE_NAME, O_RDWR | O_NONBLOCK, 0);
  if (context.camFd < 0)
  {
    std::cerr << "Failed to open device." << std::endl;;
    exit(EXIT_FAILURE);
  }

  // Specify the format of the data we want from the camera
  // Run v4l2-ctl --device=/dev/video1 --list-formats on the
  // device to see that sort of pixel formats are supported!
  v4l2_format fmt = {};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = FRAME_WIDTH;
  fmt.fmt.pix.height = FRAME_HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  xioctl(context.camFd, VIDIOC_S_FMT, &fmt);

  // Set streaming parameters, i.e. frames per second.
  // You'll want to query the device for whether or not it
  // supports setting the frame time, and what possible choices
  // it supports.
  // See http://stackoverflow.com/questions/13981933/v4l2-fcntl-ioctl-vidioc-s-parm-for-setting-fps-and-resolution-of-camera-capture
  v4l2_streamparm parm = {};
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  parm.parm.output.timeperframe.numerator = 1;
  parm.parm.output.timeperframe.denominator = 30;
  xioctl(context.camFd, VIDIOC_S_PARM, &parm);

  // Check what format we _actually_ got
  std::cout << "Opened device with format:" << std::endl
            << "Width: " << fmt.fmt.pix.width << std::endl
            << "Height: " << fmt.fmt.pix.height << std::endl
            << "Pixel format: " << fmt.fmt.pix.pixelformat << std::endl;

  // Request N buffers that are memory mapped between
  // our application space and the device
  v4l2_requestbuffers request = {};
  request.count = MMAP_BUFFERS;
  request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request.memory = V4L2_MEMORY_MMAP;
  xioctl(context.camFd, VIDIOC_REQBUFS, &request);

  context.numBuffers = request.count;
  std::cout << "Got " << context.numBuffers << " buffers" << std::endl;

  // Find out where each requested buffer is located in memory
  // and map them into our application space
  for (int bufferIx = 0; bufferIx < context.numBuffers; ++bufferIx) {
    v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = bufferIx;
    xioctl(context.camFd, VIDIOC_QUERYBUF, &buf);

    context.buffers[bufferIx].length = buf.length;
    context.buffers[bufferIx].start =
      mmap(0,  // start anywhere.
           buf.length,
           PROT_READ | PROT_WRITE, // required.
           MAP_SHARED, // recommended.
           context.camFd, buf.m.offset);

    if (MAP_FAILED == context.buffers[bufferIx].start)
    {
      printf("mmap failed %d, %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  // Queue the buffers, i.e. indicate to the device
  // that they are available for writing now.
  for (int i = 0; i < context.numBuffers; ++i) {
    v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    xioctl(context.camFd, VIDIOC_QBUF, &buf);
  }

  // Start stream
  v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  xioctl(context.camFd, VIDIOC_STREAMON, &type);

  return context;
}

int waitForFrame(const CameraContext& cameraContext, unsigned char* outData)
{
  // The device will now output data continuously.
  // We will use the FD_ZERO/FD_SET/select mechanisms
  // to wait until there is data available from the
  // device. We can specify how long we should wait,
  // and timeout if we think too much time has passed.

  fd_set fds;
  int r = 0;
  do {
    FD_ZERO(&fds);
    FD_SET(cameraContext.camFd, &fds);
    timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    r = select(cameraContext.camFd + 1, &fds, NULL, NULL, &tv);
  } while ((r == -1 && (errno = EINTR)));

  if (r == -1) {
    std::cerr << "Failed while waiting for pixels" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Data has arrived! Let's "dequeue" a buffer to get its data
  v4l2_buffer buf = {};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  xioctl(cameraContext.camFd, VIDIOC_DQBUF, &buf);

  const int bytesUsed = buf.bytesused;
  memcpy(outData, cameraContext.buffers[buf.index].start, bytesUsed);

  // Queue buffer for writing again
  xioctl(cameraContext.camFd, VIDIOC_QBUF, &buf);
  return bytesUsed;
}

void deinitCamera(const CameraContext& cameraContext)
{
  // Turn off stream
  v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  xioctl(cameraContext.camFd, VIDIOC_STREAMOFF, &type);

  // Unmap buffers
  for (int i = 0; i < cameraContext.numBuffers; ++i)
    munmap(cameraContext.buffers[i].start, cameraContext.buffers[i].length);

  // Tell the device that it can release memory
  v4l2_requestbuffers request = {};
  request.count = 0;
  request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request.memory = V4L2_MEMORY_MMAP;
  xioctl(cameraContext.camFd, VIDIOC_REQBUFS, &request);

  v4l2_close(cameraContext.camFd);
}
