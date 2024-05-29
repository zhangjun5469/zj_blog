#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 1000
#define PORT 7788
#define NUM_THREADS 4

typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} ClientInfo;

typedef struct {
    int epoll_fd;
    int thread_id;
} thread_arg_t;

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

void* worker_thread(void* arg) {
    thread_arg_t *targ = (thread_arg_t*)arg;
    int epoll_fd = targ->epoll_fd;
    int thread_id = targ->thread_id;
    struct epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    printf("Worker thread %d started\n", thread_id);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; ++i) {
            int client_fd = events[i].data.fd;
            if (events[i].events & EPOLLIN) {
                ClientInfo* client_info = (ClientInfo*)events[i].data.ptr;
                char client_ip[INET_ADDRSTRLEN] = {0};
                int client_port = ntohs(client_info->client_addr.sin_port);

                if (inet_ntop(AF_INET, &client_info->client_addr.sin_addr, client_ip, sizeof(client_ip)) == NULL) {
                    perror("inet_ntop failed");
                } else {
                    printf("Thread %d: Received Message from %s:%d\n", thread_id, client_ip, client_port);
                }

                int read_count = read(client_fd, buffer, BUFFER_SIZE);
                if (read_count < 0) {
                    perror("read failed");
                    close(client_fd);
                } else if (read_count == 0) {
                    printf("Client %s:%d disconnected.\n", client_ip, client_port);
                    close(client_fd);
                } else {
                    if (write(client_fd, buffer, read_count) < 0) {
                        perror("write failed");
                        close(client_fd);
                    }
                }
            }
        }
    }
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // 创建并初始化线程池
    pthread_t threads[NUM_THREADS];
    thread_arg_t targs[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        targs[i].epoll_fd = epoll_create1(0);
        if (targs[i].epoll_fd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
        targs[i].thread_id = i;
        pthread_create(&threads[i], NULL, worker_thread, &targs[i]);
    }

    struct epoll_event ev;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        set_nonblocking(client_fd);

        // 为每个客户端创建ClientInfo结构
        ClientInfo* client_info = (ClientInfo*)malloc(sizeof(ClientInfo));
        client_info->client_fd = client_fd;
        memcpy(&client_info->client_addr, &client_addr, sizeof(client_addr));

        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = client_info;

        // Assign the new connection to a worker thread
        int worker_index = client_fd % NUM_THREADS;
        if (epoll_ctl(targs[worker_index].epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
            perror("epoll_ctl: client_fd");
            close(client_fd);
            free(client_info);
        }
    }

    close(server_fd);
    return 0;
}
