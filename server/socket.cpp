#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "socket.h"

struct Ctx
{
	int sock;
	sockaddr_in serverAddr,clientAddr;
};
typedef struct Ctx Ctx;

void *socket_open (int port)
{
	Ctx *ctx = new Ctx;
	ctx->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ctx->sock == -1) {
		fprintf(stderr, "%s: create sock err\n", __func__);
		exit(-1);
	}

	ctx->serverAddr.sin_family = AF_INET;
	ctx->serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ctx->serverAddr .sin_port = htons(port);

	int bindRet = bind(ctx->sock ,(struct sockaddr *)&ctx->serverAddr, sizeof(ctx->serverAddr));

	if(bindRet < 0)
	{
		fprintf(stderr, "%s: bind error\n", __func__);
		exit(-1);
	}


	return ctx;
}

void socket_close (void *snd)
{
	Ctx *c = (Ctx*)snd;
	close(c->sock);
	delete c;
}

int socket_send (void *snd, const void *data, int len)
{
	assert(len < 65536);
	Ctx *c = (Ctx*)snd;
	return sendto(c->sock, data, len, 0, (sockaddr*)&c->clientAddr, sizeof(sockaddr_in));
}

int socket_recvfrom(void *snd,void *data)
{
	Ctx *c = (Ctx*)snd;

	socklen_t fromlen = sizeof(c->clientAddr);
	int rc = recvfrom(c->sock, data, 65536, 0, (struct sockaddr*) &c->clientAddr,
			&fromlen);
	return rc;
}


