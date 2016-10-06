#include <stdlib.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "steque.h"
#include "gfserver.h"
#include "content.h"

#define BUFFER_SIZE 4096

int numthreads = 0;
pthread_t _tid[100];
//requese queue
steque_t *requeue;

//mutal exclusion 
pthread_mutex_t reqlock;
pthread_mutex_t filelock;

typedef struct req_context_t
{
    gfcontext_t *ctx;
    char *path;
    void *arg;
    
}req_context_t;

void thd_callback();

void threads_init(int nthreads)
{
	if (pthread_mutex_init(&reqlock, NULL) != 0)
    {
        fprintf(stderr, "ERROR Queue lock mutex init failed.\n");
        exit(-1);
    }

    if (pthread_mutex_init(&filelock, NULL) != 0)
    {
        fprintf(stderr, "ERROR File lock mutex init failed.\n");
        exit(-1);
    }

    numthreads = nthreads;

    fprintf(stdout, "Creating %d threads.\n", nthreads);
    int i;
    for(i = 0; i < nthreads; i++)
    {
        if(pthread_create(&(_tid[i]), NULL, (void*)&thd_callback, NULL))
        {
            fprintf(stderr, "ERROR Creating  the %dth thread failed.\n", i);
            exit(-1);
        }
    }
}

void threads_requeue_init()
{
	puts("Request Queue Init.\n");
	requeue = (steque_t*)malloc(sizeof(steque_t));
    steque_init(requeue); 
}

void threads_release()
{
	steque_destroy(requeue);
	free(requeue);
}

void thd_callback()
{
	fprintf(stderr, "processing requests from queue.\n");

    while(1)
    {
    	//1: empty 0: non-empty
        if(!steque_isempty(requeue))
        {
        	// Dequeue
	        pthread_mutex_lock(&reqlock);
	        req_context_t* ctx = steque_pop(requeue);
	        pthread_mutex_unlock(&reqlock);

	        ///////////////////////////////////////////////////
            int fildes;
            ssize_t file_len, bytes_transferred;
            ssize_t read_len, write_len;
            char buffer[BUFFER_SIZE];

            if( 0 > (fildes = content_get(ctx->path)))
            {
                gfs_sendheader(ctx->ctx, GF_FILE_NOT_FOUND, 0);
                printf("File '%s' not found\n", ctx->path);
                continue;
            }

            /* Calculating the file size */
            file_len = lseek(fildes, 0, SEEK_END);
            printf("File size %ld calculated\n", file_len);

            gfs_sendheader(ctx->ctx, GF_OK, file_len);

            pthread_mutex_lock(&filelock);

            /* Sending the file contents chunk by chunk. */
            bytes_transferred = 0;
            while(bytes_transferred < file_len){
                read_len = pread(fildes, buffer, BUFFER_SIZE, bytes_transferred);
                if (read_len <= 0){
                    fprintf(stderr, "handle_with_file read error, %zd, %zu, %zu", read_len, bytes_transferred, file_len );
                    gfs_abort(ctx->ctx);
                    exit(-1);
                }
                write_len = gfs_send(ctx->ctx, buffer, read_len);
                if (write_len != read_len){
                    fprintf(stderr, "handle_with_file write error");
                    gfs_abort(ctx->ctx);
                    exit(-1);
                }
                bytes_transferred += write_len;
            }

            pthread_mutex_unlock(&filelock);
             ///////////////////////////////////////////////////
            
            	
        }
   
    }  
}



ssize_t handler_get(gfcontext_t *ctx, char *path, void* arg)
{
	//put received requests into queue, for mutal exclusion lock
	req_context_t* req_ctx = (req_context_t *)malloc(sizeof(req_context_t));
	req_ctx->ctx = ctx;
	req_ctx->path = path;
	req_ctx->arg = arg;
    steque_enqueue(requeue, req_ctx);
	return 0;
}
