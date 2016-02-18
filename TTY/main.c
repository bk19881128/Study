#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdarg.h>
#include "xtty.h"
//#include "TkCli.h"
#define TTY_FILE_NAME "Test_tty"
#define gLocalBusTrace 1

struct xtty_info *test_xtty = NULL;
extern void TkCliTask(void);
int LocalBusPrint(const char *format, ...) 
{
        int ret = 0;
        va_list param;
        va_start(param, format);

        if (gLocalBusTrace)
        {
        /*required by UNIX sometimes to actually display*/
        ret = vprintf(format, param);
        (void)fflush(stdout);
        }

        va_end(param);

	return ret;
}

int main(int argc, char *const *argv)
{
	printf("This is test from jacob \n");
	const char print_char[] = "\n\r const--print to terminal from tty\n\r";
	//const char *p_char = &print_char;
//	struct xtty_info *test_xtty = NULL;

        test_xtty = xtty_create(TTY_FILE_NAME);

        if (test_xtty) {
                printf("name=%s,fd=%d\n\r", xtty_get_name(test_xtty), xtty_get_fd(test_xtty));
                xtty_set_prefix(test_xtty, "TekHost>");
                 //  TkMainStart(argc, argv);
		TkCliTask();		
        } else {
                   printf("create pon_mngt_xtty fail!\n\r");
        }


/*
	while(1)
	{
		LocalBusPrint("\n\rprint to terminal from tty\n\r"); 
		LocalBusPrint("%s",&print_char);
		sleep(5);
	}	
*/	
	
	xtty_destroy(test_xtty);

	return 0;
}
