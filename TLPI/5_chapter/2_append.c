#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char* argv[]) {

    int fd;
    ssize_t numWritten;

    if (argc != 2)
        usageErr("%s TestFile\n",
                 argv[0]);

    if (strcmp(argv[1], "--help") == 0)
        usageErr("%s TestFile\n",
                 argv[0]);

    printf("create a file %s\n", argv[1]);
    fd = creat(argv[1],
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd == -1)
        errExit("create fail");

    sleep(10);

    printf("write 123456789 to the file %s\n", argv[1]);
    char s1[10] = "123456789";
    numWritten = write(fd, s1, strlen(s1));
    if (numWritten == -1)
        errExit("write");

    sleep(10);

    printf("close the file %s, reopen it with the flag O_APPEND, change the offset at begining\n", argv[1]);
    if (close(fd) == -1)
        errExit("close");
    fd = open(argv[1], O_WRONLY | O_APPEND, 
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd == -1)
        errExit("open fail");
    lseek(fd, 0, SEEK_SET);

    sleep(10);

    printf("write abcdefghi to the file %s\n", argv[1]);
    char s2[10] = "abcdefghi";
    numWritten = write(fd, s2, strlen(s2));
    if (numWritten == -1)
        errExit("write");

    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}


