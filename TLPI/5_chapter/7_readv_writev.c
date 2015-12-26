//!!!!!have lots of problems.!!!!!!
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/uio.h>

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt);
ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt);

ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t read_length = 0;
    ssize_t whole_length = 0;
    for(int i = 0; i < iovcnt; i++) {
	read_length = read(fd, iov[i].iov_base, iov[i].iov_len);
	whole_length += read_length;
    }
    return whole_length;
}

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t write_length = 0;
    ssize_t whole_length = 0;
    for(int i = 0; i < iovcnt; i++) {
        write_length = write(fd, iov[i].iov_base, iov[i].iov_len);
        whole_length += write_length;
    }
    return whole_length;
}

int
main(int argc, char *argv[])
{
    int fd, fd1;
    struct iovec iov[3];
    struct stat myStruct;       /* First buffer */
    int x;                      /* Second buffer */
#define STR_SIZE 100
    char str[STR_SIZE];         /* Third buffer */
    ssize_t numRead, totRequired, numWrite;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s readFile writeFile\n", argv[0]);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        errExit("open");

    totRequired = 0;

    iov[0].iov_base = &myStruct;
    iov[0].iov_len = sizeof(struct stat);
    totRequired += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totRequired += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totRequired += iov[2].iov_len;

    numRead = my_readv(fd, iov, 3);
    if (numRead == -1)
        errExit("readv");

    if (numRead < totRequired)
        printf("Read fewer bytes than requested\n");

    printf("total bytes requested: %ld; bytes read: %ld\n",
            (long) totRequired, (long) numRead);

    fd1 = creat(argv[2], S_IRUSR | S_IWUSR);
    if (fd1 == -1)
        errExit("open");

    numWrite = my_writev(fd1, iov, 3);
    if (numWrite == -1)
        errExit("writev");

    printf("total bytes requested: %ld; bytes write: %ld\n",
            (long) totRequired, (long) numWrite);

    exit(EXIT_SUCCESS);
}

