/* 
 * Ph. Langevin.
 * March 2012
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

#define UN ( (ullong) 1 ) 

int   IDENT  = 0;
int   PORT   = 0;
char *SERVER = NULL;
ullong FIRST = 0, STEP = 0, LAST   = 0, BEST = 0;
ullong  NBSTEP = 0;
int SPLIT=0;

struct sockaddr_in SERVEUR;
struct in_addr    ADDRSERV;
struct sockaddr_in  CLIENT;
int sock_server;
ticket current;


int bigloopargs(int argc, char* argv[])
{
  char *optliste = "a:b:i:f:l:p:s:hx:F:L:S:X:";
  int   s, opt;
  while ( ( opt = getopt(argc, argv, optliste )) >=0 ) {
	    switch ( opt ){
	    case 'i' : IDENT    = atoi(optarg);         break;
	    case 'p' : PORT     = atoi(optarg);         break;
            case 'b' : BEST     = atoi(optarg);         break;
            case 'a' : SERVER   = strdup(optarg);       break;
            case 's' : STEP     = atoll(optarg);        break;
	    case 'l' : LAST     = atoll(optarg);        break;
	    case 'f' : FIRST    = atoll(optarg);        break;
            case 'S' : s = atoi(optarg); STEP  = UN << s;  break;
	    case 'L' : s = atoi(optarg); LAST  = UN << s;  break;
	    case 'F' : s = atoi(optarg); FIRST = UN << s;  break;
            case 'X' : s = atoi(optarg); SPLIT = UN << s;  break;
	    case 'x' : SPLIT    = atoi(optarg);
            case 'h' :
	    default  : printf("\nbigloop usage : %s", optliste);
                       return 0;
	    }
	  }
return 1;
}


int getticket( int sock, ticket *p )
{
  int nb;
  uint len = sizeof( struct sockaddr_in );
  struct sockaddr addr;
  nb = recvfrom( sock, p, sizeof( ticket ), 0  , &addr, &len);
  if ( nb <= 0 ) return 0;
  return 1;
}


struct in_addr adresse( char *str )
{ struct in_addr res;
  struct hostent *infos; 
  infos = gethostbyname( str );
  if ( ! infos ) {
           perror("\nget hostname:");
           exit(1);
	 };
  memcpy((char *) &res, infos->h_addr_list[0], infos->h_length);
  return res;
}
 
void pticket( FILE*dst, ticket p )
{ fprintf(dst, "\n");
  switch ( p.op ){
     case READY   : fprintf(dst, "RDY "); break;
     case   JOB   : fprintf(dst, "JOB "); break;
     case  STOP   : fprintf(dst, "STP "); break;
     case  END    : fprintf(dst, "END "); break;
     case  PID    : fprintf(dst, "PID "); break;
     case  GET    : fprintf(dst, "GET "); break;
     case  VALUE  : fprintf(dst, "VAL "); break; 
     case  SCORE  : fprintf(dst, "SCR "); break;
     default      : fprintf(dst, "UNK "); break;
  }
  fprintf(dst, "%u:%u %Ld %Ld %Ld : ", p.idt, p.pid, p.deb, p.fin, p.scr);
  fprintf(dst, " %s:%d", inet_ntoa( CLIENT.sin_addr ) , CLIENT.sin_port);
} 

void initsocketserver( void )
{ 
  memset( & SERVEUR, 0 , sizeof( SERVEUR ) );
  SERVEUR.sin_family = AF_INET;       
  SERVEUR.sin_port = htons( PORT );    
  SERVEUR.sin_addr = ADDRSERV;
}
void initbigloop( void )
{ char line[128], str[128];
  FILE * src;
  int psize;
  src = fopen( "bigloop.conf", "r");
  if ( ! src ){
    perror("\nconfiguation file not found!");
    exit( 1 );
  }
  psize =  sizeof(ticket);
  if ( psize != PSIZE ) {
    fprintf(stderr, "ticket size is %d?\n", psize);
    exit( 1 );
  }
  while ( ! feof( src) ){
    line[0] = 0;
    fscanf( src, "%s", line);
    switch( line[0] ){
	    case 'i' : 
	      if ( IDENT == 0 )
	          sscanf(line, "ident=%d", &IDENT);
	      break;
	    case 'p' :
	      if ( PORT == 0 )
		sscanf(line, "port=%d",  &PORT);
	      break;
            case 'a' : 
              if ( SERVER == NULL ){
	      sscanf(line, "address=%s",  (char*) &str);
	      SERVER = strdup( str );
	      }
	      break;
            case 'b' :
	      sscanf(line, "best=%Ld",  &BEST);
	      break;
            case 's' :
	      if ( STEP ==  0 )
		sscanf(line, "step=%Ld", &STEP);
	      break;
	    case 'f' : 
	      if ( FIRST == 0 )
		sscanf(line, "first=%Ld", &FIRST);
	      break;
	    case 'l' :
	      if ( LAST == 0 )
		sscanf(line, "last=%Ld", &LAST);
	      break;
           case 'S' :
	     if ( STEP ==  0 ){
		sscanf(line, "Step=%Ld", &STEP);
		STEP = UN << STEP;
	     }
	      break;
	    case 'F' : 
	      if ( FIRST == 0 ){
		sscanf(line, "First=%Ld", &FIRST);
		FIRST = UN << FIRST;
	      }
	      break;
	    case 'L' :
	      if ( LAST == 0 ){
		sscanf(line, "Last=%Ld", &LAST);
		LAST = UN << LAST;
	      }
	      break;
	    }
  }
  fclose(src);
  if ( SPLIT ) STEP = ( LAST - FIRST) / SPLIT;
  if ( ! STEP ) {
    printf("\nbad increment!");
    exit(0);
  }
  NBSTEP = ( LAST - FIRST ) / STEP;

  ADDRSERV =   adresse( SERVER );
  initsocketserver();
}

void bigloopparms( void )
{
  printf("\nusing server : %s-%d", SERVER, PORT);
  printf("\nidentificator: %d ", IDENT);
  printf("\nloop from %Ld to %Ld by %Ld (%Ld steps)", FIRST, LAST, STEP, NBSTEP);
  printf("\nusually it takes a while...\n");
}


