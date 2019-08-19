
//ping.h
#ifndef  _PING_H
#define  _PING_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#define BUFSIZE 1500

struct proto
{
    int icmp_proto;                                                         /** Ping send packet type:icmp */
    int sockfd;
    pid_t pid;
    const char *host;
    struct sockaddr *send_addr;                                             /** send address */
    struct sockaddr *recv_addr;                                             /** recv address */
    int send_count;
    void (*fproc_recv)(char *,ssize_t, struct msghdr*,struct timeval *);    /** process recv buffer */
    void (*fproc_send)();                                                   /** process send buffer */
};

void handle_alarm(int);                                                     /** alarm sig handler */
void main_loop(const char *host);                                                           /** main loop func */
int create_and_set_socket(const struct proto *pproto);                      /** socket init */
void init(struct proto *pproto);                                            /**init pproto */
void handle_send();
void handle_recv(char *,ssize_t, struct msghdr*,struct timeval *);
int init_sig_handler(int signo);
struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype);
extern struct proto ipv4_proto;
#endif


//ping.cpp
#include "ping.h"
#include <iostream>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
int create_and_set_socket(const struct proto *pproto)
{
    int sockfd = socket(AF_INET,SOCK_RAW,pproto->icmp_proto);
    setuid(getuid());           /**root -> user */

    int recv_size = 60 * 1024;
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,&recv_size,sizeof(recv_size)) < 0)
    {
        std::cout << "setsockopt failed!" << std::endl;
        return 0;
    }

    return sockfd;

}

struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype)
{
    int n;
    struct addrinfo hints, *res;
    bzero(&hints,sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = family;
    hints.ai_socktype = socktype;

    if((n = getaddrinfo(host,serv,&hints,&res) != 0))
        return 0;
    return res;
}

void main_loop(const char *host)
{


    char recv_buf[BUFSIZE];
    struct msghdr msg;
    struct iovec iov;
    struct addrinfo *addr = host_serv(host,NULL,NULL,0);
    ipv4_proto.send_addr = addr->ai_addr;
    ipv4_proto.host = host;
    bzero(recv_buf,BUFSIZE);
    bzero(&msg,sizeof(struct msghdr));
    bzero(&iov,sizeof(struct iovec));

    iov.iov_base = recv_buf;
    iov.iov_len = BUFSIZE;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_name = ipv4_proto.recv_addr;
    msg.msg_namelen = sizeof(struct sockaddr);
    timeval val;

    /** send first */
    handle_alarm(SIGALRM);
    while(1)
    {
        int size = recvmsg(ipv4_proto.sockfd,&msg,0);
        if(size < 0)
        {
            if(errno == EINTR)
                continue;
            else
            {
                std::cout << "socket failed!" << errno << std::endl;
                return ;
            }
        }

        gettimeofday(&val,NULL);
        ipv4_proto.fproc_recv(recv_buf,size,&msg,&val);
    }
}

void handle_recv(char *buf,ssize_t buf_size, struct msghdr* msg,struct timeval *tv)
{
    //printf("1\n");
    /** buf is ip packet with ip header */
    struct ip *ip_packet;
    struct icmp *icmp_packet;
    int ilen = 0;
    ip_packet = (struct ip*)buf;

    /** ip_hl 4 bit ,and it will tranform to int when The displacement happend
        the left displacement by 2 is *4 */
    int ip_header_len = ip_packet->ip_hl << 2;

    if(ip_packet->ip_p != IPPROTO_ICMP)
    {
        std::cout << "recv packet is not ICMP packet!" << std::endl;
        return ;
    }

    icmp_packet = (struct icmp*)(buf +ip_header_len);
    if((ilen = buf_size - ip_header_len) < 8)
    {
        std::cout << "icmp packet is failed!" << std::endl;
        return ;
    }

    if(icmp_packet->icmp_type == ICMP_ECHOREPLY)
    {
        if(icmp_packet->icmp_id != ipv4_proto.pid)
        {
            //std::cout << "icmp id failed!" << std::endl;
            //return ;
        }

        if(ilen < 16)
        {
            std::cout << "icmp len failed!" << std::endl;
            return ;
        }

        struct timeval *icmp_d = (struct timeval*)icmp_packet->icmp_data;
        int t = (icmp_d->tv_usec - tv->tv_usec);
        if(t < 0)
            t = -t;
        float sub_sec = t/1000.0/1000;
        if(sub_sec == 0)
        {
            sub_sec ++;
        }


        printf("%d bytes from %s: seq = %d,ttl = %d, rtt = %fs\n",
               ilen,
               ipv4_proto.host,
               icmp_packet->icmp_seq,
               ip_packet->ip_ttl,
               sub_sec);

    }


}

void handle_alarm(int signo)
{
    if(ipv4_proto.send_count == 0)
        exit(0);
    ipv4_proto.fproc_send();
    ipv4_proto.send_count--;
    alarm(1);
}


int init_sig_handler(int signo)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_alarm;
    if(sigaction(SIGALRM,&sa,NULL) < 0)
    {
        std::cout << "init sig handler failed!" << std::endl;
        return -1;
    }
}

static uint16_t  in_cksum(uint16_t *addr,int len)
{
    int nleft = len;
    uint32_t  sum = 0;
    uint16_t  *w = addr;
    uint16_t  answer = 0;
    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if(nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char*)w;
        sum += answer;
    }

    sum = (sum >>16) + (sum & 0xffff);
    sum += (sum >>16);
    answer = ~sum;
    return answer;
}

void handle_send()
{
    char buff[BUFSIZE];
    struct icmp *icmp_packet = (struct icmp*)buff;

    icmp_packet->icmp_type = ICMP_ECHO;
    icmp_packet->icmp_code = 0;
    icmp_packet->icmp_code = ipv4_proto.pid;
    icmp_packet->icmp_seq = 3 - ipv4_proto.send_count;
    memset(icmp_packet->icmp_data,0,56);
    gettimeofday((struct timeval*)icmp_packet->icmp_data,NULL);
    int len = 8 + 56;
    icmp_packet->icmp_cksum = in_cksum((u_short *)icmp_packet,len);

    int size = sendto(ipv4_proto.sockfd,buff,len,0,ipv4_proto.send_addr,sizeof(struct sockaddr));
    if(size <= 0)
    {
        std::cout << "sendto failed!" << errno << std::endl;
        return ;
    }
}

//main.cpp
#include "ping.h"
#include <iostream>
#include <unistd.h>

struct proto ipv4_proto = {IPPROTO_ICMP,0,0,0,NULL,NULL,3,handle_recv,handle_send};;

int main(int argc, char **argv)
{
    /** init */

    if(init_sig_handler(SIGALRM) < 0)
        return 1;

    /** socket create set */
    int sockfd = create_and_set_socket(&ipv4_proto);
    ipv4_proto.sockfd = sockfd;
    ipv4_proto.pid = getpid()&0xfffff;


    /** enter main loop */
    main_loop(argv[1]);

    return 0;
}
