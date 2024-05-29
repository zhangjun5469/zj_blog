#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char client_ip[INET_ADDRSTRLEN];
    int client_port;
    char buffer[BUFFER_SIZE] = {0};

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    // {
    //     perror("setsockopt failed");
    //     exit(EXIT_FAILURE);
    // }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(7788);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    if((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    if(inet_ntop(AF_INET, &address.sin_addr, client_ip, sizeof(client_ip)) == NULL)
    {
        perror("inet_ntop failed");
        exit(EXIT_FAILURE);
    }
    client_port = ntohs(address.sin_port);

    printf("Connection Established! Connected From Client IP is: %s, Port is: %d\n", client_ip, client_port);

    // 进入读写循环
    while(true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int read_count = read(client_fd, buffer, BUFFER_SIZE);
        if(read_count < 0)
        {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        else if (read_count == 0) 
        {
            printf("Client Disconnected !\n");
            break;
        }
        // 回显
        printf("Recived Message: %s, From Client IP: %s, Port: %d\n", buffer, client_ip, client_port);
        if(write(client_fd, buffer, read_count) < 0)
        {
            perror("write failed");
            exit(EXIT_FAILURE);
        }
    }

    // 关闭连接
    if(close(client_fd) < 0)
    {
        perror("close failed");
        exit(EXIT_FAILURE);
    }
    if(close(server_fd) < 0)
    {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}