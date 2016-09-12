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

};

void gfc_cleanup(gfcrequest_t *gfr){

}

gfcrequest_t *gfc_create(){

    return (gfcrequest_t *)NULL;

}

size_t gfc_get_bytesreceived(gfcrequest_t *gfr){
    return -1;
}

size_t gfc_get_filelen(gfcrequest_t *gfr){
    return -1;
}

gfstatus_t gfc_get_status(gfcrequest_t *gfr){
    return -1;
}

void gfc_global_init(){
}

void gfc_global_cleanup(){
}

int gfc_perform(gfcrequest_t *gfr){
    return -1;
}

void gfc_set_headerarg(gfcrequest_t *gfr, void *headerarg){

}

void gfc_set_headerfunc(gfcrequest_t *gfr, void (*headerfunc)(void*, size_t, void *)){

}

void gfc_set_path(gfcrequest_t *gfr, char* path){

}

void gfc_set_port(gfcrequest_t *gfr, unsigned short port){

}

void gfc_set_server(gfcrequest_t *gfr, char* server){
  
}

void gfc_set_writearg(gfcrequest_t *gfr, void *writearg){

}

void gfc_set_writefunc(gfcrequest_t *gfr, void (*writefunc)(void*, size_t, void *)){
  
}

char* gfc_strstatus(gfstatus_t status){
    return (char *)NULL;
}

