#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

// Client nhập từ bàn phím 

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

    char buf[256];
    char NamePC[20];
    printf("Name: ");
    scanf("%s", NamePC);

    strcpy(buf, NamePC);
    int pos = strlen(NamePC);
    buf[pos] = 0;
    pos ++;

    int n;
    printf("so luong o dia: ");
    scanf("%d", &n);

    getchar();

    char name;
    short int size;
    for(int i = 0; i < n; i++) {
        printf("Ten o dia: ");
        scanf("%c", &name);

        printf("Kich thuoc o dia: ");
        scanf("%hd", &size);

        getchar();

        buf[pos] = name;
        pos++;
        memcpy(buf + pos, &size, sizeof(size));
        pos += sizeof(size);
    }

    printf("%d\n", pos);
    send(client, buf, pos, 0);
    printf("Sent to Port 9000\n");

    close(client);
}