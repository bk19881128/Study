#include "tlpi_hdr.h"
#include <sys/times.h>
#include <time.h>
#include <math.h>

//strange!!!! we see the uids are not the same???
int
main(int argc, char *argv[])
{
    printf("CLOCKS_PER_SEC=%ld  sysconf(_SC_CLK_TCK)=%ld\n\n",
            (long) CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));

    unsigned long ticks = pow(2,32);
    printf("ticks %ld\n", ticks);
    printf("value return from times() will expire at %ld min %ld sec\n", ticks/CLOCKS_PER_SEC/60, ticks/CLOCKS_PER_SEC%60);
    printf("value return from clock() will expire at %ld days\n", ticks/sysconf(_SC_CLK_TCK)/(24*3600));

    exit(EXIT_SUCCESS);
}

