/* 
by Philippe Langevin
last modification March 2012
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



// server methods

int getticketfromzombie( ticket *p )
{ int retry = 2;
  int nb;
  uint len = sizeof( struct sockaddr_in );
  while ( retry-- ){
    nb = recvfrom( sock_server, p, sizeof( ticket )  , MSG_DONTWAIT, (struct sockaddr *) &CLIENT, &len);
    if ( nb <= 0 ) {
      if ( errno != EAGAIN ) 
         perror("zombie");
      else sleep( 10 );
    }
  }   
  return ( nb > 0);
}

int getticketfromclient( ticket *p )
{ 
  int nb;
  uint len = sizeof( struct sockaddr_in );
  nb = recvfrom( sock_server, p, sizeof( ticket )  , 0, (struct sockaddr *) &CLIENT, &len);
  if ( nb <= 0 ) {
    perror("getticketfromclient");
    return 0;
  }   
  if ( p->idt != IDENT ){
    pticket( stderr, *p );
    fprintf( stderr, "\nbad ident (%d) :  ignored", p->idt );
    return 0;
  }
  return 1;
}
int sndticketoclient( ticket p )
{ 
  int nb;
  uint len;
  len = sizeof( CLIENT );
  nb = sendto( sock_server, &p, sizeof( ticket ) , 0, (struct sockaddr *) &CLIENT, len);
  if ( nb < 0 ) {
    perror("sendticketoclient");
    return 0;
  }
  pticket( stderr, p);
  return 1;
}

int sendstop( void )
{
  current.op  = STOP;
  current.scr = 0;
  current.deb = 0;
  current.fin = 0;
  if ( ! sndticketoclient( current) ) {
    perror("send stop");
    return 0;
  }
  return 1;
}

int sendpid( int pid )
{
  current.op  = PID;
  current.pid = pid;
  if ( ! sndticketoclient( current) ) {
    perror("send pid");
    return 0;
  }
  return 1;
}

int sendjob( ullong s, ullong f, ullong score)
{
  current.deb = s;
  current.fin = f;
  current.op  = JOB;
  current.scr = score;
  if ( ! sndticketoclient ( current ) ){
    printf("send job");
    return 0;
  }
  return 1;
}

void initserver( void )
{ 
  sock_server  = socket( AF_INET, SOCK_DGRAM, 0);
  if ( bind( sock_server, (struct sockaddr *) &SERVEUR, sizeof(SERVEUR)) <0) {
      perror("intit socket"); 
      exit(1);
    } 
}

void stopserver( void )
{
  close( sock_server );
}
