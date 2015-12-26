#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int my_dup(int oldfd);
int my_dup2(int oldfd, int newfd);

int my_dup(int oldfd) {
    int newfd = fcntl(oldfd, F_DUPFD, 0);
    return newfd;
}

int my_dup2(int oldfd, int newfd) {
    if (fcntl(oldfd, F_GETFL) == -1)
	return -1;
    if (oldfd == newfd)
	return newfd;
    else {
	close(newfd);
        newfd = fcntl(oldfd, F_DUPFD, newfd);
        return newfd;
    }    
}

int main(int argc, char* argv[]) {

    int fd;

    fd = creat("4_test_file",
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
               S_IROTH | S_IWOTH);                     /* rw-rw-rw- */

    printf("the new create file fd is %d\n", fd);

    printf("use my_dup get a new fd is %d\n", my_dup(fd));

    printf("use my_dup2 get a new fd(100) is %d\n", my_dup2(fd, 100));

    printf("use dup get a new fd is %d\n", dup(fd));

    printf("use dup2 get a new fd(100) is %d\n", dup2(fd, 100));

    if (close(fd) == -1)
        errExit("close");

    remove("4_test_file");

    exit(EXIT_SUCCESS);
}


