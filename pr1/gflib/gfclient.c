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

#include "gfclient.h"

#define BUFSIZE 4096

#define DEFAULTPORT 8888

struct gfcrequest_t  {
    
    //socket connection
    char *server;
    char *path;
    unsigned short port;
    
    //connection status
    gfstatus_t status;
    //char *statusStr;
    
    //received data including header
    size_t bytesreceived;
    size_t bytesFileReceived;

    //header pointer
    void *header;
    size_t headlen;

    //call back funtion
    void (*writefunc)(void *, size_t, void *);
    void (*headfunc)(void *, size_t, void *);
    //callback function arguments
    void *writearg;
    void *headarg;
    
    //file content
    void *fileData;
    //filename length
    size_t filelen;
    //file name
    char *filelenstr;
};

gfcrequest_t *gfr;

gfcrequest_t *gfc_create(){
    //create gfc struct and initialize its members
    gfr =  (struct gfcrequest_t *)malloc(sizeof(struct gfcrequest_t));

    gfr->headfunc = NULL;
    gfr->headarg = NULL;
    gfr->writefunc = NULL;
    gfr->writearg = NULL;
    gfr->fileData = NULL;
    gfr->server = NULL;
    gfr->path = NULL;
    gfr->status = GF_FILE_NOT_FOUND;
    gfr->filelen = 0;
    gfr->filelenstr = NULL;
    gfr->headlen = 0;
    gfr->bytesreceived = 0;
    gfr->bytesFileReceived = 0;

    return gfr;
}

void gfc_set_server(gfcrequest_t *gfr, char* server){
    if(server != NULL)
    {
        gfr->server = (char*)malloc(sizeof(char) * (strlen(server) + 1));
        strcpy(gfr->server, server);
    }else
    {
        perror("ERROR SETTING SERVER\n");
    }
    
}

void gfc_set_path(gfcrequest_t *gfr, char* path){
    if(path != NULL)
    {
        gfr->path = (char*)malloc(sizeof(char) * (strlen(path) + 1));
        strcpy(gfr->path, path);
    }
    else
    {
        fprintf(stderr, "ERROR SETTING PATH\n");
        exit(-1);
    }
}

void gfc_set_port(gfcrequest_t *gfr, unsigned short port){
    gfr->port = port;
}

void gfc_set_headfunc(gfcrequest_t *gfr, void (*headfunc)(void*, size_t, void *)){
    gfr->headfunc = headfunc;

}

void gfc_set_headerarg(gfcrequest_t *gfr, void *headerarg){
  
    gfr->headarg = malloc(sizeof headerarg);
    if (gfr->headarg == NULL){
        
        fprintf(stderr,"ERROR SETTING headerarg\n");
        exit(-1);
    
    }
    gfr->headarg = headerarg;
}

void gfc_set_writefunc(gfcrequest_t *gfr, void (*writefunc)(void*, size_t, void *)){
    gfr->writefunc = writefunc;
}

void gfc_set_writearg(gfcrequest_t *gfr, void *writearg){
    gfr->writearg = malloc(sizeof(writearg));
    if (gfr->writearg == NULL){
        
        fprintf(stderr,"ERROR SETTING writearg\n");
        exit(-1);
    }
    gfr->writearg = writearg;
}

void sockInitClient(gfcrequest_t *gfr, int* sockfd, struct sockaddr_in* serveraddr)
{
    /* socket: create the socket */
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) 
    {
        
        fprintf(stderr,"ERROR opening socket\n");
        exit(-1);
        
    }

    /* gethostbyname: get the server's DNS entry */
    struct hostent* server = gethostbyname(gfr->server);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", gfr->server);
        exit(-1);
    }

    /* build the server's Internet address */
    bzero((char *) serveraddr, sizeof(struct sockaddr_in));
    serveraddr->sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
      (char *)&serveraddr->sin_addr.s_addr, server->h_length);
    serveraddr->sin_port = htons(gfr->port);
}

char *clientscheme(char *path)
{
    char * request = (char *) malloc(22 + strlen(path) );
    strcpy(request, "GETFILE GET ");
    strcat(request, path);
    strcat(request, " \r\n\r\n");
    return request;
}

int gfc_perform(gfcrequest_t *gfr){
    
    int sockfd = 0;
    struct sockaddr_in serveraddr;

    char *fullData = "";

    //int set_reuse_addr = 1;
    
    //0: incomplete 1: compelete
    int headercheck = 0;
    
    //socket create and init
    sockInitClient(gfr, &sockfd, &serveraddr);

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
    { 
        perror("ERROR connecting"); 
        exit(0);
    }
 
    char * request = clientscheme(gfr->path);
    
    send(sockfd, request, strlen(request), 0);
    
    fprintf(stdout, "Client request scheme: %s.\n", request);
    fflush(stdout);
    
    char buf[BUFSIZE];

    size_t bytesReceivedTotal = 0;
    size_t bytesReceivedFILE = 0;
    
    while (1) {

        //clear buffer
        bzero(buf, BUFSIZE);

        int bytesrecv = recv(sockfd, buf, BUFSIZE, 0);
        bytesReceivedTotal = bytesReceivedTotal + bytesrecv;
        gfr->bytesreceived = bytesReceivedTotal;

        //socket error
        if (bytesrecv < 0) {
            return -1;
        }
        
        //socket abortion
        if (bytesrecv == 0) {
            
            char *scheme = strtok(fullData, " ");
            char *status = strtok(NULL, " ");
            char *filelen = strtok(NULL, " \r\n");
           
            gfr->bytesreceived = bytesReceivedTotal;

           if (filelen != NULL) {
                gfr->filelen = atol(filelen);
                gfr->filelenstr = filelen;
                
            }

            if (strcmp(status, "OK") != 0 || strcmp(scheme, "GETFILE") != 0) {
                gfr->status = GF_INVALID;  
            }
            
            close(sockfd);
            return -1;
        }
        
        if (!headercheck) {
            
            char * currecvdata = (char *) malloc(bytesReceivedTotal + 1);
            strcpy(currecvdata, fullData);
            strcat(currecvdata, buf);
            
            fullData = currecvdata;
            
            char * checkformat = (char *) malloc(strlen(currecvdata) + 1);
            strcpy(checkformat, currecvdata);
            
            if (bytesReceivedTotal > 7) {
                char *scheme = strtok(checkformat, " ");
                if (strcmp(scheme, "GETFILE") != 0) {
                    gfr->status = GF_INVALID;
                    gfr->bytesFileReceived = bytesReceivedTotal;
                    return -1;
                }
                
                if (bytesReceivedTotal > 10) {
                    char *statusStr = strtok(NULL, " \r\n");
                    if (strcmp(statusStr, "OK") != 0) {
                        if (strcmp(statusStr, "FILE_NOT_FOUND") == 0) {
                            gfr->status = GF_FILE_NOT_FOUND;
                            if (gfr->headarg != NULL) {
                                gfr->headfunc("GETFILE FILE_NOT_FOUND", strlen("GETFILE FILE_NOT_FOUND"), gfr->headarg);
                            }
                            return 0;
                        }
                        if (strcmp(statusStr, "ERROR") == 0) {         
                            gfr->status = GF_ERROR;
                            if (gfr->headarg != NULL) {
                                gfr->headfunc("GETFILE ERROR", strlen("GETFILE ERROR"), gfr->headarg);
                            }
                            return 0;
                        }
                    }
                    else {
                        
                        gfr->status = GF_OK;
                        
                        char *filelen = strtok(strtok(NULL, ""), " \r\n");
                        char *temp1 = strtok(NULL, "\n");
                        char *temp2 = strtok(NULL, "\n");
                        char *fileData = strtok(NULL, "\n");
                        
                        if (filelen != NULL && temp1 != NULL) {
                            gfr->filelenstr = filelen;
                            gfr->filelen = atol(filelen);
                            
                            headercheck = 1;
                        }
                        
                        if (temp2 != NULL && strcmp(temp2, "\r") != 0) {
                            fileData = temp2;
                        }
                        
                        if (fileData != NULL) {
                            int restpartdata = strlen(fileData);
                            bytesReceivedFILE = bytesReceivedFILE + restpartdata;
                            gfr->bytesFileReceived = bytesReceivedFILE;
                            
                            char *header = (char *) malloc(11 + strlen(filelen) + 5 + 1);
                            strcpy(header, "GETFILE OK ");
                            strcat(header, gfr->filelenstr);

                            strcat(header, " \r\n\r\n");
                            
                            gfr->header = header;
                            gfr->headlen = strlen(header);
                            
                            if (gfr->headarg != NULL) {
                                gfr->headfunc(gfr->header, gfr->headlen, gfr->headarg);
                            }
                            
                            gfr->writefunc(fileData, restpartdata, gfr->writearg);
                          
                            
                            if (bytesReceivedFILE == gfr->filelen) {
                                fprintf(stdout, "#############File Transfer Correct !!!#############\n");
                                close(sockfd);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
        else {
            
            bytesReceivedFILE = bytesReceivedFILE + bytesrecv;
            gfr->writefunc(buf, bytesrecv, gfr->writearg);
            gfr->bytesFileReceived = bytesReceivedFILE;

            
            fprintf(stdout, "Received Bytes: %zu.\n Actual Bytes: %zu.\n", bytesReceivedFILE, gfr->filelen);
            fflush(stdout);

            if(bytesReceivedFILE == gfr->filelen){
                fprintf(stdout, "#############File Transfer Correct !!!#############\n");
                close(sockfd);
                return 0;
            }
            
        }
    }
    
    close(sockfd);
}

gfstatus_t gfc_get_status(gfcrequest_t *gfr){

    return gfr->status;
    
}

char* gfc_strstatus(gfstatus_t status){
     
    switch(status)
    { 
        case GF_OK:
            return "OK";
        case GF_FILE_NOT_FOUND:
            return "FILE_NOT_FOUND";
        case GF_ERROR:
            return "ERROR";
        default:    
            return "INVALID";         
    }
}

size_t gfc_get_filelen(gfcrequest_t *gfr){
    return gfr->filelen;
}

size_t gfc_get_bytesreceived(gfcrequest_t *gfr){
   
    return gfr->bytesFileReceived;
}

void gfc_cleanup(gfcrequest_t *gfr){


    free(gfr->server);
    gfr->server = NULL;
    gfr->port = DEFAULTPORT;

    free(gfr->path);
    gfr->path = NULL;

    free(gfr->fileData);
    gfr->fileData = NULL;

    free(gfr->filelenstr);
    gfr->filelenstr = NULL;

    free(gfr->headarg);
    gfr->headarg = NULL;

    free(gfr->writearg);
    gfr->writearg = NULL;

	free(gfr);
	gfr=NULL;

}


void gfc_global_init(){
    fprintf(stdout, "Global Init\n");
}

void gfc_global_cleanup(){
    fprintf(stdout, "Global Cleanup\n");
}

