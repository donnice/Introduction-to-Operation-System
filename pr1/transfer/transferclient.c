#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define BUFSIZE 4096

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  transferclient [options]\n"                                                \
"options:\n"                                                                  \
"  -h                  Show this help message\n"                              \
"  -o                  Output file (Default foo.txt)\n"                       \
"  -p                  Port (Default: 8080)\n"                                \
"  -s                  Server (Default: localhost)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
        {"server", required_argument, NULL, 's'},
        {"port",   required_argument, NULL, 'p'},
        {"output", required_argument, NULL, 'o'},
        {"help",   no_argument,       NULL, 'h'},
        {NULL, 0,                     NULL, 0}
};

/* Main ========================================================= */
int main(int argc, char **argv) {
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 8080;
    char *filename = "foo.txt";

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:o:h", gLongOptions, NULL)) != -1) {
        switch (option_char) {
            case 's': // server
                hostname = optarg;
                break;
            case 'p': // listen-port
                portno = atoi(optarg);
                break;
            case 'o': // filename
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

    if (NULL == hostname) {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }


    /* Socket Code Here */
	int hSocket;				// client socket
	ssize_t len;
	struct hostent* pHostInfo;
	struct sockaddr_in Address;	// remote_addr
	int fileSize;				// file_size
	FILE *received_file;
	int remain_data = 0;
	long nHostAddress;
	char pBuffer[BUFSIZE];

	/* get IP address from name */
	pHostInfo=gethostbyname(hostname);
	/* copy address into long*/
	memcpy(&nHostAdress,pHostInfo->h_addr,pHostInfo->h_length);

	/* Construct remote_addr struct */
	Address.sin_family = AF_INET;
	Address.sin_port=htons(portno);
	Address.sin_addr.s_addr=nHostAddress;

	/* make a socket */
	hSocket=socket(AF_INET,SOCK_STREAM,0);
	if(hSocket < 0)
	{
		fprintf("Error creating socket!\n");
		return 0;
	}

	/* connect to host*/
	if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) < 0 )
	{
		fprintf("Error on connect!\n");
		return 0;
	}

	/* Receiving file size */
	recv(hSocket,pBuffer,BUFSIZE,0);
	file_size=atoi(pBuffer);

	received_file=fopen(filename,"w");
	if(received_file == NULL)
	{
		fprintf("Fail to open file too!\n");
		return 0;
	}
	
	remain_data = file_size;
	
	/* keep read*/
	while(((
}
