#ifndef _socket__hh
#define _socket__hh

// 仅仅为了演示方便, 使用 udp 传输视频帧, 而且假设每个视频帧都能放到一个 udp 包中
//
//
#ifdef __cplusplus
extern "C" {
#endif // c++

void *socket_open (int target_port);
void socket_close (void *snd);
int socket_send (void *snd, const void *data, int len);
int socket_recvfrom(void *snd,void *data);

#ifdef __cplusplus
}
#endif // c++

#endif // sender.h

