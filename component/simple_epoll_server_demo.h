#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define  MAX_LINE_SIZE 5
#define  MAX_EPOLL_SIZE 100
enum
{
    SEND_BUFFER,
    RECV_BUFFER
};
void test_type(const int &type)
{
    switch(type)
    {
        case SOCK_STREAM:
            std::cout << "socket is TCP SOCKET\n";break;
        case SOCK_DGRAM:
            std::cout << "Socket is UDP SOCKET\n";break;
        default:
            std::cout << "other socket\n";break;
    }
}

void test_buffer(const int &type, int buffer_type, const int &buffer_size)
{
    if(type == SOCK_STREAM)
    {
        if(buffer_type == SEND_BUFFER)
            std::cout << "TCP SOCKET SEND BUFFER SIZE: " << buffer_size << std::endl;
        else
            std::cout << "TCP SOCKET RECV BUFFER SIZE: " << buffer_size << std::endl;
    }
    else if(type == SOCK_DGRAM)
    {
        if(buffer_type == SEND_BUFFER)
            std::cout << "UDP SOCKET SEND BUFFER SIZE: " << buffer_size << std::endl;
        else
            std::cout << "UDP SOCKET RECV BUFFER SIZE: " << buffer_size << std::endl;
    }
}

int recv_sock ;
void handle_SIGURG(int sig)
{
    char buff[16] = {0};
    int size = recv(recv_sock, buff, sizeof(buff), MSG_OOB);
    std::cout << "OOB: " << buff << std::endl;
}


int main(int argc, char **argv)
{

    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = handle_SIGURG;
    sigaction(SIGURG, &act, 0);

    /** epoll tcp server */
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    socklen_t server_addr_len = sizeof(server_addr);
    bzero(&server_addr, server_addr_len);
    bzero(&client_addr, client_addr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10004);
    inet_pton(AF_INET, "192.168.1.8", &server_addr.sin_addr);

    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenfd < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    if(bind(listenfd, (struct sockaddr*)&server_addr, server_addr_len) < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    if(listen(listenfd, 10) < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    struct epoll_event* res_events;
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listenfd;
    res_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * MAX_EPOLL_SIZE);

    int epoll_fd = epoll_create(MAX_EPOLL_SIZE);
    if(epoll_fd < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &event);
    while(1)
    {
        uint32_t res_size = epoll_wait(epoll_fd, res_events, MAX_EPOLL_SIZE, -1);
        if(res_size < 0)
        {
            std::cerr << strerror(errno) << std::endl;
            return 1;
        }

        for(int idx = 0; idx < res_size; ++idx)
        {
            if(res_events[idx].data.fd == listenfd)
            {
                /** new client access */
                int client_fd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
                if(client_fd < 0)
                {
                    std::cerr << strerror(errno) << std::endl;
                    continue;
                }

                int flags = fcntl(client_fd, F_GETFL);
                flags |= O_NONBLOCK;
                fcntl(client_fd, F_SETFL, flags);

                event.events = EPOLLIN;
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                std::cout << "a new client access...(" << inet_ntoa(client_addr.sin_addr) << ")\n";

            }
            else
            {
                /** read data */
                char buff[64] = {0};
                int read_size = 0;
                while(read_size < sizeof(buff))
                {
                    int size = read(res_events[idx].data.fd, buff, sizeof(buff));
                    if(size == 0)
                    {
                        std::cout << "client has close!\n";
                        epoll_ctl(listenfd, EPOLL_CTL_DEL, res_events[idx].data.fd, 0);
                        close(res_events[idx].data.fd);
                        break;
                    }
                    else if(size > 0)
                    {
                        read_size += size ;
                    }
                    else
                    {
                        break;
                    }
                }

                std::cout << "client: " << buff << std::endl;
            }
        }


    }

    close(epoll_fd);
    close(listenfd);

    return 0;

}