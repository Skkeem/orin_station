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

/* $begin orin.cpp */
#include "stdafx.h"
#include "orin.h"

/************************** 
 * Error-handling functions
 **************************/
void wsa_error(char *msg) /* WSA-style error */
{
//	fprintf(stderr, "%s: %d\n", msg, WSAGetLastError());
	exit(0);
}

void posix_error(int code, char *msg) /* Posix-style error */
{
//	fprintf(stderr, "%s: %d\n", msg, code);
	exit(0);
}

void app_error(char *msg) /* Application error */
{
//	fprintf(stderr, "%s\n", msg);
	exit(0);
}


/**************************** 
 * Sockets interface wrappers
 ****************************/
int Socket(int domain, int type, int protocol) 
{
	int rc;

	if ( (rc = socket(domain, type, protocol)) == INVALID_SOCKET)
		wsa_error("Socket error");
		//may be you can ignore WSAEMFILE and WSAENOBUFS.

	return rc;
}

void Setsockopt(SOCKET s, int level, int optname, const void *optval, int optlen) 
{
	int rc;

	if ( (rc = setsockopt(s, level, optname, (const char *)optval, optlen)) == SOCKET_ERROR)
		wsa_error("Setsockopt error");
		//WSAENETRESET, WSAENOTCONN. what is SO_KEEPALIVE???

	return;
}

void Bind(SOCKET sockfd, struct sockaddr *my_addr, int addrlen) 
{
	int rc;

	if ( (rc = bind(sockfd, my_addr, addrlen)) == SOCKET_ERROR)
		wsa_error("Bind error");
		//WSAENOBUFS

	return;
}

void Listen(SOCKET s, int backlog) 
{
	int rc;

	if ( (rc = listen(s,  backlog)) == SOCKET_ERROR)
		wsa_error("Listen error");


	return;
}

int Accept(SOCKET s, struct sockaddr *addr, socklen_t *addrlen) 
{
	int rc;

	if ( (rc = accept(s, addr, (int *)addrlen)) == INVALID_SOCKET) {
		if (WSAGetLastError() == WSAEINTR)
			rc = Accept(s, addr, addrlen);
		else
			wsa_error("Accept error");
	}

	return rc;
}

void Connect(SOCKET sockfd, struct sockaddr *serv_addr, int addrlen) 
{
	int rc;

	if ( (rc = connect(sockfd, serv_addr, addrlen)) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEINTR)
			Connect(sockfd, serv_addr, addrlen);
		else
			wsa_error("Connect error");
	}

	return;
}

void Closesocket(SOCKET s) 
{
	int rc;

	if ((rc = closesocket(s)) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEINTR)
			Closesocket(s);
		else
			wsa_error("Closesocket error");
	}

	return;
}

int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) 
{
	int rc;

	if ((rc = select(n, readfds, writefds, exceptfds, timeout)) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEINTR)
			rc = Select(n, readfds, writefds, exceptfds, timeout);
		else
			wsa_error("Select error");
	}

	return rc;
}


/*********************************************************************
 * The Rio package - robust I/O functions
 **********************************************************************/
/*
 * rio_recvn - robustly receive n bytes (unbuffered)
 */
/* $begin rio_recvn */
ssize_t rio_recvn(SOCKET s, void *usrbuf, size_t n, int flags) 
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = (char *)usrbuf;

	while (nleft > 0) {
		if ( (nread = recv(s, bufp, nleft, flags)) == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEINTR)	/* Interrupted by sig handler return */
				nread = 0;		/* and call read() again */
			else
				return -1;		/* errno set by read() */ 
		}
		else if (nread == 0)
			break;				/* EOF */
		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft);			/* return >= 0 */
}
/* $end rio_recvn */

/*
 * rio_sendn - robustly send n bytes (unbuffered)
 */
/* $begin rio_sendn */
ssize_t rio_sendn(SOCKET s, void *usrbuf, size_t n, int flags)
{
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = (char *)usrbuf;

	while (nleft > 0) {
		if ( (nwritten = send(s, bufp, nleft, flags)) <= 0) {
			if (WSAGetLastError() == WSAEINTR)	/* Interrupted by sig handler return */
				nwritten = 0;	/* and call write() again */
			else
				return -1;		/* errno set by write() */
		}
		nleft -= nwritten;
		bufp += nwritten;
	}

	return n;
}
/* $end rio_sendn */


/**********************************
 * Wrappers for robust I/O routines
 **********************************/
ssize_t Rio_recvn(SOCKET s, void *ptr, size_t nbytes, int flags) 
{
	ssize_t n;
  
	if ( (n = rio_recvn(s, ptr, nbytes, flags)) < 0) {
		if (WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAESHUTDOWN)	/* Connection broken */
			n = 0;
		else
			wsa_error("Rio_recvn error");
	}

	return n;
}

void Rio_sendn(SOCKET s, void *usrbuf, size_t n, int flags) 
{
	if (rio_sendn(s, usrbuf, n, flags) != n)
		if (WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAESHUTDOWN) {	/* Connection broken */
			return;
		}
		else
			wsa_error("Rio_sendn error");
	return;
}

void Rio_sendn_mux(SOCKET *connfds, int i, void *usrbuf, size_t n, int flags) 
{
	if (rio_sendn(connfds[i], usrbuf, n, flags) != n) {
		if (WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAESHUTDOWN) {	/* Connection broken */
			Closesocket(connfds[i]);
			connfds[i] = NO_SOCKET;
		}
		else
			wsa_error("Rio_sendn error");
	}
	return;
}


/******************************** 
 * Client/server helper functions
 ********************************/
/*
 * open_clientsock - open connection to server at <ipaddr, port> 
 *   and return a socket descriptor ready for receiving and sending.
 *   Returns -1 and sets errno on Unix error. 
 */
/* $begin open_clientsock */
SOCKET Open_clientsock(char *ipaddr, int port) 
{
	int clientsock;
	struct sockaddr_in serveraddr;

	clientsock = Socket(AF_INET, SOCK_STREAM, 0);

	/* Fill in the server's IP address and port */
	memset((char *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons((unsigned short)port);

	/* Establish a connection with the server */
	Connect(clientsock, (SA *) &serveraddr, sizeof(serveraddr));

	return clientsock;
}
/* $end open_clientsock */

/*  
 * open_listensock - open and return a listening socket on port
 *	 Returns -1 and sets errno on Unix error.
 */
/* $begin open_listensock */
SOCKET Open_listensock(int port) 
{
	int listensock, optval = 1;
	struct sockaddr_in serveraddr;
  
	/* Create a socket descriptor */
	listensock = Socket(AF_INET, SOCK_STREAM, 0);
 
	/* Eliminates "Address already in use" error from bind */
	Setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval , sizeof(int));

	/* Listensock will be an endpoint for all requests to port
	   on any IP address for this host */
	memset((char *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	Bind(listensock, (SA *)&serveraddr, sizeof(serveraddr));

	/* Make it a listening socket ready to accept connection requests */
	Listen(listensock, LISTENQ);
	
	return listensock;
}
/* $end open_listensock */


/******************************************
 * Winsock initialiser 
 ******************************************/
void Startup_winsock() {
	WSADATA wsa;
	int rc;

	if ( (rc = WSAStartup(MAKEWORD(2,2), &wsa)) != 0) {
//		fprintf(stderr, "WSAStartup error: %d\n", rc);
		exit(0);
	}

	return;
}

void Cleanup_winsock() {
	if (WSACleanup() == SOCKET_ERROR) {
//		fprintf(stderr, "WSACleanup error: %d\n", WSAGetLastError());
//		exit(0);
		wsa_error("WSACleanup error");
	}

	return;
}


/* $end orin.c */
//#endif