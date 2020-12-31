#ifndef LL_SOCKET_H
#define LL_SOCKET_H
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <error.h>
#include <string.h>
#include <string>
#define SOCKET_ERROR -1

namespace Socket{

/**
 * set sockfd to no blocking and close-on-exec flag.
 */
void setNonBlockAndCloseOnExec(int sockfd);

/**
 * Create a non-blocking socket.
 * @Param: 
 * protocol is represented UDP or TCP (just like IPPROTO_TCP or IPPROTO_UDP)
 * famliy is represented IPv4 or IPv6 (just like AF_INET or AF_INET6)
 */
int createNonblockingSocket(int protocol, sa_family_t family = AF_INET);

/**
 * listen the socket.
 * @Return: On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 * Note: listen(sockfd, maxNums) this maxNums is pending queue and waiting-accpet queue size. this maxNums = 10 is Ok.
 */
bool listen(int sockfd); 

/**
 * bind the socket.
 * @Return: On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
bool bind(int sockfd, struct sockaddr* localAddr); 

/**
 * connect peer host.
 * @Param: 
 * sockfd is local socket regard as client.
 * peerAddr is peer host address regard as server.
 * @Return: On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 * @Note: connect always isn't used by server.
 */
bool connect(int sockfd, struct sockaddr* peerAddr);

/**
 * accept from network by tcp.
 * @Return: On success, sockfd is returned.  On error, -1 is returned, and errno is set appropriately.
 */
int accept(int sockfd, struct sockaddr* peerAddr); 

/**
 * close the socket.
 * @Return: On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
bool close(int sockfd);

/**
 * read from socket.
 * @Param:
 * buff must be allocated by usr.
 * size is buff size, must >= 0.
 * @Return: On success, the number of bytes read is returned. On error, -1 is returned, and errno is set appropriately.
 */
int64_t read(int sockfd, char* buff, std::size_t size);

/**
 * write data to socket.
 * @Return: On success, the number of bytes write is returned. On error, -1 is returned, and errno is set appropriately.
 */
int64_t write(int sockfd, const char* buff, std::size_t size);

/**
 * shutdown the socket out.
 */
void shutdownWrite(int sockfd);

/**
 * get socket errno.
 */
 inline int getErrno(){
	return errno;
 }

 /**
  * get socket error string.
  */
 inline const char* getError(){
	return ::strerror(errno);
 } 

/**
 * get peer address.
 */
void getPeerAddress(int sockfd, struct sockaddr& peerAddr);

/**
 * get local address.
 */
void getLoaclAddress(int sockfd, struct sockaddr& localAddr);


}

#endif
