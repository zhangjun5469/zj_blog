#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1" // 服务端IP地址
#define SERVER_PORT 7788       // 服务端端口号

int main() {
    int sockfd;
    ssize_t num_read, num_written;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type 'exit' to quit.\n");

    // 循环读取用户输入并发送给服务器
    while (true) {
        // 清空缓冲区
        memset(buffer, 0, BUFFER_SIZE);

        // 读取用户输入
        printf("Enter message to send to server (or 'exit' to quit): ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            if (feof(stdin)) {
                printf("\nEnd of input. Exiting...\n");
                break;
            } else {
                perror("Error reading from stdin");
                break;
            }
        }

        // 检查是否输入了退出命令
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting...\n");
            break;
        }

        // 发送数据到服务器
        num_written = write(sockfd, buffer, strlen(buffer));
        if (num_written < 0) {
            perror("write failed");
            break;
        }

        // 读取服务器的响应
        num_read = read(sockfd, buffer, BUFFER_SIZE);
        if (num_read < 0) {
            perror("read failed");
            break;
        } else if (num_read == 0) {
            printf("Server disconnected.\n");
            break;
        }

        // 打印服务器的响应
        buffer[num_read] = '\0'; // 确保字符串以空字符结尾
        printf("Server response: %s\n", buffer);
    }

    // 关闭套接字
    if (close(sockfd) < 0) {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}