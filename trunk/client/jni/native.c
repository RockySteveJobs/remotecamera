#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <android/log.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#define  LOG_TAG    "remoteCamera"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define RECV_PORT 3020
struct sockaddr_in serveraddr, cliAddr;
int sock;

int close_video_flag = 0;

AVCodec *codec;
AVCodecContext *dec;

//存放解码后1帧YUV数据
AVFrame *pFrame;

int iWidth = 640;
int iHeight = 480;

int *colortab = NULL;
int *u_b_tab = NULL;
int *u_g_tab = NULL;
int *v_g_tab = NULL;
int *v_r_tab = NULL;

//short *tmp_pic=NULL;

unsigned int *rgb_2_pix = NULL;
unsigned int *r_2_pix = NULL;
unsigned int *g_2_pix = NULL;
unsigned int *b_2_pix = NULL;

void DeleteYUVTab() {
//	av_free(tmp_pic);

	av_free(colortab);
	av_free(rgb_2_pix);
}

void CreateYUVTab_16() {
	int i;
	int u, v;

//	tmp_pic = (short*)av_malloc(iWidth*iHeight*2); // 缓存 iWidth * iHeight * 16bits

	colortab = (int *) av_malloc(4 * 256 * sizeof(int));
	u_b_tab = &colortab[0 * 256];
	u_g_tab = &colortab[1 * 256];
	v_g_tab = &colortab[2 * 256];
	v_r_tab = &colortab[3 * 256];

	for (i = 0; i < 256; i++) {
		u = v = (i - 128);

		u_b_tab[i] = (int) (1.772 * u);
		u_g_tab[i] = (int) (0.34414 * u);
		v_g_tab[i] = (int) (0.71414 * v);
		v_r_tab[i] = (int) (1.402 * v);
	}

	rgb_2_pix = (unsigned int *) av_malloc(3 * 768 * sizeof(unsigned int));

	r_2_pix = &rgb_2_pix[0 * 768];
	g_2_pix = &rgb_2_pix[1 * 768];
	b_2_pix = &rgb_2_pix[2 * 768];

	for (i = 0; i < 256; i++) {
		r_2_pix[i] = 0;
		g_2_pix[i] = 0;
		b_2_pix[i] = 0;
	}

	for (i = 0; i < 256; i++) {
		r_2_pix[i + 256] = (i & 0xF8) << 8;
		g_2_pix[i + 256] = (i & 0xFC) << 3;
		b_2_pix[i + 256] = (i) >> 3;
	}

	for (i = 0; i < 256; i++) {
		r_2_pix[i + 512] = 0xF8 << 8;
		g_2_pix[i + 512] = 0xFC << 3;
		b_2_pix[i + 512] = 0x1F;
	}

	r_2_pix += 256;
	g_2_pix += 256;
	b_2_pix += 256;
}

void DisplayYUV_16(unsigned int *pdst1, unsigned char *y, unsigned char *u,
		unsigned char *v, int width, int height, int src_ystride,
		int src_uvstride, int dst_ystride) {
	int i, j;
	int r, g, b, rgb;

	int yy, ub, ug, vg, vr;

	unsigned char* yoff;
	unsigned char* uoff;
	unsigned char* voff;

	unsigned int* pdst = pdst1;

	int width2 = width / 2;
	int height2 = height / 2;

	if (width2 > iWidth / 2) {
		width2 = iWidth / 2;

		y += (width - iWidth) / 4 * 2;
		u += (width - iWidth) / 4;
		v += (width - iWidth) / 4;
	}

	if (height2 > iHeight)
		height2 = iHeight;

	for (j = 0; j < height2; j++) // 一次2x2共四个像素
			{
		yoff = y + j * 2 * src_ystride;
		uoff = u + j * src_uvstride;
		voff = v + j * src_uvstride;

		for (i = 0; i < width2; i++) {
			yy = *(yoff + (i << 1));
			ub = u_b_tab[*(uoff + i)];
			ug = u_g_tab[*(uoff + i)];
			vg = v_g_tab[*(voff + i)];
			vr = v_r_tab[*(voff + i)];

			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			rgb = r_2_pix[r] + g_2_pix[g] + b_2_pix[b];

			yy = *(yoff + (i << 1) + 1);
			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			pdst[(j * dst_ystride + i)] = (rgb)
					+ ((r_2_pix[r] + g_2_pix[g] + b_2_pix[b]) << 16);

			yy = *(yoff + (i << 1) + src_ystride);
			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			rgb = r_2_pix[r] + g_2_pix[g] + b_2_pix[b];

			yy = *(yoff + (i << 1) + src_ystride + 1);
			b = yy + ub;
			g = yy - ug - vg;
			r = yy + vr;

			pdst[((2 * j + 1) * dst_ystride + i * 2) >> 1] = (rgb)
					+ ((r_2_pix[r] + g_2_pix[g] + b_2_pix[b]) << 16);
		}
	}
}

char *intToString(int value, char *string, int radix)
{
    int rt=0;
    if(string==NULL)
        return NULL;
    if(radix<=0 || radix>30)
        return NULL;
    rt = snprintf(string, radix, "%d", value);
    if(rt>radix)
        return NULL;
    string[rt]='\0';
    return string;
}

void Java_com_linuxlabs_remotecamera_VideoView_connectTo(JNIEnv * env,
		jobject this, jstring ip) {

	char *ip2 = (char *) (*env)->GetStringUTFChars(env, ip, NULL);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(RECV_PORT);
	serveraddr.sin_addr.s_addr = inet_addr(ip2);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		LOGE("create socket error");
	}

	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliAddr.sin_port = htons(0);
}

void Java_com_linuxlabs_remotecamera_VideoView_sendCommand(JNIEnv * env,
		jobject this, jint cmd) {

	char c_cmd[10];
	intToString(cmd,c_cmd,10);

	int sendRet = sendto(sock, c_cmd, strlen(c_cmd), 0,
			(struct sockaddr*) &serveraddr, sizeof(struct sockaddr_in));
}

void Java_com_linuxlabs_remotecamera_VideoView_initDecoder() {
	avcodec_register_all();
	codec = avcodec_find_decoder(CODEC_ID_H264);
	dec = avcodec_alloc_context();
	pFrame = avcodec_alloc_frame();
	if (avcodec_open(dec, codec) < 0) {
		LOGE("ppen decoder error");
	}
}

void Java_com_linuxlabs_remotecamera_VideoView_displayVideo(JNIEnv * env,
		jobject this, jbyteArray out) {
	unsigned char *buf = (unsigned char*) alloca(65536);
	CreateYUVTab_16();

	while (!close_video_flag) {
		socklen_t fromlen = sizeof(cliAddr);
		int rc = recvfrom(sock, buf, 65536, 0, (struct sockaddr*) &cliAddr,
				&fromlen);
		if (rc > 0) {
			// 解压
			int got;
			AVPacket pkt;
			pkt.data = buf;
			pkt.size = rc;
			int ret = avcodec_decode_video2(dec, pFrame, &got, &pkt);

			if (ret > 0 && got) {
				// 解码成功
				jbyte * Pixel = (jbyte*) (*env)->GetByteArrayElements(env, out,
						0);
				DisplayYUV_16((int*) Pixel, pFrame->data[0], pFrame->data[1],
						pFrame->data[2], dec->width, dec->height,
						pFrame->linesize[0], pFrame->linesize[1], iWidth);
				(*env)->ReleaseByteArrayElements(env, out, Pixel, 0);
			}
		}
	}

}

void Java_com_linuxlabs_remotecamera_VideoView_closeSocket(JNIEnv * env,
		jobject this) {

	//停止接收数据标志
	close_video_flag = 1;

	avcodec_close(dec);
	av_free(dec);
	av_free(pFrame);
	close(sock);
}

