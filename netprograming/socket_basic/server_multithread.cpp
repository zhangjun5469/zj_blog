#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} ClientInfo;

void* client_thread(void* arg) {
    ClientInfo* client_info = (ClientInfo*)arg;

    char buffer[BUFFER_SIZE] = {0};
    char client_ip[INET_ADDRSTRLEN] = {0};
    int client_port = ntohs(client_info->client_addr.sin_port);

    // 使用inet_ntop转换IP地址
    if (inet_ntop(AF_INET, &client_info->client_addr.sin_addr, client_ip, sizeof(client_ip)) == NULL) {
        perror("inet_ntop failed");
    } else {
        // 打印客户端的IP地址和端口
        printf("Connection from %s:%d\n", client_ip, client_port);
    }

    // 处理接收和发送数据
    while (true) {
        int read_count = read(client_info->client_fd, buffer, BUFFER_SIZE);
        if (read_count < 0) {
            perror("read failed");
            break;
        } else if (read_count == 0) {
            printf("Client %s:%d disconnected.\n", client_ip, client_port);
            break;
        }
        // 回显数据到客户端
        if (write(client_info->client_fd, buffer, read_count) < 0) {
            perror("write failed");
            break;
        }
    }

    // 关闭客户端文件描述符
    close(client_info->client_fd);
    free(client_info); // 释放ClientInfo结构内存
    return nullptr;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    pthread_t thread_id;

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置选项，允许地址重用
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    //     perror("setsockopt failed");
    //     exit(EXIT_FAILURE);
    // }

    // 绑定套接字
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(7788);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // 接受客户端连接并创建线程
    while (true) {
        socklen_t client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // 为每个客户端创建ClientInfo结构
        ClientInfo* client_info = (ClientInfo*)malloc(sizeof(ClientInfo));
        client_info->client_fd = client_fd;
        memcpy(&client_info->client_addr, &client_addr, sizeof(client_addr));

        // 创建线程处理客户端
        if (pthread_create(&thread_id, NULL, client_thread, (void*)client_info) != 0) {
            perror("pthread_create failed");
            close(client_fd);
            free(client_info);
            exit(EXIT_FAILURE);
        }
    }

    // 主函数不会执行到这里，因为上面的循环是无限的
    close(server_fd);
    return 0;
}