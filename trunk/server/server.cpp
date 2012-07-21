#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include "capture.h"
#include "vcompress.h"
#include "socket.h"

/** webcam_server: 打开 /dev/video0, 获取图像, 压缩, 发送到 localhost:3020 端口
 *
 * 	使用 320x240, fps=10
 */
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FPS 10.0

#define OPEN_VIDEO "1"
#define PLAY_VIDEO "2"
#define STOP_VIDEO "2"
#define CLOSE_VIDEO "3"

#define TARGET_PORT 3020

int main (int argc, char **argv)
{

        pthread_t play_video, stop_video;
	void *capture = capture_open("/dev/video0", VIDEO_WIDTH, VIDEO_HEIGHT, PIX_FMT_YUV420P);
	if (!capture) {
		fprintf(stderr, "ERR: can't open '/dev/video0'\n");
		exit(-1);
	}

	void *encoder = vc_open(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_FPS);
	if (!encoder) {
		fprintf(stderr, "ERR: can't open x264 encoder\n");
		exit(-1);
	}

	void *socket = socket_open(TARGET_PORT);
	if (!socket) {
		fprintf(stderr, "ERR: can't open sender for:%d\n",TARGET_PORT);
		exit(-1);
	}

	int tosleep = 100000 / VIDEO_FPS;

	char *cmd = (char*) alloca(100);

	do
	{
	          int len = socket_recvfrom(socket,cmd);
	                  if(strcmp(cmd,PLAY_VIDEO) == 0)
	                  {
	                          for (; ; ) {
	                                  // 抓
	                                  Picture pic;
	                                  capture_get_picture(capture, &pic);

	                                  // 压
	                                  const void *outdata;
	                                  int outlen;
	                                  int rc = vc_compress(encoder, pic.data, pic.stride, &outdata, &outlen);
	                                  if (rc < 0) continue;

	                                  // 发
	                                  socket_send(socket, outdata, outlen);
	                                  printf("sending over\n");

	                                  // 等
	                                  usleep(tosleep);
	                          }
	                  }

	        }

	while(strcmp(cmd,STOP_VIDEO) != 0);

	socket_close(socket);
	vc_close(encoder);
	capture_close(capture);

	return 0;
}

