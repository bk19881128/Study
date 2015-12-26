#include <pwd.h>
#include "tlpi_hdr.h"

struct passwd *my_getpwnam(const char *name);

struct passwd *my_getpwnam(const char *name) {
    struct passwd *pwd;
    setpwent();
    while ((pwd = getpwent()) != NULL) {
	if(!strcmp(pwd->pw_name, name))
	    break;	
    }
    endpwent();
    return pwd;
}

//strange!!!! we see the uids are not the same???
int
main(int argc, char *argv[])
{
    printf("%ld %ld\n", (long) (my_getpwnam("kubao")->pw_uid),
			(long) (my_getpwnam("erjung")->pw_uid));

    exit(EXIT_SUCCESS);
}

