#include "socket.h"
#include "logger.h"
#include <fcntl.h>
#include <unistd.h>

namespace Socket{

void setNonBlockAndCloseOnExec(int sockfd){

	// set sockfd O_NONBLOCK flag.
	int flags = ::fcntl(sockfd, F_GETFL);
	flags |= O_NONBLOCK;
	::fcntl(sockfd, F_SETFL, flags);

	// set sockfd FD_CLOEXEC flag.
	flags = ::fcntl(sockfd, F_GETFD, 0);
  	flags |= FD_CLOEXEC;
  	::fcntl(sockfd, F_SETFD, flags);
}

int createNonblockingSocket(int protocol, sa_family_t famliy){
	int type = protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM;
	int sockfd = ::socket(famliy, type, protocol);
	if(sockfd == SOCKET_ERROR){
		LOG_ERROR("socket() call failure. error: %s", getError());
	}
	else 
		setNonBlockAndCloseOnExec(sockfd);
	return sockfd;
}

bool listen(int sockfd){
	if(::listen(sockfd, 10) == SOCKET_ERROR){
		LOG_ERROR("listen() call failure. error: %s", getError());
		return false;
	}
	else 
		return true;
}

bool bind(int sockfd, struct sockaddr* localAddr){
	if(::bind(sockfd, localAddr, static_cast<socklen_t>(sizeof(struct sockaddr))) == SOCKET_ERROR){
		LOG_ERROR("bind() call failure. error: %s", getError());
		return false;
	}
	else 
		return true;
}

bool connect(int sockfd, struct sockaddr* peerAddr){
	return ::connect(sockfd, peerAddr, static_cast<socklen_t>(sizeof(struct sockaddr)));
}

int accept(int sockfd, struct sockaddr* peerAddr){
	socklen_t len = sizeof(struct sockaddr);
	int newSock = ::accept(sockfd, peerAddr, &len);
	if(newSock == SOCKET_ERROR){
		int savedErrno = errno;
		LOG_ERROR("accept() call failure. error: %s", getError());
	    switch (savedErrno)
	    {
	      case EAGAIN:
	      case ECONNABORTED:
	      case EINTR:
	      case EPROTO: // ???
	      case EPERM:
	      case EMFILE: // per-process lmit of open file desctiptor ???
	        // expected errors
	        errno = savedErrno;
	        break;
	      case EBADF:
	      case EFAULT:
	      case EINVAL:
	      case ENFILE:
	      case ENOBUFS:
	      case ENOMEM:
	      case ENOTSOCK:
	      case EOPNOTSUPP:
	        // unexpected errors
	        LOG_FATAL("fatal occur, %s:%d", __FILE__, __LINE__);
	        break;
	      default:
	        LOG_FATAL("fatal occur, %s:%d", __FILE__, __LINE__);
	        break;
	    }
	}

	setNonBlockAndCloseOnExec(newSock);
	return newSock;
}

bool close(int sockfd){
	if(::close(sockfd) == SOCKET_ERROR){
		LOG_ERROR("close socket failure, error: %s", getError());
		return false;
	}
	else 
		return true;
}

int64_t read(int sockfd, char* buff, std::size_t size){
	return ::read(sockfd, buff, size);
}

int64_t write(int sockfd, const char* buff, std::size_t size){
	return ::write(sockfd, buff, size);
}

void shutdownWrite(int sockfd){
	::shutdown(sockfd, SHUT_WR);
}

void getPeerAddress(int sockfd, struct sockaddr& peerAddr){
	socklen_t len = sizeof(struct sockaddr);
	if(::getpeername(sockfd, &peerAddr, &len) == SOCKET_ERROR){
		LOG_ERROR("getPeerAddress() call failure, error: %s", getError());
	}
}

void getLoaclAddress(int sockfd, struct sockaddr& localAddr){
	socklen_t len = sizeof(struct sockaddr);
	if(::getsockname(sockfd, &localAddr, &len) == SOCKET_ERROR){
		LOG_ERROR("getLoaclAddress() call failure, error: %s", getError());
	}
}

}
