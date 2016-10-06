#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "gfserver.h"

/*
 * Modify this file to implement the interface specified in
 * gfserver.h.
 */

#define BUFSIZE 4096


struct gfcontext_t {
    //listen socket file discriptor
    int socklsn;
    //connection socket file discriptor
    int sockconn;
    //file path
    char* filepath;
}; 

struct gfserver_t {
	//socket connect arguments

	//port number
    unsigned short port;
    //max number of pending methods
    int max_npending;

    //arguments
    gfcontext_t *ctx;
    char *path;
    void *handlearg;

    //callback function
    ssize_t (*handler)(gfcontext_t *ctx, char *path, void *handlearg);
};


void error(char* mge)
{
	fprintf(stderr, "%s\n", mge);
	exit(-1);
}

void print(char* mge) {
    fprintf(stdout, "%s.\n", mge);
    fflush(stdout); 
}

ssize_t gfs_sendheader(gfcontext_t *ctx, gfstatus_t status, size_t file_len){
    
    print("1. sending response header");
    
    if (status == GF_FILE_NOT_FOUND) {
    	print("GETFILE FILE_NOT_FOUND \r\n\r\n");
        int size = write(ctx->sockconn, "GETFILE FILE_NOT_FOUND \r\n\r\n", strlen("GETFILE FILE_NOT_FOUND \r\n\r\n") + 1);
        gfs_abort(ctx);
        return size;
    }

    if (status == GF_ERROR) {
    	print("GETFILE ERROR \r\n\r\n");
        int size = write(ctx->sockconn, "GETFILE ERROR \r\n\r\n", strlen("GETFILE ERROR \r\n\r\n") + 1);
        gfs_abort(ctx);
        return size;
    }
    
    char header[256];
    bzero(header, 256);
    sprintf(header, "GETFILE OK %zu \r\n\r\n", file_len);
    print(header);
    
    return write(ctx->sockconn, header, strlen(header));
}

ssize_t gfs_send(gfcontext_t *ctx, void *data, size_t len){

    return write(ctx->sockconn, data, len);;
}

void gfs_abort(gfcontext_t *ctx){
    close(ctx->sockconn);
}

gfserver_t* gfserver_create(){
    struct gfserver_t *gfs = (struct gfserver_t*)malloc(sizeof(struct gfserver_t));
    gfs->ctx = NULL;
    gfs->path = NULL;
    gfs->handlearg = NULL;
    return gfs;
}

void gfserver_set_port(gfserver_t *gfs, unsigned short port){
    gfs->port = port;
}
void gfserver_set_maxpending(gfserver_t *gfs, int max_npending){
    gfs->max_npending = max_npending;
}

void gfserver_set_handler(gfserver_t *gfs, ssize_t (*handler)(gfcontext_t *, char *, void*)){
    gfs->handler = handler;
}

void gfserver_set_handlerarg(gfserver_t *gfs, void* arg){
    gfs->handlearg = arg;
}


void sockInitServer(gfserver_t *gfs, int* socklsn, struct sockaddr_in* serveraddr)
{
    /* socket: create the socket */
   
    *socklsn = socket(AF_INET, SOCK_STREAM, 0);
    if (*socklsn < 0) {
		error("ERROR opening socket\n");        
    }
	

  
  	int option_char = 1;
  	setsockopt(*socklsn, SOL_SOCKET, SO_REUSEADDR, 
       (const void *)&option_char, sizeof(int));

	/* build the server's Internet address */
    bzero((char *) serveraddr, sizeof(struct sockaddr_in));

	serveraddr->sin_family = AF_INET;
	serveraddr->sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr->sin_port = htons(gfs->port);

	if(bind(*socklsn, (struct sockaddr *) serveraddr, sizeof(*serveraddr))<0){
		error("ERROR on binding");
	}

	print("socket bind completed");
	if(listen(*socklsn, gfs->max_npending) < 0){
		error("ERROR on listen");
	}
	print("socket listen completed");    
}

void gfserver_serve(gfserver_t *gfs){
    
    int socklsn = 0;
    int sockconn = 0;

    struct sockaddr_in serveraddr;
    char readData[BUFSIZE];
    bzero(readData, BUFSIZE);


    //infinte loop: waiting 
    sockInitServer(gfs, &socklsn, &serveraddr);
    while(1) { 

	   	/* accept: wait for a connection method */
		sockconn = accept(socklsn, (struct sockaddr *) NULL, NULL);
		if (sockconn < 0) {
	      error("ERROR on accept");
		}
		print("socket connected successed");
		

        recv(sockconn, readData, BUFSIZE, 0);
        
        
        char *scheme = strtok(readData, " ");
        char *method = strtok(NULL, " ");
        char *filenamerecvcli = strtok(NULL, " ");
        
        struct gfcontext_t *ctx = (struct gfcontext_t*)malloc(sizeof(struct gfcontext_t));
        ctx->filepath = filenamerecvcli;
        ctx->socklsn = socklsn;
        ctx->sockconn = sockconn;
        
        char filename = *filenamerecvcli;
        
        if (scheme == NULL || method == NULL || filenamerecvcli == NULL ||
        	strcmp(scheme, "GETFILE") != 0 || strcmp(method, "GET") != 0 
        	|| filename != '/') {
            char header[] = "GETFILE FILE_NOT_FOUND  \r\n\r\n";
            write(ctx->sockconn, header, strlen(header) + 1);
        }else {
            gfs->handler(ctx, filenamerecvcli, gfs->handlearg);
        }
        
        fprintf(stdout, "method header: %s %s %s\n", scheme, method, filenamerecvcli);
    }

}
