#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_READ 1000

int main(int argc, char* argv[]) {

    Boolean APPEND = FALSE;
    int fd, opt;
    ssize_t numWritten, numRead;
    mode_t modeFile = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
        case 'a':
            APPEND = TRUE;
            break;

        default:
            usageErr("%s [-a] file\n"
                     "        -a    append mode\n", argv[0]);
        }
    }

    if (argc != optind + 1)
        usageErr("%s [-a] file\n", argv[0]);

    if (APPEND == FALSE)
	fd = creat(argv[optind], modeFile);
    else
	fd = open(argv[optind], O_WRONLY | O_APPEND, modeFile);

    char buf[MAX_READ + 1];
    numRead = read(STDIN_FILENO, buf, MAX_READ);
    if(numRead == -1)
	errExit("read");

    buf[numRead] = '\0';

    numWritten = write(fd, buf, numRead);
    if(numWritten != numRead)
	errExit("write");

/*
// for test
    char buf[MAX_READ + 1];

    while(read(STDIN_FILENO, buf, 1) != -1) {
	if( buf[0] == EOF )
	    break;
	else {
	    numWritten = write(fd, buf, 1);
            if(numWritten != 1)
                errExit("write");
	}
    }
*/

    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}
