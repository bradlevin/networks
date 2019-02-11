/******************************************************************************
* tcp_client.c
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#include "networks.h"
#include "funcs.h"

#define MAXBUF 1400
#define DEBUG_FLAG 1
#define xstr(a) str(a)
#define str(a) #a

void sendToServer(int socketNum);
void checkArgs(int argc, char * argv[]);
uint16_t shift_to_16(u_char *data);
int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
    fd_set fdvar;
    char buffer[MAXBUF];
    uint8_t sendBuf[MAXBUF];
    uint8_t recvBuf[MAXBUF];
    int i = 0;
    char c;
    char *name = argv[1];
	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
    make1(sendBuf, name);
    send(socketNum, sendBuf, MAXBUF, 0);
    recv(socketNum, recvBuf, MAXBUF, 0);
    parsePacketClient(recvBuf, name, socketNum);
    while(1){
        //printf("you are here\n");
        FD_ZERO(&fdvar);
        FD_SET(socketNum, &fdvar);
        FD_SET(STDIN_FILENO, &fdvar);
        printf("$: ");
        fflush(stdout);
        select(socketNum+1, (fd_set *) &fdvar, NULL, NULL, NULL);
        if(FD_ISSET(STDIN_FILENO, &fdvar)){
            i = 0;
            memset(sendBuf, '\0', MAXBUF);
            memset(buffer, '\0', MAXBUF);
            c = getc(stdin);
            while(c != '\n'){
                buffer[i] = c;
                c = getc(stdin);
                i++;
            }
            makePacketClient(name, sendBuf, buffer);
            //printf("sending!\n");
            send(socketNum, sendBuf, MAXBUF, 0);
        }
        else{
            memset(recvBuf, '\0', MAXBUF);
            recv(socketNum, recvBuf, MAXBUF, 0);
            parsePacketClient(recvBuf, name, socketNum);
        }
    }    
	
    close(socketNum);
	
	return 0;
}

/*
void sendToServer(int socketNum, sendBuf);
{
	char sendBuf[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent
			
	printf("Enter the data to send: ");
	scanf("%" xstr(MAXBUF) "[^\n]%*[^\n]", sendBuf);
	
	sendLen = strlen(sendBuf) + 1;
	printf("read: %s len: %d\n", sendBuf, sendLen);
		
	sent =  send(socketNum, sendBuf, sendLen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("String sent: %s \n", sendBuf);
	printf("Amount of data sent is: %d\n", sent);
}
*/
void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
