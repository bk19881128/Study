/*
* when define _LARGEFILE64_SOURCE
* All the 32 bit functions and data types will be automatically converted into the 64-bit version
* for example, open() is converted open64(), off_t is converted to off64_t
*/
#define _LARGEFILE64_SOURCE
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    int fd;
    off_t off;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname offset\n", argv[0]);

    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open32");

    off = atoll(argv[2]);
    if (lseek(fd, off, SEEK_SET) == -1)
        errExit("lseek32");

    if (write(fd, "test", 4) == -1)
        errExit("write");
    exit(EXIT_SUCCESS);
}
