#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char* argv[]) {

    size_t holeLength;
    int fd;
    ssize_t numWritten;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file {holeLength}... holelength is set to 10000 by default\n",
                 argv[0]);

    fd = open(argv[1], O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd == -1)
        errExit("open");

    if (argc == 3) 
	holeLength = getLong(&argv[2][0], GN_ANY_BASE, argv[2]);
    else
	holeLength = 10000;


    char begin[10] = "testbegin";
    char end[10] = "testend";
    numWritten = write(fd, begin, strlen(begin));
    if (numWritten == -1)
	errExit("write");

    lseek(fd, holeLength, SEEK_CUR);
    numWritten = write(fd, end, strlen(end));
    if (numWritten == -1)
        errExit("write");

    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}
