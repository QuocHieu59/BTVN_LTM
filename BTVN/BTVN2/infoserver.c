#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc)
{ 

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }
    
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Client IP: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    char bufget[256];
    int retget = recv(client, bufget, sizeof(bufget), 0);
    if (retget <= 0) {printf("failed recv\n");}
    else { 
        printf("%d bytes received: \n", retget);
        
        // boc tach data:
        char NamePC[20];
        strcpy(NamePC, bufget);
        printf("+ Ten May Tinh: %s\n", NamePC);

        int pos = strlen(NamePC) + 1;
        int n = (retget - pos)/3;
        
        printf("+ So o dia: %d\n", n);
        for(int i = 0; i < n; i++) {
            char name;
            name = bufget[pos];
            pos++;

            short int size;
            memcpy(&size, bufget + pos, sizeof(size));
            pos += sizeof(size);
            printf("     %c - %hdGB\n", name, size);
        } 
    }

    close(client);
    close(listener);
   }