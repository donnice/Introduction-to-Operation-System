#include <unistd.h>
#include "gfserver.h"

/* 
 * Modify this file to implement the interface specified in
 * gfserver.h.
 */
struct gfcontext_t
{
	int hServerSocket;
	int hSocket;
};

struct gfserver_t
{
	char port[12];
	unsigned short max_npending;
	char filePath[256];
	ssize_t(*fp_handler)(gfcontext_t *ctx, char *, void*);
	int *handler_arg;
};

static gfserver_t *gfserv;
static gfcontext_t *gfcontent;

void gfs_abort(gfcontext_t *ctx){
	close(ctx->clntSock);
	close(ctx->sockfd);
	free(gfserv);

	perror("aborting...\n");

	exit(1);
}

gfserver_t* gfserver_create(){
//    return (gfserver_t *)NULL;
	gfserv = (gfserver_t*)malloc(1*sizeof(gfserver_t));
	gfcontext = (gfcontext_t*)malloc(1*sizeof(gfcontext_t));

	return gfserv;
}

ssize_t gfs_send(gfcontext_t *ctx, void *data, size_t len){
    
	size_t total = 0;
	size_t bytesRemain = len;
	size_t n;

	int debug_tries = 1;
	n = send(ctx->clntSock
}

ssize_t gfs_sendheader(gfcontext_t *ctx, gfstatus_t status, size_t file_len){
    char resp_header[MAX_REQUEST_LEN];
	char end_mark[12] = "\\r\\n\\r\\n";

	sprintf(resp_header,"GETFILE%d%z%ds",status,file_len,end_mark);

	printf("Response from header:%s\n",resp_header);

	if(send(ctx->hSocket,resp_header,MAX_REQUEST_LEN,0)!=MAX_REQUEST_LEN)
		printf("Send failed.\n");
	
	return 0;
}

void gfserver_serve(gfserver_t *gfs){
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage clntAddr;
	socklen_t clntSize;
	int yes = 1;
	char str[INET6_ADDRSTRLEN];
	int rv;

	int recvMsg = 0;
	char *req_path;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((rv = getaddrinfo(NULL,gfs->port,&hints, &servinfo))!=0)
	{
		printf("Error in getaddrinfo 83\n");
		return 1;
	}

	for(p= servinfo; p!=NULL;p=p->ai_next)
	{
		if((gfccontext->hServerSocket = socket(p->ai_family,p->ai_socktype, p->ai_protocol)) < 0)
		{
			printf("server:socket\n");
			continue;
		}

		// get rid of 'address already in use'
		if(setsockopt(gfcontext->hServerSocket, SOL_SOCKET, SO_REUSEADDR,&yes, sizeof(int)) < 0)
		{
			printf("setsockopt");
			exit(1);
		}

		if(bind(gfcontext->hServerSocket,p->ai_addr, p->ai_addrlen) < 0)
		{
			close(gfcontext->hServerSocket);
			printf("server:bind\n");
			continue;
		}

		break;
	}

	if(p == NULL)
	{
		printf("Server:failed to bind.\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	if(listen(gfcontext->hServerSocket, gfs->max_npending) == -1)
	{
		printf("listen\n");
		exit(1);
	}

	while(1)
	{
		clntSize = sizeof(clntAddr);
		gfcontext->hSocket = accept(gfcontext->hServerSocket,(struct sockaddr*)&clntAddr, &clntSize);

		if(gfcontext->hSocket == -1)
		{
			
		}
	}
}

void gfserver_set_handlerarg(gfserver_t *gfs, void* arg){
	gfs->handler_arg = (int *)arg;
}

void gfserver_set_handler(gfserver_t *gfs, ssize_t (*handler)(gfcontext_t *, char *, void*)){
	gfs->fp_handler = handler;
}

void gfserver_set_maxpending(gfserver_t *gfs, int max_npending){
	gfs->max_npending = max_npending;
}

void gfserver_set_port(gfserver_t *gfs, unsigned short port){
	gfr->port = port;	
}


