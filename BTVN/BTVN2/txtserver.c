#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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
    
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Client IP: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    char bufadd[18];
    char s[] = "0123456789";
    int len = strlen(s) ;
    int count = 0;
    while (1)
    {
        char buf[21] = {'\0'};
        int ret = recv(client, buf, sizeof(buf), 0);
      
    //   printf("%s\n", buf);
         strncat(bufadd, buf, 9);
    //   printf("%s\n", bufadd);

        if(ret <=0)  break;
        int n = 0;
        int m = 0;

         while(bufadd[n] != '\0') {
 
            if(bufadd[n] == s[m]) {    

            while(bufadd[n] == s[m]  && bufadd[n] !='\0') {
                n++;
                m++;
            if(m == len) {
                count++;
            //    printf("%d\n", count);
                break;
                }
            }
            m=0;

            } else {   
                n++;
            }
          
        }

        n = 0;
        m = 0;
        while(buf[n] != '\0') {
 
            if(buf[n] == s[m]) {    

            while(buf[n] == s[m]  && buf[n] !='\0') {
                n++;
                m++;
            if(m == len) {
                count++;
            //  printf("%d\n", count);
                break;
                }
            }
            m=0;

            } else {   
                n++;
            }
          
        }

        for (int i = 9; i > 0; i--)
        {
            bufadd[9 - i] = buf[21 - i];
        }
        for (int i = 9; i < 18; i++)
        {
            bufadd[i] = '\0';
        }
       
    }

    printf("%d\n", count);
    
    close(client);
    close(listener);
}