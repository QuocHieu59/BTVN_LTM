#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

void signalHandler(int signo)
{   
    int stat;
    int pid = wait(&stat);
    printf("Chid %d terminated.\n", pid);
}

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

    char STC[] = "Name and Pass?\n";
    char fail[] = "fail Name or Pass! Pls send again.\n";
    char acc[] = "Logged in successfully.\n";
    char buf1[128];
    FILE *f = fopen("user.txt", "rb");
    char buf[256];
    fread(buf, 1, sizeof(buf), f);
    signal(SIGCHLD, signalHandler);
    while (1)
    {   
        printf("Waiting for new client...\n");
        int clients = accept(listener, NULL, NULL);
        char bufrecv[30];

        if (fork() == 0) 
        {
            close(listener);
            printf("Ket noi moi trong process %d\n", getpid());

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
                if (strncmp(buf1, "exit", 4) == 0) break;
                sprintf(tmp, "%s > out.txt", buf1);
                system(tmp);
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

            close(clients);
            exit(0);
            
        }

        close(clients);
    }

    fclose(f);
    close(listener);

    return 0;
}
