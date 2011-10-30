#include "misc.h"

int isprintable( int ch ){
	return isprint( ch );
	}

void close_socket( int sockfd ){
  close( sockfd );
  return;
}

int read_all( int sockfd, char *message, int length ){
  if( read( sockfd, message, length ) <= 0 )return 0;
  return 1;
}

int write_all( int sockfd, char *message, int length ){
  int written;

  while( length ){
    if( (written = write( sockfd, message, length )) == -1 ){
      puts( "write error" );
      return 0;
    } else{
      length -= written;
      message += written;
    }
  }

  return 1;
}


int create_socket( int domain, int type, int protocol ){
  int listener;
  
  if( ( listener = socket( domain, type, protocol ) ) == -1 ){
    puts( "socket error" );
    exit( 1 );
  } else{
    puts( "socket ok" );
    return listener;
  }

  return -1;
}

void reuse_socket( int sockfd, int level, int option, char *value, int length ){
  if( setsockopt( sockfd, level, option, value, length ) == 1 ){
    puts( "setsockopt error" );
    close_socket( sockfd );
    exit( 1 );
  } else{
    puts( "setsockopt ok" );
  }
  
  return;    
}

void bind_socket( int listener, struct sockaddr *server_addr, int size ){
  if( bind( listener, server_addr, size ) == -1 ){
    puts( "bind error" );
    close_socket( listener );
    exit( 1 );
  } else{
    puts( "bind ok" );
  }

  return;
}

void set_listener( int listener, int backlog ){
  if( listen( listener, backlog ) == -1 ){
    puts( "listen error" );
    close_socket( listener );
    exit( 1 );
  } else{
    puts( "listen ok" );
  }

  return;  
}

void check_sockets( int listener, fd_set *master, int fdmax, fd_set *readfd, fd_set *writefd, fd_set* exceptfd, struct timeval *timeout ){
  if( select( fdmax, readfd, writefd, exceptfd, timeout ) == -1 ){
    puts( "select error" );
    quit( listener, master, fdmax );
  } else{
    puts( "select ok" );
  }
  
  return;
}

int accept_client( int sockfd, struct sockaddr *addr, int *addrlen ){
  int newfd;
  
  if( ( newfd = accept( sockfd, addr, (socklen_t *)addrlen ) ) == -1 ){
    puts( "accept error" );
  } else{
    puts( "accept ok" );
    return newfd;
  }
  
  return -1;
}

void connect_socket( int sockfd, struct sockaddr *addr, int size ){
  if( connect( sockfd, addr, size ) == -1 ){
    puts( "connect error" );
    close_socket( sockfd );
    exit( 1 );
  } else
    puts( "connect ok" );
  
  return;
}

struct hostent* get_host_by_name( char *host ){
  struct hostent *hostp;
  
  if( ( hostp = gethostbyname( host ) ) == NULL )  {
    puts( "host resolving error" );
    exit( 1 );
  } else
    puts( "host resolving ok" );
    
  return hostp;
}
