
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>


int main()  
{   
    
    
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    FILE *f = fopen("string.txt", "rb");    
    char buf[20];
    int ret1;
    
    while (!feof(f))
    {
        ret1 = fread(buf, 1, sizeof(buf), f);
        if (ret1 <= 0) {
            printf("failed\n");
            break;

        }
        send(client, buf, ret1, 0);
    }
    printf("sent to server\n");
    fclose(f);
    close(client);
}
