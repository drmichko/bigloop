/* 
by Philippe Langevin
last modification March 2012
*/

#include "bigloop.h"

#define NBPROC 1024
#define NOT    0
#define RUN    1
#define RDY    2
int cputime = 0, runtime;
int actif = 0;
int jobcount = 0;
int proc = 0;
ullong work = 0;
time_t initial;
ullong score = 0;
extern ticket current;
char outname[64], procname[64], infoname[64];

typedef struct proc {
    int status;
    ullong deb;
    ullong fin;
    time_t date;
    struct in_addr mac;
} proc_t;


typedef struct lj_ {
    ullong deb;
    struct lj_ *next;
} enrlost, *lost;

lost lostjobs = NULL;
ullong lastjob=0;

typedef struct  {
    ullong wkf;
    ullong cpu;
    struct in_addr mac;
    int qte;
    float perf;
} data;

typedef struct log_ {
    data *ptr;
    struct log_ *next;
} enrlog, *listlog;

proc_t proctable[NBPROC];

void openproc(void)
{
    actif++;
    proctable[proc].mac = CLIENT.sin_addr;
    proctable[proc].status = RDY;
}

void initjob(ullong s, ullong f)
{
    int pid;
    pid = current.pid;
    proctable[pid].date = time(NULL);
    proctable[pid].status = RUN;
    proctable[pid].deb = s;
    jobcount++;
    fprintf(stderr, "\n>>> %sjob started on %s", ctime(&proctable[pid].date),
	    inet_ntoa(proctable[pid].mac));
}

void savejob()
{
    FILE *dst;
    int run;
    int pid;
    pid = current.pid;
    run = difftime(time(NULL), proctable[pid].date);
    proctable[pid].status = RDY;
    fprintf(stderr, "\n%d sec on %s", run, inet_ntoa(proctable[pid].mac));
    dst = fopen(infoname, "a");
    fprintf(dst, "%Ld:%Ld:%d:%s\n", current.deb, current.scr, run,
	    inet_ntoa(proctable[pid].mac));
    fclose(dst);
}

void savevalue(void)
{
    ullong val;
    FILE *dst;
    dst = fopen(outname, "a");
    val = current.scr;
    fprintf(dst, "value=%Ld\n", val);
    fclose(dst);
}
void savescore(void)
{   FILE *dst;
    if ( current.scr <= score ) return;
    score = current.scr; 
    dst = fopen(outname, "a");
    fprintf(dst, "score=%Ld\n", score);
    fclose(dst);
}



void initproc(void)
{
    int pid;
    for (pid = 0; pid < NBPROC; pid++)
	proctable[pid].status = NOT;
    initial = time(NULL);
}

void logprocess( ullong s )
{   //char buffer[32];
    FILE *dst;
    dst = fopen(procname, "w");
    int pid;
    fprintf(dst, "\niter=%Ld", s );
    for (pid = 0; pid < NBPROC; pid++)
	if (proctable[pid].status != NOT) {
            fprintf(dst,"\n>>> %s",   ctime( &proctable[pid].date) );
	    switch (proctable[pid].status) {
	    case RDY:
		fprintf(dst, "RDY");
		break;
	    case RUN:
		fprintf(dst, "RUN");
		break;
	    }
	    fprintf(dst, " %Ld:%s", proctable[pid].deb,
		    inet_ntoa(proctable[pid].mac)
		   );
	}
    fclose(dst);
}


int acceptstep( ullong s )
{
lost aux;
if ( s >= lastjob ) 
     return 1;
aux = lostjobs;
while ( aux ) {
    if ( aux->deb == s ) {
	fprintf(stderr,"\nlost job %Ld selected\n", s);
        return 1;
    }
    aux = aux->next;
}
return 0;
}

int loadlost()
{
    FILE *src;
    lost aux;
    src = fopen(procname, "r");
    char line[1024], op[8], cp[128];
    ullong deb;
    int count  = 0;
    if (!src)
	return 0;
    fprintf(stderr, "looking for lost jobs\n");
    while (! feof(src) ) {
	line[0] = 0;
	fgets(line, 1024, src);
	switch (*line) {
        case '>':fprintf(stderr,"%s", line);
                 break;
	case 'R':
	    if (strncmp(line, "RUN", 3) == 0) {
		if (3 == sscanf(line, "%s %Ld:%s", op, &deb, cp)) {
		    fprintf(stderr,"step %Ld started on %s is lost\n", deb, cp);
		    aux = (lost) malloc(sizeof(enrlost));
		    aux->deb = deb;
		    aux->next = lostjobs;
		    lostjobs  = aux;
                    count++;
		};
	    };
	    break;
	case 'i':
	    sscanf(line, "\niter=%Ld", &lastjob);
	    break;
	}
       }
    fclose(src);
    fprintf( stderr, "\n%d jobs were lost\nloop restart after %Ld", count, lastjob);
    return 1;
    }


    void report(void) {
        float factor;
        time_t finaltime;
        ullong runtime;
	FILE *src;
	char line[1024], cp[128];
	listlog lr = NULL, aux, auy;
	data *tmp;
	ullong wkf = 0, cpu = 0, deb;
	struct in_addr mac;
	int qte = 0;
	float perf;
	struct hostent *info;
	FILE *dst;
        finaltime = time( NULL );
        runtime = difftime(finaltime, initial);
	src = fopen(infoname, "r");
	while (!feof(src)) {
	    line[0] = 0;
	    fgets(line, 1024, src);
	    if (4 == sscanf(line, "%Ld:%Ld:%Ld:%s", &deb, &wkf, &cpu, cp)) {
		inet_aton(cp, &mac);
		aux = lr;
		while (aux && aux->ptr->mac.s_addr != mac.s_addr)
		    aux = aux->next;
		if (! aux ) {
		    aux = (listlog) malloc(sizeof(enrlog));
                    aux->ptr = (data*) malloc(sizeof(data));
		    aux->ptr->mac = mac;
		    aux->ptr->qte = 1;
		    aux->ptr->wkf = wkf;
		    aux->ptr->cpu = cpu;
		    aux->next = lr;
		    lr = aux;
		} else {
		    aux->ptr->qte++;
		    aux->ptr->wkf += wkf;
		    aux->ptr->cpu += cpu;
		}
	    }
	}
	fclose(src);
	wkf = 0;
	cpu = 0;
        aux = lr;
	while (aux) {
	    cpu += aux->ptr->cpu;
	    wkf += aux->ptr->wkf;
	    qte += aux->ptr->qte;
	    aux = aux->next;
	}
        factor = wkf;
        factor /= cpu;
	aux = lr;
	while (aux) {
	    aux->ptr->perf  = aux->ptr->cpu;
            aux->ptr->perf *= factor;
	    aux->ptr->perf /= aux->ptr->wkf;
	    aux = aux->next;
	}
	aux = lr;
	while (aux) {
	    auy = aux->next;
	    while (auy) {
		if (aux->ptr->perf > auy->ptr->perf) {
		    tmp = aux->ptr;
	            aux->ptr = auy->ptr;
                    auy->ptr = tmp;
		}
		auy = auy->next;
	    }
	    aux = aux->next;
	}
	dst = fopen(outname, "a");
        fprintf(dst, "\ndeparture     : %s", ctime( &initial) );
        fprintf(dst, "\narrival       : %s", ctime( &finaltime));
        fprintf(dst, "\nelapse time   : %Ld", runtime);
	fprintf(dst, "\ncpu time      : %Ld", cpu);
	fprintf(dst, "\njob count     : %d", qte);
	fprintf(dst, "\nprocessors    : %d", proc);
	fprintf(dst, "\nwork factor   : %Ld", wkf);
	perf  = cpu;
        perf  *=factor;
	perf /= wkf;
	fprintf(dst, "\nsec / wkf     : %.4f", perf);
	fprintf(dst, "\n%8s %13s%24s%8s%5s%4s","perf ms","ip","wkf","cpu","job", "cpu");
	while (lr) {
	    fprintf(dst, "\n%8.3f %13s%24Ld%8Ld%5d%4Ld", lr->ptr->perf,
		    inet_ntoa(lr->ptr->mac), lr->ptr->wkf, lr->ptr->cpu, lr->ptr->qte, (100*lr->ptr->cpu)/cpu);
	    info = gethostbyaddr(&(lr->ptr->mac), 4, AF_INET);
	    fprintf(dst, " %s", info->h_name);
	    lr = lr->next;
	}
        fprintf(dst, "\n");
	fclose(dst);
    }

  

    int main(int argc, char *argv[]) {
	ullong s, f;
	
	if (! bigloopargs(argc, argv) )
	    exit(1);

	initbigloop();
	initserver();

	bigloopparms();
        score = BEST;
	sprintf(outname,  "log/output-%d.log", IDENT);
	sprintf(infoname, "log/info-%d.log", IDENT);
	sprintf(procname, "log/proc-%d.log", IDENT);
	initproc();
	loadlost();
	s    = FIRST;
	while (! acceptstep(s) ) s += STEP;
	while (s < LAST || actif) {
	    if (getticketfromclient(&current)) {
		pticket(stderr, current);
		fflush(stderr);
		switch (current.op) {
		case READY:
		    openproc();
		    sendpid(proc);
		    proc++;
		    break;
		case GET:
		    if (s < LAST) {
			f = s + STEP;
			if (f > LAST)
			    f = LAST;
			initjob(s, f);
			sendjob(s, f, score);
			s += STEP;
			while ( ! acceptstep(s)) s += STEP;
		    } else {
			sendstop();
			actif--;
		    }
		    break;
		case VALUE:
		    savevalue();
		    break;
                case SCORE:
		    savescore();
		    break;
		case END:
		    savejob();
		    break;
		}
		fprintf(stderr, "\nstep=%d/%Ld actifs : %d", jobcount,
			NBSTEP, actif);
		logprocess( s );
	    }
	}
	fprintf(stderr, "\nwaiting for zombies...");
	fflush(stderr);

    while (getticketfromzombie(&current)) {
	pticket(stderr, current);
	sendstop();
    }

	
        
	report();
	printf("\neoj\n");
	stopserver();
	return 0;
    }
