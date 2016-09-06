#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <getopt.h>

/* Be prepared accept a response of this length */
#define BUFSIZE 4096

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  echoclient [options]\n"                                                    \
"options:\n"                                                                  \
"  -h                  Show this help message\n"                              \
"  -m                  Message to send to server (Default: \"Hello World!\"\n"\
"  -p                  Port (Default: 8080)\n"                                \
"  -s                  Server (Default: localhost)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
        {"server",  required_argument, NULL, 's'},
        {"port",    required_argument, NULL, 'p'},
        {"message", required_argument, NULL, 'm'},
        {"help",    no_argument,       NULL, 'h'},
        {NULL, 0,                      NULL, 0}
};


/* Main ========================================================= */
int main(int argc, char **argv) {
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 8080;
    char *message = "Hello World!";

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:m:h", gLongOptions, NULL)) != -1) {
        switch (option_char) {
            case 's': // server
                hostname = optarg;
                break;
            case 'p': // listen-port
                portno = atoi(optarg);
                break;
            case 'm': // server
                message = optarg;
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

    if (NULL == message) {
        fprintf(stderr, "%s @ %d: invalid message\n", __FILE__, __LINE__);
        exit(1);
    }

    if ((portno < 1025) || (portno > 65535)) {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    if (NULL == hostname) {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    /* Socket Code Here */
	
	int hSocket;
	struct hostent* pHostInfo;
	struct sockaddr_in Address;		/* Internet socket add*/
	long nHostAddress;
	char pBuffer[BUFSIZE];
//	unsigned nReadAmount;
//	char strHostName[255];
//	int nHostPort;

//	printf("\nMaking a socket");
	/* make a socket*/
	hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	strcpy(pBuffer,message);
	if(hSocket < 0)
	{
		printf("\nCould not make a socket\n");
		return 0;
	}

	/* get IP address from name */

	pHostInfo=gethostbyname(hostname);
	/* copy address into long */
	memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

	/* fill address struct */
	Address.sin_addr.s_addr=nHostAddress;
	Address.sin_port=htons(portno);
	Address.sin_family=AF_INET;

//	printf("\nConnecting to %s on port %d",hostname,portno);

	/* connect to host*/
	if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address))
		< 0)
	{
		printf("\nCould not connect to host\n");
		return 0;
	}
	
	int n =write(hSocket,pBuffer,BUFSIZE);
	if(n < 0)
		printf("ERROR writing to socket!\n");
	
	n=read(hSocket,pBuffer,BUFSIZE);

	if(n < 0)
		printf("ERROR reading to socket!\n");
//	printf("\nReceived \"%s\" from server \n",pBuffer);
	/* write what we received back to server */
//	write(hSocket,message,nReadAmount);
	strcpy(message,pBuffer);
	printf("%s",message);

//	printf("\nClosing socket\n");
	/* close socket */
	if(close(hSocket) < 0)
	{
		printf("\nCould not close socket\n");
		return 0;
	}

	return 0;

}
