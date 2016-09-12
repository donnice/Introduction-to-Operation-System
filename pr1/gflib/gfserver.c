#include <unistd.h>
#include "gfserver.h"

/* 
 * Modify this file to implement the interface specified in
 * gfserver.h.
 */
struct gfcontext_t
{
	int sockfd;
	int clntSock;
};

struct gfserver_t
{
	char port[12];
	unsigned short nHostPort;
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
    return -1;
}

ssize_t gfs_sendheader(gfcontext_t *ctx, gfstatus_t status, size_t file_len){
    return -1;
}

void gfserver_serve(gfserver_t *gfs){

}

void gfserver_set_handlerarg(gfserver_t *gfs, void* arg){

}

void gfserver_set_handler(gfserver_t *gfs, ssize_t (*handler)(gfcontext_t *, char *, void*)){

}

void gfserver_set_maxpending(gfserver_t *gfs, int max_npending){

}

void gfserver_set_port(gfserver_t *gfs, unsigned short port){

}


