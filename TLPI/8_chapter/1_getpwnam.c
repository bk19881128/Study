#include <pwd.h>
#include "tlpi_hdr.h"

//strange!!!! we see the uids are not the same???
int
main(int argc, char *argv[])
{
    printf("%ld %ld\n", (long) (getpwnam("kubao")->pw_uid),
			(long) (getpwnam("erjung")->pw_uid));

    exit(EXIT_SUCCESS);
}

