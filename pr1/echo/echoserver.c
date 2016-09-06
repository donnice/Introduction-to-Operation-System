#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#if 0
/*
 * Structs exported from netinet/in.h (for easy reference)
 */

/* Internet address */
struct in_addr {
  unsigned int s_addr; 
};

/* Internet style socket address */
struct sockaddr_in  {
  unsigned short int sin_family; /* Address family */
  unsigned short int sin_port;   /* Port number */
  struct in_addr sin_addr;	 /* IP address */
  unsigned char sin_zero[...];   /* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
  char    *h_name;        /* official name of host */
  char    **h_aliases;    /* alias list */
  int     h_addrtype;     /* host address type */
  int     h_length;       /* length of address */
  char    **h_addr_list;  /* list of addresses */
}
#endif

#define BUFSIZE 4096

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  echoserver [options]\n"                                                    \
"options:\n"                                                                  \
"  -h                  Show this help message\n"                              \
"  -n                  Maximum pending connections\n"                         \
"  -p                  Port (Default: 8080)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
        {"port",        required_argument, NULL, 'p'},
        {"maxnpending", required_argument, NULL, 'n'},
        {"help",        no_argument,       NULL, 'h'},
        {NULL, 0,                          NULL, 0}
};


int main(int argc, char **argv) {
    int option_char;
    int portno = 8080; /* port to listen on */
    int maxnpending = 5;

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:n:h", gLongOptions, NULL)) != -1) {
        switch (option_char) {
            case 'p': // listen-port
                portno = atoi(optarg);
                break;
            case 'n': // server
                maxnpending = atoi(optarg);
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

    if ((portno < 1025) || (portno > 65535)) {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    if (maxnpending < 1) {
        fprintf(stderr, "%s @ %d: invalid pending count (%d)\n", __FILE__, __LINE__, maxnpending);
        exit(1);
    }


    /* Socket Code Here */
	int hSocket, hServerSocket;		/* handle to socket*/
//	struct hostent* pHostInfo;		/* holds info of a machine*/
	struct sockaddr_in Address;		/* Internet socket address */
	int nAddressSize=sizeof(struct sockaddr_in);
	char pBuffer[BUFSIZE];

	printf("\nStarting server!");

	printf("\nMaking socket");

	hServerSocket=socket(AF_INET,SOCK_STREAM,0);

	if(hServerSocket < 0)
	{
		printf("\nCould not make a socket\n");
		return 0;
	}
	
	int enable = 1;
	if(setsockopt(hServerSocket,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int)) < 0)
		printf("setsockopt(SO_REUSEADDR) failed.\n");
	
	Address.sin_family=AF_INET;
	Address.sin_addr.s_addr=INADDR_ANY;
	Address.sin_port=htons(portno);

	printf("\nBinding to port %d",portno);

	/* bind to a port*/
	if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address))
		< 0)	// binds a socket to an address
	{
		printf("\nCould not connect to host\n");
		return 0;
	}

	/* get port number */
	//getsocketname(hServerSocket,(struct sockaddr *)&Address,
		//(socklen_t *)&nAddressSize);
	
	printf("Server\n\
			sin_family			=%d\n\
			sin_addr.s_addr		=%d\n\
			sin_port			=%d\n"
			,Address.sin_family
			,Address.sin_addr.s_addr
			,ntohs(Address.sin_port)
	);
	
	printf("\nMaking a listen queue of %d elements",maxnpending);

	if(listen(hServerSocket,maxnpending) < 0)
	{
		printf("\nCould not listen!\n");
		return 0;
	}

	for(;;)
	{
		printf("\nWaiting for a connection\n");

		hSocket = accept(hServerSocket,(struct sockaddr*)&Address,
			(socklen_t *)&nAddressSize);

		printf("\nGot a connection\n");
		//strcpy(pBuffer,"I received it!");
		bzero(pBuffer,4096);
		read(hSocket,pBuffer,BUFSIZE);
		printf("Here is the message: %s\n",pBuffer);
		/* write what we received back to server */

		write(hSocket,pBuffer,strlen(pBuffer)+1);
		/* read from socket into buffer */
//		read(hSocket,pBuffer,BUFSIZE);

		printf("\nClosing the socket");

		if(close(hSocket) < 0)
		{
			printf("\nCould not close socket!\n");
			return 0;
		}
	}
}
