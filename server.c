#include <signal.h>
#include "misc.h"

static fd_set master;
static int fdmax, listener;   /* maximal value + 1 */
static char buff[ bufferMAX ];

void quit( int listener, fd_set *master, int fdmax ){
  char *msg = (char *)malloc( bufferMAX );
  int i;
  
  strcpy( msg, "Server quits\n" );
  for( i = 0; i < fdmax; ++i ){
    if( FD_ISSET( i, master ) && i != listener ){
      write_all( i, msg, strlen(msg) );
      FD_CLR( i, master );
      close_socket( i );
    }
  }
  FD_CLR( listener, master );
  close_socket( listener );
  puts( "\nserver quits" );
  
  free( msg );
  exit( 0 );
}

void handler( int sig ){
  signal( sig, SIG_IGN );
  
  quit( listener, &master, fdmax );
}

int main( int argc, char **argv ){
  fd_set readfd;
  int i, j, newfd, usercnt = 0, length, action;
  char yes = 1;
  struct sockaddr_in server_addr, client_addr;
  struct hostent *hostp;
  
  FD_ZERO( &master ); FD_ZERO( &readfd );
  
  signal( SIGINT, handler );
  signal( SIGABRT, handler );
  signal( SIGTERM, handler );
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons( PORT );
  server_addr.sin_addr.s_addr = INADDR_ANY;
  memset( &( server_addr.sin_zero ), 0 , sizeof( server_addr.sin_zero ) );
  
  listener = create_socket( AF_INET, SOCK_STREAM, 0  );
  
  reuse_socket( listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) );
  
  bind_socket( listener, (struct sockaddr *)&server_addr, sizeof( server_addr ) );
  
  set_listener( listener, connectionMAX );
  
  gethostname( buff, sizeof( buff ) );
	hostp = get_host_by_name( buff );
  
  printf( "Server [%s] is running, [%s] on socket [%d], port opened [%d]\n",buff, inet_ntoa( *((struct in_addr *)hostp->h_addr_list[0]) ), listener, PORT );
  
  fdmax = listener + 1;
  FD_SET( listener, &master );
  
  while( 1 ){
    readfd = master; 
    check_sockets( listener, &master, fdmax, &readfd, NULL, NULL, NULL );
    
    for( i = 0; i < fdmax; ++i ){
      if( FD_ISSET( i, &readfd ) ){
        
        memset( buff, 0, sizeof buff );
        
        if( i == listener ){
          /* novi korisnik */
          length = sizeof( client_addr );
          newfd = accept_client( listener, (struct sockaddr *)&client_addr, &length );
          
          if( newfd < 0 ){
          
          } else if( usercnt < connectionMAX ){
            /* posalji svim korisnicima info o novom korisniku + broj aktivnih korisnika*/
            
            sprintf( buff, "New user from [%s], [%d] users active\n", inet_ntoa( client_addr.sin_addr ), usercnt );
            
            for( j = 0; j < fdmax; ++j ){
              if( FD_ISSET(j, &master) && j != listener )
                write_all( j, buff, strlen(buff) );
              }
            
            /* posalji novom korisniku */
            
            sprintf( buff, "Welcome [%s], [%d] users active\n",inet_ntoa( client_addr.sin_addr ), usercnt );
            write_all( newfd, buff, strlen(buff) );
            
            /* server side poruka */
            printf( "New user from [%s] on socket [%d]\n", inet_ntoa(client_addr.sin_addr ),newfd );
            
            if( newfd + 1 > fdmax )fdmax = newfd + 1;
            FD_SET( newfd, &master );
            usercnt++;
          }else {
            /* posalji korisniku da ima previse ljudi gore */
            sprintf( buff, "Server is full\n" );
            write_all( newfd, buff, strlen(buff) );
            close_socket( newfd );
          }
          
        } else{
          /* mozemo primati podatke od korisnika */
          /* ako ne primimo nista onda je korisnik prekino vezu */
          if( (action = read_all( i, buff, sizeof( buff ) )) == 0 ){
            /* korisnik je prekinuo */
            FD_CLR( i, &master );
            usercnt--;
            
            sprintf( buff, "Users remaining [%d]\n", usercnt );
            
            for( j = 0; j < fdmax; ++j ){
              if( FD_ISSET(j, &master) && j != listener )
                write_all( j, buff, strlen(buff) );
            }
            
            printf( "User on socket [%d] hung up, [%d] users remaining\n", i, usercnt);
            close_socket( i );

          } else{
            /* posalji ostalim korisnicima poruku */
            for( j = 0; j < fdmax; ++j ){
              if( FD_ISSET(j, &master) && j != listener && j != i )
                write_all( j, buff, strlen(buff) );
            }
          }
          
          
        }
      }
    }
    
  }
  
  quit( listener, &master, fdmax );
  return 0;
  }
