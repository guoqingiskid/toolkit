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
#define  MAX_LINE_SIZE 5
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

    /** select tcp server */
    fd_set my_fds,now_fds;
    FD_ZERO(&my_fds);
    FD_ZERO(&now_fds);

    /**listen socket */
    int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenfd < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    socklen_t  client_addr_len = sizeof(client_addr);
    bzero(&server_addr,server_addr_len);
    bzero(&client_addr,client_addr_len);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10005);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    if(bind(listenfd, (struct sockaddr*)&server_addr,server_addr_len) < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    if(listen(listenfd, 10) < 0)
    {
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    FD_SET(listenfd, &my_fds);
    int maxfd = listenfd + 1;
    while(1)
    {
        now_fds = my_fds;
        int res = select(maxfd, &now_fds, 0, 0, 0);
        if(res < 0)
        {
            std::cerr << strerror(errno) << std::endl;
            return 1;
        }
        else if(FD_ISSET(listenfd, &now_fds))
        {
            /** new client */
            int clientfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
            if(clientfd < 0)
            {
                std::cerr << strerror(errno) << std::endl;
                continue;
            }
            recv_sock = clientfd;
            /** set NOBLOCK */
            int fd_flags = fcntl(clientfd, F_GETFL,0);
            fd_flags |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fd_flags);
            fcntl(clientfd, F_SETOWN, getpid());


            FD_SET(clientfd, &my_fds);
            maxfd = maxfd > clientfd ? maxfd: clientfd + 1;
            std::cout << "now, a new client access this server...(" << inet_ntoa(client_addr.sin_addr) << ")" << std::endl;
        }
        else
        {
            /** client fd read data */
            for(int idx = 0; idx < maxfd; ++idx)
            {
                if(FD_ISSET(idx, &now_fds))
                {
                    char buff[64] = {0};
                    int read_size = 0;
                    while(read_size < sizeof(buff))
                    {
                        int size = read(idx, buff, sizeof(buff));
                        if(size < 0)
                        {
                            break;
                        }
                        else if(size == 0)
                        {
                            FD_CLR(idx, &my_fds);
                            close(idx);
                            std::cout << "client exit...\n";
                            break;
                        }
                        read_size += size ;
                    }

                    std::cout << "client: " << buff << std::endl;
                }
            }

        }


    }



    return 0;

}