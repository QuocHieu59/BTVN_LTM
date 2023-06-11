#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

char STC[] = "GET_TIME [format]?\n";
char fail[] = "fail command! Pls send again.\n";
char fail_format[] = "fail format! Pls send again.\n";

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
    send(clients, STC, sizeof(STC), 0);
    while (1)
    {
        char ID1[10];
        char Name1[10];
        char test[5];
        int ret2 = recv(clients, bufrecv, sizeof(bufrecv), 0);
        bufrecv[ret2] = 0;
        if (strncmp(bufrecv, "exit", 4) == 0)
            break;
        int check = sscanf(bufrecv, "%s %s %s", ID1, Name1, test);
        printf("bufrecv: %s\n", bufrecv);
        if (check == 2 && strncmp(ID1, "GET_TIME", 8) == 0)
        {
            // current time
            time_t rawtime;
            struct tm *time_str;
            time(&rawtime);
            time_str = localtime(&rawtime);
            char data[50];
            if (strncmp(Name1, "dd/mm/yyyy", 10) == 0)
            {
                snprintf(data, sizeof(data), "%d/%d/%d\n",
                         time_str->tm_mday, time_str->tm_mon + 1, time_str->tm_year + 1900);
                send(clients, data, strlen(data), 0);
            }
            else if (strncmp(Name1, "mm/dd/yyyy", 10) == 0)
            {
                snprintf(data, sizeof(data), "%d/%d/%d\n",
                         time_str->tm_mon + 1, time_str->tm_mday, time_str->tm_year + 1900);
                send(clients, data, strlen(data), 0);
            }
            else if (strncmp(Name1, "dd/mm/yy", 8) == 0)
            {
                snprintf(data, sizeof(data), "%d/%d/%d\n",
                         time_str->tm_mday, time_str->tm_mon + 1, (time_str->tm_year + 1900) % 1000);
                send(clients, data, strlen(data), 0);
            }
            else if (strncmp(Name1, "mm/dd/yy", 8) == 0)
            {
                snprintf(data, sizeof(data), "%d/%d/%d\n",
                         time_str->tm_mon + 1, time_str->tm_mday, (time_str->tm_year + 1900) % 1000);
                send(clients, data, strlen(data), 0);
            }
            else
                send(clients, fail_format, sizeof(fail_format), 0);
        }
        else
        {
            send(clients, fail, sizeof(fail), 0);
        }
    }

    printf("Ket thuc luong %ld \n", pthread_self());
    close(clients);
    pthread_exit(param);
}
