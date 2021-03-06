#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "workload.h"
#include "gfclient.h"


#define USAGE                                                                 \
"usage:\n"                                                                    \
"  webclient [options]\n"                                                     \
"options:\n"                                                                  \
"  -s [server_addr]    Server address (Default: 0.0.0.0)\n"                   \
"  -p [server_port]    Server port (Default: 8888)\n"                         \
"  -w [workload_path]  Path to workload file (Default: workload.txt)\n"       \
"  -t [nthreads]       Number of threads (Default 1)\n"                       \
"  -n [num_requests]   Requests download per thread (Default: 1)\n"           \
"  -h                  Show this help message\n"                              \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"server",        required_argument,      NULL,           's'},
  {"port",          required_argument,      NULL,           'p'},
  {"workload-path", required_argument,      NULL,           'w'},
  {"nthreads",      required_argument,      NULL,           't'},
  {"nrequests",     required_argument,      NULL,           'n'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};

pthread_mutex_t _fileLock = PTHREAD_MUTEX_INITIALIZER;

/*multithread callback function's arguments struct*/
typedef struct mtarg mtarg;
struct mtarg {
  int port;
  int nrequest;
  char *server;
};

static void Usage() {
  fprintf(stdout, "%s", USAGE);
}

static void localPath(char *req_path, char *local_path){
  static int counter = 0;

  sprintf(local_path, "%s-%06d", &req_path[1], counter++);
}

static FILE* openFile(char *path){
  char *cur, *prev;
  FILE *ans;

  /* Make the directory if it isn't there */
  prev = path;
  while(NULL != (cur = strchr(prev+1, '/'))){
    *cur = '\0';

    if (0 > mkdir(&path[0], S_IRWXU)){
      if (errno != EEXIST){
        perror("Unable to create directory");
        exit(EXIT_FAILURE);
      }
    }

    *cur = '/';
    prev = cur;
  }

  if( NULL == (ans = fopen(&path[0], "w"))){
    perror("Unable to open file");
    exit(EXIT_FAILURE);
  }

  return ans;
}

/* Callbacks ========================================================= */
static void writecb(void* data, size_t data_len, void *arg){
  FILE *file = (FILE*) arg;

  fwrite(data, 1, data_len, file);
}

void *gfcallback(void* parg)
{
    
  //convert param type!
  mtarg *mtagu = (mtarg*) parg; 
    int j;
for(j = 0; j < mtagu->nrequest; j++)
{
  char *req_path;
  char local_path[512];
  FILE *file;
  gfcrequest_t *gfr;
  int returncode = 0;

  req_path = workload_get_path();
  printf("%d\n",mtagu->nrequest);

  if(strlen(req_path) > 256){
    fprintf(stderr, "Request path exceeded maximum of 256 characters\n.");
    exit(EXIT_FAILURE);
  }

  localPath(req_path, local_path);
    
  pthread_mutex_lock(&_fileLock);
  
  file = openFile(local_path);
  gfr = gfc_create();
  gfc_set_server(gfr, mtagu->server);
  gfc_set_path(gfr, req_path);
  gfc_set_port(gfr, mtagu->port);
  gfc_set_writefunc(gfr, writecb);
  gfc_set_writearg(gfr, file);
  pthread_mutex_unlock(&_fileLock);
      
  fprintf(stdout, "Requesting %s%s\n", mtagu->server, req_path);

  if ( 0 > (returncode = gfc_perform(gfr))){
    fprintf(stdout, "gfc_perform returned an error %d\n", returncode);
    fclose(file);
    if ( 0 > unlink(local_path))
      fprintf(stderr, "unlink failed on %s\n", local_path);
  }
  else {
      fclose(file);
  }

  if ( gfc_get_status(gfr) != GF_OK){
    if ( 0 > unlink(local_path))
      fprintf(stderr, "unlink failed on %s\n", local_path);
  }

  fprintf(stdout, "Status: %s\n", gfc_strstatus(gfc_get_status(gfr)));
  fprintf(stdout, "Received %zu of %zu bytes\n", gfc_get_bytesreceived(gfr), gfc_get_filelen(gfr));
  gfr=NULL;
}
return 0;
}
/* Main ========================================================= */
int main(int argc, char **argv) {
/* COMMAND LINE OPTIONS ============================================= */
  char *server = "localhost";
  unsigned short port = 8080;
  char *workload_path = "workload.txt";

  int i;
  int option_char = 0;
  int nrequests = 1;
  int nthreads = 1;

  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "s:p:w:n:t:h", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 's': // server
        server = optarg;
        break;
      case 'p': // port
        port = atoi(optarg);
        break;
      case 'w': // workload-path
        workload_path = optarg;
        break;
      case 'n': // nrequests
        nrequests = atoi(optarg);
        break;
      case 't': // nthreads
        nthreads = atoi(optarg);
    
        break;
      case 'h': // help
        Usage();
        exit(0);
        break;                      
      default:
        Usage();
        exit(1);
    }
  }

  if( EXIT_SUCCESS != workload_init(workload_path)){
    fprintf(stderr, "Unable to load workload file %s.\n", workload_path);
    exit(EXIT_FAILURE);
  }

  gfc_global_init();

  //initialize multi thread callback arguments (struct)
  mtarg* mtagu = (mtarg*)malloc(sizeof(mtarg));
  mtagu->nrequest = nrequests;
  mtagu->server = server;
  mtagu->port = port;
 
  /*Making the requests...*/
  pthread_attr_t attr;
  pthread_t* t_id = (pthread_t*)malloc(sizeof(pthread_t)*nthreads);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
  //set all threads to be joinable.
    
   if (pthread_mutex_init(&_fileLock, NULL) != 0)
   {
        fprintf(stderr, "ERROR lock mutex failed.\n");
        exit(-1);
   }  
  for(i = 0; i < nthreads; i++){
        if (pthread_create( &t_id[i], NULL, gfcallback, (void*)mtagu) < 0)
        {
          perror("ERROR create multi-thread.\n");
          exit(-1);
        }
  }
   
   void *status;
  for(i = 0; i < nthreads; i++){
      if(pthread_join(t_id[i], &status)){
         perror("Error thread unable to join.\n");
         exit(-1);
      }
   }
  fprintf(stdout, "all threads are joined.\n");

  //release memory
  pthread_attr_destroy(&attr);
  free(mtagu);
  free(t_id);
  gfc_global_cleanup();

  return 0;
}  
