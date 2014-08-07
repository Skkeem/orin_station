/**************************************************************************************
 **************************************************************************************
 **************************************************************************************
 *   @author : Sung Kook Kim (ingenuus@kaist.ac.kr)
 *   @date : 2014/07/29
 *   @description : Winsock with wrappers for error-handling
 *                 + robust I/O package for networking.
 **************************************************************************************
 **************************************************************************************
 **************************************************************************************/

/* $begin orin.h */
#ifndef __ORIN_H__
#define __ORIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")

/* Simplifies calls to bind(), connect(), and accept() */
/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */

/* Misc types */
typedef unsigned int socklen_t;
typedef int ssize_t;

/* Misc constants */
#define	MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  5 //1024  /* second argument to listen() */
#define NO_SOCKET -1

/* Our own error-handling functions */
void wsa_error(char *msg);
void posix_error(int code, char *msg);
void app_error(char *msg);

/* Sockets interface wrappers */
int Socket(int domain, int type, int protocol);
void Setsockopt(SOCKET s, int level, int optname, const void *optval, int optlen);
void Bind(SOCKET sockfd, struct sockaddr *my_addr, int addrlen);
void Listen(SOCKET s, int backlog);
int Accept(SOCKET s, struct sockaddr *addr, socklen_t *addrlen);
void Connect(SOCKET sockfd, struct sockaddr *serv_addr, int addrlen);
void Closesocket(SOCKET s);
int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

/* Rio (Robust I/O) package */
ssize_t rio_recvn(SOCKET s, void *usrbuf, size_t n, int flags);
ssize_t rio_sendn(SOCKET s, void *usrbuf, size_t n, int flags);

/* Wrappers for Rio package */
ssize_t Rio_recvn(SOCKET s, void *usrbuf, size_t n, int flags);
void Rio_sendn(SOCKET s, void *usrbuf, size_t n, int flags);
void Rio_sendn_mux(SOCKET *connfds, int i, void *usrbuf, size_t n, int flags);

/* Client/server helper functions */
SOCKET Open_clientsock(char *ipaddr, int portno);
SOCKET Open_listensock(int portno);

/* Winsock Startup & Cleanup */
void Startup_winsock();
void Cleanup_winsock();


#endif /* __ORIN_H__ */
/* $end orin.h */