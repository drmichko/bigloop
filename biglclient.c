/*
* by Philippe Langevin, March 2012
*/

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include "bigloop.h"

ullong  workfactor = 0;

int checkavg( void )
{
double avg;
int nbproc;
while ( 1 != getloadavg(&avg, 1) )
   sleep( 10 );
nbproc = system("grep -c processor /proc/cpuinfo" );
return nbproc > avg;
}

void closeloop( ullong wkf )
{
FILE *output;
char outname[128];
sprintf(outname, "data/trace-%d-%d.log", IDENT, current.pid);
output    = fopen( outname, "a");
workfactor = wkf; 
fprintf( output, "\n#EOJ s=%Ld wkf=%Ld\n", current.deb, wkf ); 
fclose( output );
}

int sndticketoserver( ticket p )
{ 
  int nb;
  socklen_t len = sizeof( SERVEUR );
  int sock = socket( AF_INET, SOCK_DGRAM, 0);
  nb = sendto( sock, &p, sizeof( ticket ) , 0, (struct sockaddr *) & SERVEUR, len);
  close(sock);
  if ( nb < 0 ) {
    perror("sendticketoserver");
    return 0;
  }
  return 1;
}


int biglregister( void )
{ int nb; 
  int sock = socket( AF_INET, SOCK_DGRAM, 0);
  uint len = sizeof( struct sockaddr_in );
  current.op  = READY;
  current.idt = IDENT;
  current.scr = 0;
  current.pid = 0;
  current.deb = 0;
  current.fin = 0;
  nb = sendto( sock, &current, sizeof( ticket ) , 0, (struct sockaddr *) & SERVEUR, len);
  if ( nb < 0 ) {
    perror("registration");
    return 0;
  }
  getticket( sock, &current );
  close( sock ); 
  return current.op == PID;
}

int sendend( void )
{ int nb; 
  int sock = socket( AF_INET, SOCK_DGRAM, 0);
  uint len = sizeof( struct sockaddr_in );
  current.op  = END;
  current.scr = workfactor;
  nb = sendto( sock, &current, sizeof( ticket ) , 0, (struct sockaddr *) & SERVEUR, len);
  if ( nb < 0 ) {
    perror("sendend");
    return 0;
  }
  close( sock ); 
  return 1;
}
int sendvalue( ullong val)
{ int nb; 
  int sock = socket( AF_INET, SOCK_DGRAM, 0);
  uint len = sizeof( struct sockaddr_in );
  current.op  = VALUE;
  current.scr = val;
  nb = sendto( sock, &current, sizeof( ticket ) , 0, (struct sockaddr *) & SERVEUR, len);
  if ( nb < 0 ) {
    perror("send value");
    return 0;
  }
  close( sock ); 
  return 1;
}


int sendscore( ullong val)
{ int nb; 
  int sock = socket( AF_INET, SOCK_DGRAM, 0);
  uint len = sizeof( struct sockaddr_in );
  current.op  = SCORE;
  current.scr = val;
  nb = sendto( sock, &current, sizeof( ticket ) , 0, (struct sockaddr *) & SERVEUR, len);
  if ( nb < 0 ) {
    perror("send value");
    return 0;
  }
  close( sock ); 
  return 1;
}

int getbigljob( void )
{ int nb; 
  int sock = socket( AF_INET, SOCK_DGRAM, 0);
  uint len = sizeof( struct sockaddr_in );
  current.op  = GET;
  current.scr = 0;
  current.deb = 0;
  current.fin = 0;
  nb = sendto( sock, &current, sizeof( ticket ) , 0, (struct sockaddr *) & SERVEUR, len);
  if ( nb < 0 ) {
    perror("newjob");
    return 0;
  }
  getticket( sock, &current );
  close( sock ); 
  if ( current.op == STOP )
    return 0;
  return 1;
}
