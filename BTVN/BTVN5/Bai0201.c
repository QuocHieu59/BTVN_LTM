#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <ctype.h>

void removeInvalidCharacters(char *str) {
    int i, j;
    int len = strlen(str);

    // Xóa ký tự dấu cách ở đầu xâu
    while (len > 0 && str[0] == ' ') {
        memmove(str, str + 1, len);
        len--;
    }

    // Xóa ký tự dấu cách ở cuối xâu
    while (len > 0 && str[len - 1] == ' ') {
        str[len - 1] = '\0';
        len--;
    }

    // Viết hoa chữ cái đầu của từ và viết thường các ký tự còn lại
    int capitalize = 1;
    for (i = 0; i < len; i++) {
        if (str[i] == ' ') {
            capitalize = 1;
        } else {
            if (capitalize) {
                str[i] = toupper(str[i]);
                capitalize = 0;
            } else {
                str[i] = tolower(str[i]);
            }
        }
    }

    // Loại bỏ các dấu cách không hợp lệ (giữa các từ chỉ có 1 dấu cách)
    for (i = j = 0; i < len; i++) {
        if (!(str[i] == ' ' && str[i + 1] == ' ')) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
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

    fd_set fdread;
   
    int client[64];
    int num_clientnumber = 0;
    char *msg = "Bye";
    while (1)
    {
        FD_ZERO(&fdread);

        FD_SET(listener, &fdread);

        int max = listener + 1;

        for (int i = 0; i < num_clientnumber; i++)
        {
            FD_SET(client[i], &fdread);
            if(max < client[i] + 1) max = client[i] + 1;
        }
 
        int ret = select(max, &fdread, NULL, NULL, NULL);
        if (ret < 0)
        {
            perror("select() failed");
            return 1;
        }
        
        if (FD_ISSET(listener, &fdread))
        {   
            int clients = accept(listener, NULL, NULL);
            printf("Ket noi moi: %d\n", clients);
            client[num_clientnumber++] = clients;       
            char bufrecv[64];   
            sprintf(bufrecv, "Xin chao. Hien dang co %d nguoi ket noi\n", num_clientnumber);
            send(clients, bufrecv, strlen(bufrecv), 0);
        }

        for (int i = 0; i < num_clientnumber; i++)
        {
            if(FD_ISSET(client[i], &fdread)) {
                char buf[128];
                int ret1 = recv(client[i], buf, sizeof(buf), 0);
                 if (ret1 <= 0)
                {
                    continue;
                }
                buf[ret1] = 0;
                printf("Du lieu nhan tu %d: %s\n", client[i], buf);
                if(strncmp(buf, "exit", 4) == 0) {
                    send(client[i], msg, strlen(msg), 0);
                    num_clientnumber--;
                    close(client[i]);
                    continue;
                }
                removeInvalidCharacters(buf);
                printf("Du lieu sau khi chinh sua: %s\n", buf);
                send(client[i], buf, strlen(buf), 0);
            }
        }
        
    }
    

    close(listener);    

    return 0;
}
