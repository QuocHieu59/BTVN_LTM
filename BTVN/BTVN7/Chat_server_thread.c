#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>

struct myStructure
{
    int Num;
    char ID[5];
    char Name[11];
};

char STC[] = "ID: NAME?\n";
char fail[] = "fail! send again.\n";
int client[64];
int num_clientnumber = 0;
struct myStructure client_array[5];

pthread_mutex_t num_clientnumber_m = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *);

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
    while (1)
    {
        char ID1[4];
        char Name1[10];
        char test[5];
        send(clients, STC, sizeof(STC), 0);
        int ret2 = recv(clients, bufrecv, sizeof(bufrecv), 0);
        bufrecv[ret2] = 0;
        int check = sscanf(bufrecv, "%s %s %s", ID1, Name1, test);
        if (check == 2)
        {
            pthread_mutex_lock(&num_clientnumber_m);
            printf("%s\n", bufrecv);
            client_array[num_clientnumber].Num = clients;
            strcpy(client_array[num_clientnumber].Name, Name1);
            strcpy(client_array[num_clientnumber].ID, ID1);
            client[num_clientnumber] = clients;
            num_clientnumber++;
            pthread_mutex_unlock(&num_clientnumber_m);
            printf("%d\n", num_clientnumber);
            break;
        }
        else
        {
            send(clients, fail, sizeof(fail), 0);
        }
    }
    int a = num_clientnumber - 1;
    while (1)
    {   
        char buf[128];
        int ret1 = recv(clients, buf, sizeof(buf), 0);
        if (ret1 <= 0)
        {
            continue;
        }
        buf[ret1] = 0;
        printf("Du lieu nhan tu %d: %s\n", clients, buf);
        if(strncmp(buf, "exit", 4) == 0) break;
        strcat(client_array[a].ID, buf);
        for (int j = 0; j < num_clientnumber; j++)
        {
            if (j == client[a] - 4)
                continue;
            send(client[j], client_array[a].ID, strlen(client_array[a].ID), 0);
            printf("sent to %d\n", client[j]);
        }
    }
}
