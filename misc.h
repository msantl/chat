#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT          3111
#define connectionMAX 10
#define bufferMAX     1024

void quit( int sockfd, fd_set *master, int fdmax );
/* implementirana zasebno za klijenta i za servera */

int isprintable( int ch );

void close_socket( int sockfd );

int read_all( int sockfd, char *message, int length );

int write_all( int sockfd, char *message, int length );

int create_socket( int domain, int type, int protocol );

void reuse_socket( int sockfd, int level, int option, char *value, int length );

void bind_socket( int listener, struct sockaddr *server_addr, int size );

void set_listener( int listener, int backlog );

void check_sockets( int listener, fd_set *master, int fdmax, fd_set *readfd, fd_set *writefd, fd_set* exceptfd, struct timeval *timeout );

int accept_client( int sockfd, struct sockaddr *addr, int *addrlen );

void connect_socket( int sockfd, struct sockaddr *addr, int size );

struct hostent* get_host_by_name( char *host );
