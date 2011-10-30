#define _REENTRANT
#include <pthread.h>
#include <ncurses.h>
#include "misc.h"

#define SERVER "localhost"
static char server[ bufferMAX ], buffer[ bufferMAX ], nick[ bufferMAX ];
static int totalcnt1, totalcnt2;

void quit( int sockfd, fd_set *master, int fdmax ){
  char *msg = (char *)malloc( bufferMAX );
  
  endwin();
  
  sprintf( msg, "User %s quits\n", nick );
  
  write_all( sockfd, msg, strlen( msg ) );
  
  puts( "\nclient quits" );
    
  free( msg );
  exit( 0 );
}

void *user_input( void *arg ){
  int ch;
  int currx, curry;
  int *sockfd = (int *)arg;
  
  while( 1 ){
    
    do{
      memset( buffer, 0, sizeof buffer );
      strcpy( buffer, nick );
      strcat( buffer, " >" );
      totalcnt1 = totalcnt2;
      
      getmaxyx( stdscr, curry, currx );
      move( curry - 1, 0);
      printw( "%s", buffer ); refresh();
      
      while( (ch = getch()) != '\n' && ch != '\r' ){
        /* TODO handlanje za ostale znakove tipa strelice */
        if( ch == 127 && totalcnt1 > totalcnt2 ){  /* backspace */
          getyx( stdscr, curry, currx );
          move( curry, currx - 1 );
          delch();
          totalcnt1--;
          
        } else if( isprintable( ch ) && totalcnt1 + 2 < bufferMAX ){
          addch( ch );
          buffer[ totalcnt1++ ] = ch;
        }
        
        refresh();
      }
      
    } while( totalcnt1 == totalcnt2  );
    
    buffer[ totalcnt1++ ] = '\n';    buffer[ totalcnt1 ] = '\0';
    
    if( strncmp( buffer + totalcnt2, "/exit", 5 ) == 0 ){ quit( *sockfd, NULL, 0 ); }
    
    scrl( 1 );  refresh();
    
    if( write_all( *sockfd, buffer, strlen( buffer ) ) == 0 ){
      quit( *sockfd, NULL, 0 );
    }
  
  }
  
  return NULL;
}

void *host_input( void *arg ){
  int currx, curry;
  int *sockfd = (int *)arg;
  
  while(1){
    memset( server, 0, sizeof( server ) );
  
    if( read_all( *sockfd, server, sizeof( server ) ) == 0 ){
      quit( *sockfd, NULL, 0 );
    } else{
      getyx( stdscr, curry, currx );
      move( curry, 0 );
      
      printw( "%s", server );
      
      buffer[ totalcnt1 ] = '\0';
      printw( "%s", buffer );
      
      refresh();
    }
    
  }
  return NULL;
}

int main( int argc, char **argv ){
  int sockfd, maxx, maxy;
  struct sockaddr_in server_addr;
  struct hostent *hostp;
  pthread_t *user, *host;
  
  if( argc < 2 ){
    puts( "Program usage" );
    printf( "%s [nick] [server]\n", argv[0] );
    puts( "If [server] left blank it connects to localhost" );
    exit( 0 );
  }
  if( argc < 3 )strcpy( server, SERVER );
  else strcpy( server, argv[2] );
  
  strcpy( nick, argv[1] );
  
  totalcnt2 = strlen( nick ) + 2;
  totalcnt1 = 0;
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons( PORT );
  
  hostp = get_host_by_name( server );
  
  memcpy( &server_addr.sin_addr, hostp->h_addr_list[0], hostp->h_length);
  memset( &server_addr.sin_zero, 0, sizeof( server_addr.sin_zero ) );

  sockfd = create_socket( AF_INET, SOCK_STREAM, 0 );
  connect_socket( sockfd, (struct sockaddr *)&server_addr, sizeof( server_addr ) );  
  
  /* ncurses init */
  
  initscr();
  getmaxyx( stdscr, maxy, maxx );
  scrollok( stdscr, 1 );
  raw();
  noecho();
  move( maxy - 1, 0 );
  refresh();
  
  /* ncurses end */
  
  /* pthread init */
  
  
  user = (pthread_t *)malloc( sizeof( *user ) );
  host = (pthread_t *)malloc( sizeof( *host ) );
  
  pthread_create( user, NULL, user_input, &sockfd );
  pthread_create( host, NULL, host_input, &sockfd );
  
  pthread_join( *user, NULL );
  pthread_join( *host, NULL );
  
  /* pthread end */
    
  quit( sockfd, NULL, 0 );
  return 0;    
}
