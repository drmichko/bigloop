#include "bigloop.h"
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    ullong s;

    if (!bigloopargs(argc, argv))
	exit(1);

    initbigloop();
    /*
    if (!checkavg())
	return 1;
    */

    if (biglregister())
	while (getbigljob()) {
	    s = first;
	    int tid = idt;
	    while (s < last) {
	        char cmd [256];
                sprintf( cmd, "/home/langevin/gitub/apin/newplay.sh %lld %d", s, tid ); 
		int ret = system( cmd );
	        if ( ret  == 0 )  s++;
		else sleep(5);
		struct timespec ts;
    		ts.tv_sec = 0;          // secondes
    		ts.tv_nsec = (30+random() %30 )  * 1000000L; 

    		nanosleep(&ts, NULL);
	    }
            // here you can :
	    // sendvalue( ... )
	    // sendscore( ... )
	    //closeloop(wkf);
	    sendend();
	}

    return 0;
}
