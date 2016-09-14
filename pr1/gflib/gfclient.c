#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h> 
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "gfclient.h"

#define BUFSIZE 4096
#define HEADER_LEN 512

const char * const scheme = "GETFILE";
const char * const method = "GET";
const char * const end_marker = "\\r\\n\\r\\n";

struct gfcrequest_t
{
	int filelen;
	char clientPort[12];
	char servIP[50];
	gfstatus_t ret_status;
	char spath[1024];
	int total_bytes;
	char filecontent[BUFSIZE];

	void (*wri_handler)(void*,size_t,void *);
	void * fDesc;

	void (*head_handler)(void*, size_t, void *);
	void *headarg;
};

static pthread_mutex_t counter_mutex;
gfcrequest_t *gfc;

void gfc_cleanup(gfcrequest_t *gfr){
	free(gfr);
	gfr=NULL;
}

gfcrequest_t *gfc_create(){
	gfc = (gfcrequest_t*)malloc(1*sizeof(gfcrequest_t));

	return gfc;
}

size_t gfc_get_bytesreceived(gfcrequest_t *gfr){
    return gfr->total_bytes;
}

size_t gfc_get_filelen(gfcrequest_t *gfr){
    return gfr->filelen;
}

gfstatus_t gfc_get_status(gfcrequest_t *gfr){
   	return gfr->ret_status;
}

void gfc_global_init(){
	pthread_mutex_lock(&counter_mutex);
	gfc = (gfcrequest_t*)malloc(1*sizeof(gfcrequest_t));
	pthread_mutex_unlock(&counter_mutex);
}

void *get_in_addr(struct sockaddr *sa)
{
	return &(((struct sockaddr_in*)sa)->sin_addr);
}

void gfc_global_cleanup(){
	pthread_mutex_lock(&counter_mutex);
	free(gfc);
	pthread_mutex_unlock(&counter_mutex);
}

int gfc_perform(gfcrequest_t *gfr){
    
	struct addrinfo hints, *servinfo, *p;
	int hSocket, rv, totalBytesRecevd;
	char req_header[512];
	int currentBytesRcvd;
	int header_len;

	char str[INET6_ADDRSTRLEN];
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;	

	if((rv = getaddrinfo(NULL,gfr->clientPort, &hints,&servinfo))!=0)
	{
		printf("ERROR in getaddrinfo line 88!\n");
		return 1;
	}

	for(p = servinfo; p!=NULL; p = p->ai_next)
	{
		if((hSocket = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) < 0)
		{
			printf("Client:socket");
			continue;
		}

		if(connect(hSocket,p->ai_addr,p->ai_addrlen) < 0)
		{
			close(hSocket);
			printf("client:connect");
			continue;
		}

		break;
	}

	if(p == NULL)
	{
		printf("Client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),str,sizeof(str));

	header_len = snprintf(req_header,512, "%s%s%s%s", scheme, method,gfc->spath, end_marker);

	if(send(hSocket,req_header,header_len,0)!=header_len)
		printf("send() sent a different number of bytes than expected\n");
	
	if((currentBytesRcvd=recv(hSocket,gfr->filecontent,BUFSIZE,0))<=0)
		printf("recv() failed or connection closed prematurely\n");
	
	int req_no=1;
	gfr->total_bytes = 0;

	char buffer[BUFSIZE];
	size_t total=0;
	while(1)
	{
		ssize_t nb = recv(str,buffer,BUFSIZE,0);
		if(nb < 0) printf("recv failed");
		if(nb == 0) break;	// end of stream
		if(write(hSocket,buffer,nb) == -1)
			printf("File write failed\n");
		total += nb;
		printf("Received Bytes:%d\n",gfr->total_bytes);
	}

	if(close(str) == -1) printf("socket close failed\n");
	if(close(hSocket)) printf("file close failed\n");
	
	//gfr->fl_handler(gfr->filecontent,nb,gfr->fDesc);
	//gfr->total_bytes+=nb;

//	printf("Received Bytes:%d\n",gfr->total_bytes);

	return 0;

}

void gfc_set_headerarg(gfcrequest_t *gfr, void *headerarg){
	gfr->headarg = headerarg;
}

void gfc_set_headerfunc(gfcrequest_t *gfr, void (*headerfunc)(void*, size_t, void *)){
	gfr->head_handler = headerfunc;
}

void gfc_set_path(gfcrequest_t *gfr, char* path){
	strcpy(gfr->spath,path);
}

void gfc_set_port(gfcrequest_t *gfr, unsigned short port){
	snprintf(gfr->clientPort,12,"%u",port);
}

void gfc_set_server(gfcrequest_t *gfr, char* server){
	strcpy(gfr->servIP,server);
}

void gfc_set_writearg(gfcrequest_t *gfr, void *writearg){
	gfr->fDesc = writearg;
}

void gfc_set_writefunc(gfcrequest_t *gfr, void (*writefunc)(void*, size_t, void *)){
	gfr->wri_handler = writefunc; 
}

char* gfc_strstatus(gfstatus_t status){
//    return (char *)NULL;
	static const char *status_array[] = {"GF_OK","GF_FILE_NOT_FOUND","GF_ERROR","GF_INVALID"};

	return status_array[status];
}

