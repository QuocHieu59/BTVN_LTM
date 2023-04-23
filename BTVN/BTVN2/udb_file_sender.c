#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{   
    char ip[9];
    strcpy(ip, argv[1]);
    int port = atoi(argv[2]);
    printf("IP: %s\n", ip);
    printf("PORT: %d\n", port);

    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int ret1 = sendto(sender, argv[3], strlen(argv[3]), 0,
        (struct sockaddr *)&addr, sizeof(addr));
    printf("%d bytes sent.\n", ret1);
    
    char filename[20];
    strcpy(filename, argv[3]);
    FILE *f = fopen(filename, "rb");

    char buf[256];
    int ret;
    while (!feof(f))
    {
        ret = fread(buf, 1, sizeof(buf), f);
        
        if (ret <= 0)
            break;
      int ret2 = sendto(sender, buf, strlen(buf), 0,
        (struct sockaddr *)&addr, sizeof(addr));
        printf("%d bytes sent from %s\n", ret2, filename);
    }
    
    fclose(f);
}