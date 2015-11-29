#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_READ 20

int main(int argc, char* argv[]) {

    int fd1, fd2;
    ssize_t numWritten, numRead;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s originalFile copiedFile\n",
                 argv[0]);

    fd1 = open(argv[1], O_RDONLY,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd1 == -1)
        errExit("open originalFile");

    fd2 = creat(argv[2],
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd2 == -1)
        errExit("open copiedFile");

    char buffer[MAX_READ];

    while((numRead = read(fd1, buffer, MAX_READ)) != -1) {
	if((numWritten = write(fd2, buffer, numRead)) != numRead)
		errExit("write");
	if(numRead < MAX_READ)
		break;
    }

    if (close(fd1) == -1)
        errExit("close");

    if (close(fd2) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}
