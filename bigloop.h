#ifndef BIGLOOP_H
#define BIGLOOP_H

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

typedef unsigned long long int ullong;


#define READY  1
#define END    2
#define STOP   3
#define JOB    4
#define SCORE  5
#define GET    6
#define PID    7
#define VALUE  8

#define UN ( (ullong) 1 ) 
#define PSIZE 30
struct msg  {
  unsigned char idt;  // task id
  unsigned char  op;  // code op
  unsigned int  pid;  // processus id
  ullong deb;  
  ullong fin;
  ullong scr;
} __attribute__ ((__packed__));

typedef struct msg ticket;

#define first ( current.deb )
#define last  ( current.fin )
#define best  ( current.scr )
extern int   IDENT;
extern ullong BEST;
extern int   PORT;
extern char *SERVER;
extern ullong FIRST;
extern ullong STEP;
extern ullong LAST;
extern ullong NBSTEP;
extern int SPLIT;
extern FILE* output;
extern struct sockaddr_in SERVEUR;
extern struct in_addr    ADDRSERV;
extern struct sockaddr_in  CLIENT;
extern int sock_server;
extern ticket current;
int checkavg( void );
void initbigloop( void );
void initserver( void );
void stopserver( void );
int  bigloopargs(int argc, char* argv[]);
void bigloopparms( void );
int getticket( int sock, ticket *p );
int biglregister( void );
int getbigljob( void );
int sendend( void );
int sendvalue( ullong  val);
int sendscore( ullong  val);
void closeloop( ullong wkf );
int sndticketoserver( ticket p );
int sndticketoclient( ticket p );
int getticketfromclient( ticket *p );
int getticketfromzombie( ticket *p );
int sendstop( void );
int sendpid( int pid );
int sendjob( ullong s, ullong f, ullong scr );
void pticket( FILE *, ticket p );
void initloop();
int sendvalue( ullong val);
#endif
