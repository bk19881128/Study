#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char* argv[]) {

    int fd;
    Boolean APPEND = FALSE;
    size_t byte;
    ssize_t numWritten;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s filename num-bytes [x]\n",
                 argv[0]);

    if (argc == 4 && strcmp(argv[3], "x") == 0)
	APPEND = TRUE;

    byte = atoi(argv[2]);

    if (APPEND == FALSE)
        fd = open(argv[1], O_WRONLY | O_CREAT,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                    S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    else
        fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                    S_IROTH | S_IWOTH);                     /* rw-rw-rw- */

    if (fd == -1)
        errExit("open");

    char s = 'x';
    for (size_t i = 0; i < byte; i++) {
	if (APPEND == FALSE)
	    lseek(fd, 0, SEEK_END);
        numWritten = write(fd, &s, 1);
        if (numWritten == -1)
            errExit("write");

    }

    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}


