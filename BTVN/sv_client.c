#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Client nhập từ bàn phím 

int main(int argc, char *argv[])
{
    char ip[9];
    strcpy(ip, argv[1]);
    int port = atoi(argv[2]);
    printf("IP: %s\n", ip);
    printf("PORT: %d\n", port);

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }
// client nhap du lieu
    printf("MSSV: ");
    char mssv[10];
    fgets(mssv, 10, stdin);
    mssv[strcspn(mssv, "\n")] = '\0';  

    printf("Name: ");
    char hoTen[20];
    fgets(hoTen, 20, stdin);
    hoTen[strcspn(hoTen, "\n")] = '\0';

    printf("Date: ");
    char ngaySinh[15];
    fgets(ngaySinh, 15, stdin);
    ngaySinh[strcspn(ngaySinh, "\n")] = '\0';

    printf("Medium score: ");
    char diemTrungBinh[6];
    fgets(diemTrungBinh, 6, stdin);
    diemTrungBinh[strcspn(diemTrungBinh, "\n")] = '\0';

    // Đóng gói dữ liệu
    char data[51];
    snprintf(data, 51, "%s|%s|%s|%s", mssv, hoTen, ngaySinh, diemTrungBinh);

    send(client, data, strlen(data), 0);
    printf("Sent: %s to Post %d\n", data, port);
    close(client);

}