/*
 * a basic loop to
 * check big loop!
 */

#include "bigloop.h"
#include <stdlib.h>


ullong wkf;

int isperfect( ullong z )
{ ullong d;
  ullong s=1;
  for( d = 2; d*d <=z; d++ )
    if ( z % d == 0 ) {
        wkf++;
	s+= d;
	if ( d*d != z ) s+= z/d;
	if ( s > z ) return 0;
    }
  if ( s == z ) return 1;
  return 0;
}



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
	    wkf = 0;
	    s = first;
	    while (s < last) {
		if ( isperfect( s ) )
                   sendvalue( s );
                s++;
	    }
            // here you can :
	    // sendvalue( ... )
	    // sendscore( ... )
	    closeloop(wkf);
	    sendend();
	}

    return 0;
}
