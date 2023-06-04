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

int main() 
{
    // Tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener != -1)
        printf("Socket created: %d\n", listener);
    else
    {
        printf("Failed to create socket.\n");
        exit(1);
    }

    // Khai bao cau truc dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    // Gan dia chi voi socket
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        printf("bind() failed.\n");
        exit(1);
    }

    if (listen(listener, 5)) 
    {
        printf("listen() failed.\n");
        exit(1);
    }

    int num_processes = 3;
    for (int i = 0; i < num_processes; i++)
        if (fork() == 0) {
            printf("Waiting for a new client ...\n");
            // Chap nhan ket noi
            int client = accept(listener, NULL, NULL);
            if (client == -1)
            {
                printf("accept() failed.\n");
                exit(1);
            }
            printf("New client accepted in process %d: %d\n", client, getpid());

            // Nhan du lieu tu trinh duyet
            char buf[128];
            char *request = NULL;
            int size = 0;
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    printf("recv() failed.\n");
                    break;
                }
            
                request = realloc(request, size + ret);
                memcpy(request + size, buf, ret);
                size += ret;
                
                if (strstr(request, "\r\n\r\n") != NULL)
                    break;
            }
            
            printf("%s\n", request);
            free(request);

            // Gui du lieu sang trinh duyet
            char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</h1></body></html>";
            send(client, msg, strlen(msg), 0);
            close(client);
        }


    // Dong ket noi
    getchar();
    close(listener);    
    killpg(0, SIGKILL);    

    return 0;
}
