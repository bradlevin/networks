#ifndef FUNCS_H
#define FUNCS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

typedef struct __attribute__((packed)) client{
    uint8_t handle[200];
    int socketNum;
} Client;

typedef struct __attribute__((packed)) node{
    Client client;
    struct node *next;
} Node;

typedef struct __attribute__((packed)) handle{
    char handle[100];
} Handle;

uint32_t shift_to_32(uint8_t *data);
uint16_t shift_to_16(u_char *data);
void makePacketClient(char *sendHandle, uint8_t *buffer, char *line);
void parsePacketServer(uint8_t *buffer, Handle *clientList, int tableSize, int socketNum);
void parsePacketClient(uint8_t *buffer, char *handle, int socketNum);

void parse1(uint8_t *buffer, Handle *clients, int tableSize, int socketNo);
void parse3(uint8_t *buffer, char *handle);
void parse4Client(uint8_t *buffer);
void parse4Server(uint8_t *buffer, Handle *handles, int tableSize, int socketNum);
void parse5Client(uint8_t *buffer);
void parse5Server(uint8_t *buffer, Handle *clients, int tableSize);
void parse7(uint8_t *buffer);
void parse8(uint8_t *buffer, Handle *clients, int socketNum);
void parse9(uint8_t *buffer, int clientSocket);
void parse10(uint8_t *buffer, Handle *handles, int tableSize, int socketNum);
void parse11(uint8_t *buffer);
void parse12(uint8_t *buffer);


void make1(uint8_t *buffer, char *handle);
void make2(uint8_t *buffer);
void make3(uint8_t *buffer);
void make4(uint8_t *buffer, char *handle, char *message);
void make5(uint8_t *buffer, char *sendHandle, int destNum, char *destHandleLens, Handle *destHandles, char *message);
void make7(uint8_t *buffer, char *handle);
void make8(uint8_t *buffer);
void make9(uint8_t *buffer);
void make10(uint8_t *buffer);
void make11(uint8_t *buffer, uint32_t handles);
void make12(uint8_t *buffer, char *handle);
void make13(uint8_t *buffer);

#endif





































