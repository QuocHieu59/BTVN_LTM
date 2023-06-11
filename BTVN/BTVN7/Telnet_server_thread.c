#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

void *client_thread(void *);

char STC[] = "Name and Pass?\n";
char fail[] = "fail Name or Pass! Pls send again.\n";
char acc[] = "Logged in successfully.\n";
char buf1[128];

int main()
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

    // printf("buffer: %s\n", buf);

    while (1)
    {

        int clients = accept(listener, NULL, NULL);
        if (clients == -1)
        {
            perror("accept() failed");
            continue;
            ;
        }
        printf("Ket noi moi: %d\n", clients);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_thread, &clients);
        pthread_detach(thread_id);
    }

    close(listener);

    return 0;
}

void *client_thread(void *param)
{
    int clients = *(int *)param;
    char bufrecv[30];
    FILE *f = fopen("user.txt", "rb");
    char buf[256];
    fread(buf, 1, sizeof(buf), f);
    while (1)
    {
        char ID1[10];
        char Name1[10];
        char test[5];
        send(clients, STC, sizeof(STC), 0);
        int ret2 = recv(clients, bufrecv, sizeof(bufrecv), 0);
        bufrecv[ret2] = 0;
        int check = sscanf(bufrecv, "%s %s %s", ID1, Name1, test);
        printf("bufrecv: %s\n", bufrecv);
        if (strstr(buf, bufrecv) != NULL && check == 2)
        {
            send(clients, acc, sizeof(acc), 0);
            break;
        }
        else
        {
            send(clients, fail, sizeof(fail), 0);
        }
    }
    while (1)
    {

        int ret1 = recv(clients, buf1, sizeof(buf1), 0);
        if (ret1 <= 0)
        {
            break;
        }
        char tmp[32];
        buf1[ret1 - 1] = 0;
        printf("Du lieu nhan tu %d: %s\n", clients, buf1);
        if (strncmp(buf1, "exit", 4) == 0)
            break;
        sprintf(tmp, "%s > out.txt", buf1);
        system(tmp);
        printf("%s\n", tmp);
        FILE *fs = fopen("out.txt", "rb");
        char bufsend[256];
        int ret;
        while (!feof(fs))
        {
            ret = fread(bufsend, 1, sizeof(bufsend), fs);
            if (ret <= 0)
                break;
            send(clients, bufsend, ret, 0);
        }
        fclose(fs);
    }
    
    fclose(f);
    close(clients);
}
