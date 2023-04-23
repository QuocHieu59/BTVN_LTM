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
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    char port[4];
    strcpy(port, argv[1]);
    printf("PORT: %s\n", port);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(port));

    bind(receiver, (struct sockaddr *)&addr, sizeof(addr));

    char namefile[11];
    struct sockaddr_in sender_addr;
    int sender_addr_len = sizeof(sender_addr);

    
    int ret1 = recvfrom(receiver, namefile, sizeof(namefile), 0,
        (struct sockaddr *)&sender_addr, &sender_addr_len);
    printf("%d bytes received %s: %s\n", ret1, 
        inet_ntoa(sender_addr.sin_addr), namefile);
    
    char buf[256];
    sprintf(buf,"new_%s" ,namefile);
    FILE *f = fopen(buf, "wb");
    int ret = recvfrom(receiver, buf, sizeof(buf), 0,
        (struct sockaddr *)&sender_addr, &sender_addr_len);
    fwrite(buf, 1, ret, f);
    printf("%d byte recv \n", ret);
    printf("%s\n", buf);
    
    fclose(f);
}
