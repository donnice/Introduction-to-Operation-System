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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

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
"  transferserver [options]\n"                                                \
"options:\n"                                                                  \
"  -f                  Filename (Default: bar.txt)\n"                         \
"  -h                  Show this help message\n"                              \
"  -p                  Port (Default: 8080)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
        {"filename", required_argument, NULL, 'f'},
        {"port",     required_argument, NULL, 'p'},
        {"help",     no_argument,       NULL, 'h'},
        {NULL, 0,                       NULL, 0}
};


int main(int argc, char **argv) {
    int option_char;
    int portno = 8080; /* port to listen on */
    char *filename = "bar.txt"; /* file to transfer */

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "f:p:h", gLongOptions, NULL)) != -1) {
        switch (option_char) {
            case 'p': // listen-port
                portno = atoi(optarg);
                break;
            case 'f': // listen-port
                filename = optarg;
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

    if (NULL == filename) {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    if ((portno < 1025) || (portno > 65535)) {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    /* Socket Code Here */
	int hServerSocket;		// server socket
	int hSocket;			// peer socket
	char pBuffer[BUFSIZE];
	struct sockaddr_in Address;	// server_addr
	FILE *fd;						// file open
//	int sent_bytes = 0;
//	struct stat file_stat;
//	int remain_data;

	/* Create server socket */
	hServerSocket = socket(AF_INET,SOCK_STREAM,0);
	if(hServerSocket == -1)
	{
		printf("Error creating socket!");
		return 0;
	}

	Address.sin_family = AF_INET;
	Address.sin_addr.s_addr = INADDR_ANY;
	Address.sin_port=htons(portno);

	printf("\nBinding to port %d",portno);

	/* bind to a port */
	if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address))
		< 0)	// binds a socket to an address
	{
		printf("\nCould not connect to the host!\n");
		return 0;
	}

	printf("\nMaking a listen queue of 5 ports!\n");

	if((listen(hServerSocket,5)) < 0)
	{
		printf("Error on listen!\n");
		return 0;
	}
	
	fd = fopen(filename, "r");
	if(fd == NULL)
	{
		printf("Error opening file!\n");
		return 0;
	}

	/* Get file stats 
	if(fstat(fd, &file_stat) < 0)
	{
		printf("Error fstat!\n");
		return 0;
	}*/

	int nAddressSize=sizeof(struct sockaddr_in);

	/* Accepting incoming peers */
	hSocket = accept(hServerSocket,(struct sockaddr*)&Address,
		(socklen_t *)&nAddressSize);
	if(hSocket < 0)
	{
		printf("Error on accept!\n");

		return 0;
	}
	printf("Accept peer => %s\n", inet_ntoa(Address.sin_addr));

//	remain_data = file_stat.st_size;

	/*	while(((sent_bytes = sendfile(hSocket,fd, NULL ,BUFSIZE)) > 0)
		 && (remain_data > 0))
	{
		printf("1. Server sent %d bytes from file's data and remaining data = %d\n", sent_bytes,remain_data);
		remain_data -= sent_bytes;
		printf("2. Server sent %d bytes from file's data and remaining data = %d\n", sent_bytes, remain_data);
	}*/
	int fs_block_sz=0;
	while((fs_block_sz=fread(pBuffer,sizeof(char),BUFSIZE,fd)) > 0)
	{
		if(send(hServerSocket,pBuffer,fs_block_sz,0) < 0)
		{
			printf("Error:Failed to send file!\n");
			exit(1);
		}
	}
	
	printf("\nClosing the socket");
	close(hSocket);
	close(hServerSocket);

	return 0;
}
