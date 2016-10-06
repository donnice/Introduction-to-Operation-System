#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "gfserver.h"
#include "content.h"

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  gfserver_main [options]\n"                                                 \
"options:\n"                                                                  \
"  -h                  Show this help message.\n"                             \
"  -c [content_file]   Content file mapping keys to content files\n"          \
"  -p [listen_port]    Listen port (Default: 8080)\n"                         \
"  -t [nthreads]       Number of threads (Default: 1)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"port",          required_argument,      NULL,           'p'},
  {"content",       required_argument,      NULL,           'c'},
  {"nthreads",      required_argument,      NULL,           't'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};


extern ssize_t handler_get(gfcontext_t *ctx, char *path, void* arg);

static void _sig_handler(int signo){
  if (signo == SIGINT || signo == SIGTERM){
    exit(signo);
  }
}

/* Main ========================================================= */
int main(int argc, char **argv) {
  int option_char = 0;
  unsigned short port = 8080;
  char *content = "content.txt";
  gfserver_t *gfs;
  int nthreads = 1;

  if (signal(SIGINT, _sig_handler) == SIG_ERR){
    fprintf(stderr,"Can't catch SIGINT...exiting.\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGTERM, _sig_handler) == SIG_ERR){
    fprintf(stderr,"Can't catch SIGTERM...exiting.\n");
    exit(EXIT_FAILURE);
  }

  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "p:t:c:h", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 'p': // listen-port
        port = atoi(optarg);
        break;
      case 't': // nthreads
        nthreads = atoi(optarg);
        break;
      case 'c': // file-path
        content = optarg;
        break;                                          
      case 'h': // help
        fprintf(stdout, "%s", USAGE);
        exit(0);
        break;       
      default:
        fprintf(stderr, "%s", USAGE);
        exit(1);
    }
  }

  /* not useful, but it ensures the initial code builds without warnings */
  if (nthreads < 1) {
    nthreads = 1;
  }
  
  content_init(content);

  /*Initializing server*/
  gfs = gfserver_create();

  /*Setting options*/
  gfserver_set_port(gfs, port);
  gfserver_set_maxpending(gfs, 100);
  gfserver_set_handler(gfs, handler_get);
  gfserver_set_handlerarg(gfs, NULL);

  /*Loops forever*/
  gfserver_serve(gfs);
}
