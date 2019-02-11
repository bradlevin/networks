/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
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

#include "networks.h"
#include "funcs.h"

#define MAXBUF 1400
#define DEBUG_FLAG 1

void recvFromClient(int clientSocket, uint8_t *buf);
int checkArgs(int argc, char *argv[]);

/* Linked List */
/*
void addHead(Node **head, Client client){
    Node *new_node = (Node*) malloc(sizeof(Node));
    new_node->client = client;
    new_node->next = NULL;
}

void addNode(Node **head, Client client){
    Node *new_node = (Node*) malloc(sizeof(Node));
    new_node->client = client;
    new_node->next = (*head);
    (*head) = new_node;
}

void rmNode(Node **head, Client client, int size){
    Node *prev_node;
    Node *curr_node;
    int i;
    if(!strcmp((char *)((*head)->client.handle), ((char *)client.handle))){
        (*head)->next = (*head);
    }
    else{
        curr_node = (*head)->next;
        prev_node = (*head);
        for(i = 0; i < size; i++){
            if(!strcmp((char *)(curr_node->client.handle), ((char *)client.handle))){
                prev_node->next = curr_node->next;
            }
            else{
                prev_node = curr_node;
                curr_node = curr_node->next;
            }
        }
    }       
}

int isClient(Node **head, Client client, int size){
    Node *curr_node = (*head);
    int i;
    for(i = 0; i < size; i++){
        if(!strcmp((char *)(curr_node->client.handle), ((char *)client.handle))){
            return 0;
        }
        if(curr_node->next == NULL){
            return 1;
        }
        else{
            curr_node = curr_node->next;
        }
    }
    return 1;
}
*/


int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	int i;
    fd_set fdvar;
    uint8_t recvBuf[MAXBUF];
    int maxSocket;
    Handle *clients;
	portNumber = checkArgs(argc, argv);	
    serverSocket = tcpServerSetup(portNumber);
    FD_SET(serverSocket, &fdvar);
	/*
    clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
    
    if(serverSocket > clientSocket){
        maxSocket = serverSocket;
    }
    else{
        maxSocket = clientSocket;
    }
    */
    maxSocket = serverSocket;
    clients = (Handle *) malloc(maxSocket*sizeof(Handle));
    for(i = 0; i < maxSocket; i++){
        memset(clients[i].handle, '\0', 100);
    }
	// wait for client to connect
    /*
    recvFromClient(clientSocket, buf);
    parsePacketServer(buf, clients, maxSocket, clientSocket);
    printf("clientSocket: %d\n", clientSocket);
    */ 
    while(1){
        FD_ZERO(&fdvar);
        FD_SET(serverSocket, &fdvar);
        for(i = 0; i < maxSocket+1; i++){
            //printf("Handle: %s\n", clients[i].handle);       
            if(clients[i].handle[0] != '\0'){
                //printf("in if statement\n");
                //printf("Handle: %s\n", clients[i].handle);
                FD_SET(i, &fdvar);
            }
        }
        //printf("Before select!\n");
        select(maxSocket + 2, (fd_set *) &fdvar, NULL, NULL, NULL);
        //printf("After select!\n");
        if(FD_ISSET(serverSocket, &fdvar)){
            //printf("At server \n");
            memset(recvBuf, '\0', MAXBUF);
            clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
            //printf("clientSocket: %d\n", clientSocket); 
            if(clientSocket > maxSocket){
                maxSocket = clientSocket;
                //printf("maxSocket: %d\n", maxSocket);
                //printf("Handle before realloc: %s\n", clients[4].handle);
                clients = (Handle *) realloc(clients, (maxSocket+1)*sizeof(Handle));
                //printf("Handle before memset: %s\n", clients[4].handle);
                memset(clients[maxSocket].handle, '\0', 100);
                //printf("Handle before memset: %s\n", clients[4].handle);
            }
            recvFromClient(clientSocket, recvBuf);
            //printf("Flag: recvBuf %d\n", recvBuf[2]);
            parsePacketServer(recvBuf, clients, maxSocket, clientSocket);
            //printf("clientSocket: %d\n", clientSocket);
        }
        else{
            for(i = 0; i < maxSocket+1; i++){
                //printf("Fore!");
                if(clients[i].handle[0] != '\0'){
                    //printf("Handle: %s\n", clients[i].handle);
                    if(FD_ISSET(i, &fdvar)){
                        memset(recvBuf, '\0', MAXBUF);
                        //printf("recieving from %d!\n", i);
                        recv(i, recvBuf, MAXBUF, 0);
                        parsePacketServer(recvBuf, clients, maxSocket, i);
                    }
                }
            }
        }
    }
	
	return 0;
}


void recvFromClient(int clientSocket, uint8_t *buf)
{
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recv(clientSocket, buf, MAXBUF, 0)) < 0)
	{
		perror("recv call");
		exit(-1);
	}
}
/*
void sendToClient(int clientSocket){
    char sendBuf[MAXBUF];
    int sendLen = 0;
    int sent = 0;



}
*/
int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

