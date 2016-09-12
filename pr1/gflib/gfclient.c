#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h> 
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>

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

	void (*wri_handler)(void*,size_t,void *);
	void * fDesc;

	void (*head_handler)(void*, size_t, void *);
	void *headarg;
};

static pthread_mutex_t counter_mutex;
gfcrequest_t *gfc;

void gfc_cleanup(gfcrequest_t *gfr){
	freee(gfr);
	gfr=NULL;
}

gfcrequest_t *gfc_create(){

    return (gfcrequest_t *)NULL;

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

void gfc_global_cleanup(){
	pthread_mutex_lock(&counter_mutex);
	free(gfc);
	pthread_mutex_unlock(&counter_mutex);
}

int gfc_perform(gfcrequest_t *gfr){
    return -1;
}

void gfc_set_headerarg(gfcrequest_t *gfr, void *headerarg){
	gfr->headerarg = headerarg;
}

void gfc_set_headerfunc(gfcrequest_t *gfr, void (*headerfunc)(void*, size_t, void *)){
	gfr->head_handler = headerfunc;
}

void gfc_set_path(gfcrequest_t *gfr, char* path){
	strcpy(gfr->spath,path);
}

void gfc_set_port(gfcrequest_t *gfr, unsigned short port){
	gfr->clientPort = port;
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
    return (char *)NULL;
}

