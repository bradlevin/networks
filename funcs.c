/*Common functions between files*/
#include "funcs.h"
#define MAXBUF 1400


uint32_t shift_to_32(uint8_t *data){
    uint32_t num1 = data[0];
    uint32_t num2 = data[1];
    uint32_t num3 = data[2];
    num1 <<= 24;
    num2 <<= 16;
    num3 <<= 8;
    num1 |= num2;
    num1 |= num3;
    num1 |= data[3];
    return num1;
}

uint16_t shift_to_16(u_char *data){
    uint16_t num = data[0];
    num <<= 8;
    num |= data[1];
    return num;
}



void makePacketClient(char *sendHandle, uint8_t *buffer, char *line){
    int numHandles;
    char *tag;
    char *handle;
    char *word;
    int lenTotal = 0;
    char message[MAXBUF];
    char sendMessage[200];
    int i;
    int spaces = 0;
    const char delim[2] = " ";
    Handle handles[10];
    char handleSize[10];
    memset(message, '\0', MAXBUF);
    tag = strtok(line, delim);
    //printf("Tag %s\n", tag);
    if(!strcmp(tag, "%M") || !strcmp(tag, "%m")){
        handle = strtok(NULL, delim);
        //printf("Handle: %s\n", handle);
        if(isdigit(handle[0])){
            numHandles = handle[0] - '0';
            //printf("numHandles: %c\n", numHandles);
            for(i = 0; i < numHandles; i++){
                handle = strtok(NULL, delim);
                //printf("Handle: %s\n", handle);
                handleSize[i] = strlen(handle);
                memcpy(handles[i].handle, handle, handleSize[i]);
            }
        }
        else{
            numHandles = 1;
            handleSize[0] = strlen(handle);
            memcpy(handles[0].handle, handle, handleSize[0]);

        }
        i = 0;
        word = strtok(NULL, delim);
        while(word != NULL){
            //printf("Word: %s\n", word);
            memcpy(&(message[lenTotal]), word, strlen(word));
            lenTotal += strlen(word);
            message[lenTotal] = ' ';
            lenTotal ++;
            word = strtok(NULL, delim);
        }
        while(lenTotal > 200){
            memset(sendMessage, '\0', MAXBUF);
            memcpy(sendMessage, &(message[spaces]), lenTotal);
            lenTotal -= 200;
            spaces += 200;
            make5(buffer, sendHandle, numHandles, handleSize, handles, sendMessage);
        }
        memcpy(sendMessage, &(message[spaces]), lenTotal);
        make5(buffer, sendHandle, numHandles, handleSize, handles, sendMessage);
        
    }
    else if(!strcmp(tag, "%B") || !strcmp(tag, "%b")){
        word = strtok(NULL, delim);
        lenTotal = 0;
        while(word != NULL){
            memcpy(&(message[lenTotal]), word, strlen(word));
            lenTotal += strlen(word);
            message[lenTotal] = ' ';
            lenTotal ++;
            word = strtok(NULL, delim);
        }

        memcpy(sendMessage, message, lenTotal);
        make4(buffer, sendHandle, sendMessage);
    }
    else if(!strcmp(tag, "%L") || !strcmp(tag, "%l")){
        make10(buffer);
    }
    else if(!strcmp(tag, "%E") || !strcmp(tag, "%e")){
        make8(buffer);
    }
    
}
void parsePacketClient(uint8_t *buffer, char *handle, int socketNum){
    uint8_t flag = buffer[2];
    if(flag == 3){
        parse3(buffer, handle);
    }
    else if(flag == 4){
        parse4Client(buffer);
    }
    else if(flag == 5){
        parse5Client(buffer);
    }
    else if(flag == 7){
        parse7(buffer);
    }
    else if(flag == 9){
        parse9(buffer, socketNum);
    }
    else if(flag == 11){
        parse11(buffer);
    }
    else if(flag == 12){
        parse12(buffer);
    }

}

void parsePacketServer(uint8_t *buffer, Handle *clientList, int tableSize, int socketNum){
    uint8_t flag = buffer[2];
    printf("Flag: %d\n", flag);
    if(flag == 1){
        parse1(buffer, clientList, tableSize, socketNum);
    }
    if(flag == 4){
        parse4Server(buffer, clientList, tableSize, socketNum);
    }
    if(flag == 5){
        parse5Server(buffer, clientList, tableSize);
    }
    if(flag == 8){
        parse8(buffer, clientList, socketNum);
    }
    
    else if(flag == 10){
        parse10(buffer, clientList, tableSize, socketNum);
    }

}

void parse4Server(uint8_t *buffer, Handle *handles, int tableSize, int socketNum){
    int i;
    for(i = 0; i < tableSize+1; i++){
        if(handles[i].handle[0] != '\0' && i != socketNum){
            printf("Sending!\n");
            send(i, buffer, MAXBUF, 0);
        }
    }
}

void parse4Client(uint8_t *buffer){
    printf("Parsing!\n");
    char messBuf[200];
    char handleBuf[100];
    int handleLen;
    handleLen = buffer[3];
    printf("handleLen: %d\n", handleLen);
    memcpy(handleBuf, &(buffer[4]), handleLen);
    memcpy(messBuf, &(buffer[handleLen + 4]), 200);
    printf("%s: %s\n", handleBuf, messBuf);
}

void parse1(uint8_t *buffer, Handle *clients, int tableSize, int socketNo){
    //printf("At parse1()\n");
    int i;
    char handle[100];
    int len = buffer[3];
    memcpy(handle, &(buffer[4]), len);
    for(i = 0; i < tableSize; i++){
        if(clients[i].handle[0] != '\0'){
            if(!strcmp(handle, clients[i].handle)){
                make3(buffer);
                send(socketNo, buffer, MAXBUF, 0);
            }
        }
        if(i == tableSize-1){
            make2(buffer);
            send(socketNo, buffer, MAXBUF, 0);
            //printf("socket number: %d\n", socketNo);
            memcpy(clients[socketNo].handle, handle, len);
            //printf("handle: %s\n", clients[socketNo].handle);
        }
    }
    //printf("Done with parse1()\n");
}

void parse3(uint8_t *buffer, char *handle){
    printf("Handle already in use: %s\n", handle);
    exit(0);
}

void parse5Client(uint8_t *buffer){
    char sender[100];
    char message[200];
    int numDest = 0;
    int len = 0;
    int bytes = 0;
    int i;
    bytes += 3;
    memset(message, '\0', 200);
    len = buffer[bytes];
    //printf("len: %d\n", len);
    bytes++;
    memcpy(sender, &(buffer[bytes]), len);
    //printf("Sender: %s\n", sender);
    bytes += len;
    numDest = buffer[bytes];
    bytes++;
    for(i = 0; i < numDest; i++){
        len = buffer[bytes];
        bytes += len + 1;
    }
    memcpy(message, &(buffer[bytes]), 200);
    printf("\n%s: %s\n", sender, message);
}

void parse5Server(uint8_t *buffer, Handle *clients, int tableSize){
    int len = 0;
    int bytes = 0;
    int dests = 0;
    char sendHandle[100];
    char handle[100];
    uint8_t errorBuf[MAXBUF];
    int i;
    int j;
    int sent = 0;
    bytes += 3;
    len = buffer[bytes];
    bytes ++;
    //printf("len: %d\n", len);
    memcpy(sendHandle, &(buffer[bytes]), len);
    bytes += len;
    dests = buffer[bytes];
    bytes++;
    //printf("dests: %d\n", dests);
    for(i = 0; i < dests; i++){
        memset(handle, '\0', 100);
        len = buffer[bytes];
        bytes ++;
        memcpy(handle, &(buffer[bytes]), len);
        //printf("Handle: %s\n", handle);
        for(j = 0; j < tableSize+1; j++){
            //printf("clients[j].handle: %s\n", clients[j].handle);
            if(!strcmp(clients[j].handle, handle)){
                send(j, buffer, MAXBUF, 0);
                sent = 1;
                //printf("sending from parse5server!\n");
            }
        }
        if(sent == 0){
            make7(errorBuf, handle);
            for(j = 0; j < tableSize+1; j++){
                if(!strcmp(clients[j].handle, sendHandle)){
                    send(j, errorBuf, MAXBUF, 0);
                    sent = 0;
                }
            }
        }
        bytes += len;
    }
}

void parse7(uint8_t *buffer){
    char handle[100];
    memcpy(handle, &(buffer[4]), buffer[3]);
    printf("\nClient with handle %s does not exist.\n", handle);
}

void parse8(uint8_t *buffer, Handle *clients, int socketNum){
    memset(clients[socketNum].handle, '\0', 100);
    make9(buffer);
    send(socketNum, buffer, MAXBUF, 0);
}

void parse9(uint8_t *buffer, int clientSocket){
    close(clientSocket);
    exit(0);
}

void parse10(uint8_t *buffer, Handle *handles, int tableSize, int socketNum){
    int i;
    uint32_t numHandles = 0;
    memset(buffer, '\0', MAXBUF);
    for(i = 0; i < tableSize+1; i++){
        if(handles[i].handle[0] != '\0'){
            printf("in if statement!\n");
            printf("handles[i].handle: %s\n", handles[i].handle);
            numHandles++;
        }
    }
    make11(buffer, numHandles);
    send(socketNum, buffer, MAXBUF, 0);
    for(i = 0; i < tableSize+1; i++){
        if(handles[i].handle[0] != '\0'){
            make12(buffer, handles[i].handle);
            send(socketNum, buffer, MAXBUF, 0);
        }
    }
    make13(buffer);
    send(socketNum, buffer, MAXBUF, 0);
}

void parse11(uint8_t *buffer){
    uint8_t num[4];
    int i; 
    for(i = 0; i < 4; i++){
        printf("Buf: %i\n", buffer[i+3]);
    }
    memcpy(num, &(buffer[3]), 4);
    uint32_t handles = shift_to_32(num);
    //handles = ntohl(handles);
    printf("Number of clients: %u\n", handles);
}

void parse12(uint8_t *buffer){
    int len = buffer[3];
    uint8_t handle[len];
    memcpy(handle, &(buffer[4]), len);
    printf("    %s\n", handle);
}

void make1(uint8_t *buffer, char *handle){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 4 + strlen(handle);
    PDU = htons(PDU);
    memcpy(buffer,&(PDU), 2);
    buffer[2] = 1;
    buffer[3] = strlen(handle);
    memcpy(&(buffer[4]), handle, strlen(handle));
    //printf("Handle after memcpy: %s\n", handle);
}

void make2(uint8_t *buffer){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 3;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 2;
}


void make3(uint8_t *buffer){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 3;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 3;
}

void make4(uint8_t *buffer, char *handle, char *message){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 4 + strlen(handle) + strlen(message);
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 4;
    buffer[3] = strlen(handle);
    memcpy(&(buffer[4]), handle, strlen(handle));
    memcpy(&(buffer[4+strlen(handle)]), message, strlen(message));
}

void make5(uint8_t *buffer, char *sendHandle, int destNum,  char *destHandleLens, Handle *destHandles, char *message){
    memset(buffer, '\0', MAXBUF);
    int i;
    int bytes = 0;
    int sendLen;
    int messageLen;
    int extraLen = 5;
    //printf("Message: %s\n", message);
    messageLen = strlen(message);
    //printf("Message len: %d\n", messageLen);
    sendLen = strlen(sendHandle);
    uint16_t PDU = extraLen + sendLen + destNum + messageLen;
    for(i = 0; i < destNum; i++){
        PDU += destHandleLens[i];
    }
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    bytes += 2;
    buffer[bytes] = 5;
    bytes += 1;
    buffer[bytes] = sendLen;
    bytes += 1;
    memcpy(&(buffer[bytes]), sendHandle, sendLen);
    bytes += sendLen;
    buffer[bytes] = destNum;
    bytes += 1;
    for(i = 0; i < destNum; i++){
        buffer[bytes] = destHandleLens[i];
        //printf("buffer dest len %d\n", buffer[bytes]);
        bytes ++;
        //printf("destNum: %d\n", destNum);
        //printf("destHandleLen: %d\n", destHandleLens[i]);
        //printf("handle: %s\n", destHandles[i].handle);
        memcpy(&(buffer[bytes]), &(destHandles[i].handle), destHandleLens[i]);
        bytes += destHandleLens[i];
    }
    /*
    buffer[bytes] = destLen1;
    bytes += 1;
    memcpy(&(buffer[bytes]), destHandle1, destLen1);
    bytes += destLen1;
    if(destLen2 != 0){
        buffer[bytes] = destLen2;
        bytes += 1;
        memcpy(&(buffer[bytes]), destHandle2, destLen2);
        bytes += destLen2;
        if(destLen3 != 0){
            buffer[bytes] = destLen3;
            bytes += 1;
            memcpy(&(buffer[bytes]), destHandle3, destLen3);
            bytes += destLen3;
        }
    } 
    */
    buffer[bytes] = messageLen;
    bytes ++;
    memcpy(&(buffer[bytes]), message, messageLen);
}

void make7(uint8_t *buffer, char *handle){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = strlen(handle) + 4;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 7;
    buffer[3] = strlen(handle);
    memcpy(&(buffer[4]), handle, strlen(handle));
}


void make8(uint8_t *buffer){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 3;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 8;
}

void make9(uint8_t *buffer){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 3;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 9;
}


void make10(uint8_t *buffer){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 3;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 10;
}

void make11(uint8_t *buffer, uint32_t handles){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 7;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 11;
    handles = htonl(handles);
    memcpy(&(buffer[3]), &(handles), 4); 
}

void make12(uint8_t *buffer, char *handle){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = strlen(handle) + 4;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 12;
    buffer[3] = strlen(handle);
    memcpy(&(buffer[4]), handle, strlen(handle));
}

void make13(uint8_t *buffer){
    memset(buffer, '\0', MAXBUF);
    uint16_t PDU = 3;
    PDU = htons(PDU);
    memcpy(buffer, &(PDU), 2);
    buffer[2] = 13;
}
