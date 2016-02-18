/******************************************************************************
**                                                                           **
**                             ALCATEL TELECOM                               **
**                                                                           **
******************************************************************************/

/*************************** COPYRIGHT INFORMATION ****************************
**                                                                           **
** This program contains proprietary information which is a trade            **
** secret of ALCATEL TELECOM and also is protected as an unpublished         **
** work under applicable Copyright laws. Recipient is to retain this         **
** program in confidence and is not permitted to use or make copies          **
** thereof other than as permitted in a written agreement with               **
** ALCATEL TELECOM.                                                          **
**                                                                           **
******************************************************************************/

/****************************** IDENTIFICATION ********************************
**
** Project          : FPXT-B
**
** Module           : DEBUG SHELL
**
** File name        : xtty.c
**
** Description      : Redirect the input/output to a pts device file.
**
** Reference(s)     :
**
** Contact          : Guolin.Pan@alcatel-sbell.com.cn
**
** Creation date    : 2013-08-24
**
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>
#include "xtty.h"

#define MAX_HISTORY_NUM        32
#define MAX_INPUT_BUFF_SIZE    512
#define MAX_OUTPUT_BUFF_SIZE   1024
#define MAX_PREFIX_LEN         32

#define KEY_BACKSPACE          0x08
#define KEY_DELETE             0x7F
#define KEY_ENTER              0x0D
#define KEY_NEW_LINE           0x0A
#define KEY_ESC                0x1B
#define KEY_LEFT_BRACKETS      0x5B  /*[*/
#define KEY_UP_OR_A            0x41  /*ESC+[+A */
#define KEY_DOWN_OR_B          0x42  /*ESC+[+B */


struct history {
	int enable;
	int current;
	int index;
	char *list[MAX_HISTORY_NUM];
};

struct priv_data {
	char prefix[MAX_PREFIX_LEN+1];
	char input[MAX_INPUT_BUFF_SIZE+1];
	char output[MAX_OUTPUT_BUFF_SIZE+1];
	struct history h;
};

struct xtty_info {
	int fd;
	char *name;
	void *priv;
};

static int __xtty_create(char *name)
{
	int ptmx_fd = -1;
	int pts_fd = -1;
	char *pts_name = NULL;

	struct termios orig_termios;

	ptmx_fd = open("/dev/ptmx", O_RDWR);

	if (ptmx_fd < 0)
		return  -1;

	pts_name = (char *)ptsname(ptmx_fd);
	if (NULL == pts_name) {
		close(ptmx_fd);
		return  -1;
	}

	grantpt(ptmx_fd);
	unlockpt(ptmx_fd);

	pts_fd = open(pts_name, O_RDWR);
	if (pts_fd < 0)
		return  -1;

	/* tell fd which process to signal */
	if (fcntl(ptmx_fd, F_SETOWN, getpid()) < 0) {
		perror("\n\rF_SETOWN failed.");
		close(ptmx_fd);
		close(pts_fd);

		return  -1;
	}

	if (tcgetattr(ptmx_fd, &orig_termios) < 0) {
		perror("\n\rgetting current terminal's attributes failed.");
		close(ptmx_fd);
		close(pts_fd);

		return  -1;
	}

	/* Set raw mode on tty */
	cfmakeraw(&orig_termios);

	if (tcsetattr(ptmx_fd, TCSANOW, &orig_termios) < 0) {
		perror("\n\rsetting current terminal's attributes failed.");
		close(ptmx_fd);
		close(pts_fd);

		return  -1;
	}

	unlink(name);
	if (symlink(pts_name, name) < 0) {
		perror("\n\rsymlink failed.");
		close(ptmx_fd);
		close(pts_fd);

		return  -1;
	}

	fprintf(stderr, "\n\r%s -> %s\n\r", name, pts_name);

	return ptmx_fd;
}

static int __is_printable_char(char c)
{
	return ((c >= 0x20) && (c <= 0x7E));
}

static void __xtty_print_prefix(struct xtty_info *xtty)
{
	int w_len = 0;
	char *p = xtty_get_prefix(xtty);
	if(p) {
		w_len = write(xtty->fd,p,strlen(p));
	} else {
		w_len = write(xtty->fd,">",1);
	}

	if(w_len < 0)
		perror("\n\r __xtty_print_prefix:write failed.");
}

static void __xtty_print_crlf(struct xtty_info *xtty)
{
	int w_len = 0;

	w_len = write(xtty->fd,"\n\r",2);
	if(w_len < 0)
		perror("\n\r __xtty_print_prefix:write failed.");
}

static void __xtty_remove_char(struct xtty_info *xtty,int num)
{
	int w_len = 0;

	while(num-->0)
	{
		w_len = write(xtty->fd,"\b \b",3);
		if(w_len < 0)
			perror("\n\r __xtty_print_prefix:write failed.");	
	}
}

static int __xtty_is_enable_history(struct xtty_info *xtty)
{
	struct history *h = &((struct priv_data *)(xtty->priv))->h;
	return h->enable;
}

static void __xtty_history_add(struct xtty_info *xtty, char *input)
{
	struct history *h = &((struct priv_data *)(xtty->priv))->h;
	int len = strlen(input);
	char *t = (char *)malloc(len+1);

	if(t) {
		strncpy(t,input,len);
		t[len] = '\0';

		if(!h->list[h->current])
			free(h->list[h->current]);
		h->list[h->current] = t;
		h->index = h->current = (h->current + 1) % MAX_HISTORY_NUM;
	}
}

static char * __xtty_history_prev(struct xtty_info *xtty)
{
	struct history *h = &((struct priv_data *)(xtty->priv))->h;
	int t = (h->index + MAX_HISTORY_NUM - 1) % MAX_HISTORY_NUM;
	char *cmd = NULL;

	if(h->list[t]) {
		h->index = t;
		cmd = h->list[t];
	}
	return cmd;
}

static char * __xtty_history_next(struct xtty_info *xtty)
{
	struct history *h = &((struct priv_data *)(xtty->priv))->h;
	char *cmd = NULL;

	if(h->index != h->current) {
		h->index = (h->index + 1) % MAX_HISTORY_NUM;
		cmd = h->list[h->index];
	}
	return cmd;
}

static void __xtty_history_clear(struct xtty_info *xtty)
{
	struct history *h = &((struct priv_data *)(xtty->priv))->h;
	int i;

	for(i=0; i<MAX_HISTORY_NUM; i++) {
		if(h->list[i])
			free(h->list[i]);
		h->list[i] = NULL;
	}
	h->current = h->index = 0;
}

struct xtty_info * xtty_create(char *name)
{
	struct xtty_info * xtty;
	int fd;
	int len;

	if(!name)
		goto fail3;

	if((fd = __xtty_create(name)) < 0)
		goto fail3;

	xtty = (struct xtty_info *)malloc(sizeof(struct xtty_info));
	if(!xtty) {
		goto fail2;
	}
	xtty->fd = fd;

	len = strlen(name);
	xtty->name = (char *)malloc(len + 1);
	if(!xtty->name) {
		goto fail1;
	} else {
		strncpy(xtty->name,name,len);
		xtty->name[len] = '\0';
	}

	xtty->priv = (void *)malloc(sizeof(struct priv_data));
	if(!xtty->priv) {
		goto fail0;
	} else {
		memset(xtty->priv,0,sizeof(struct priv_data));
		xtty_set_prefix(xtty,">");
		xtty_history(xtty,1);
	}

	return xtty;

fail0:
	free(xtty->name);
fail1:
	free(xtty);
fail2:
	close(fd);
fail3:
	return NULL;
}

int xtty_write(struct xtty_info *xtty, char *buff, int size)
{
	if((!xtty) || (!buff) || (size == 0))
		return -1;
	return write(xtty->fd,buff,size);
}

int xtty_read(struct xtty_info *xtty, char *buff, int size)
{
	if((!xtty) || (!buff) || (size == 0))
		return -1;
	return read(xtty->fd,buff,size);
}

int xtty_printf(struct xtty_info *xtty, const char *fmt, ...)
{
	int i,len,size;
	int w_len = 0;
	char c;
	char *buff;
	va_list args;

	if(!xtty)
		return -1;

	buff = ((struct priv_data *)(xtty->priv))->output;
	va_start(args,fmt);
	size = vsprintf(buff,fmt,args);
	va_end(args);

	for(i=0; i<size; i++) {
		if((c = buff[i]) == '\n')
			len = write(xtty->fd,"\n\r",2);
		else
			len = write(xtty->fd,&c,1);
		if(len <= 0)
			break;
		w_len += len;
	}
	return w_len;
}

int xtty_read_line(struct xtty_info *xtty, char *buff, int size)
{
	int len;
	int r_len = 0;
	int w_len = 0;
	int finish = 0;
	int esc_pressed = 0;
	char c;
	char *input = NULL;

	if((!xtty) || (!buff) || (size == 0))
		return -1;

	input = ((struct priv_data *)(xtty->priv))->input;

    usleep(50);

	__xtty_print_crlf(xtty);
	__xtty_print_prefix(xtty);

	do {
		if(r_len >= MAX_INPUT_BUFF_SIZE)
			break;

		len = read(xtty->fd,&c,1);
		if(len < 0)
			break;

		switch(c) {
		case KEY_ENTER :
		case KEY_NEW_LINE :
			esc_pressed = 0;
			if(r_len > 0) {
				finish = 1;
			} else {
				__xtty_print_crlf(xtty);
				__xtty_print_prefix(xtty);
			}
			break;

		case KEY_BACKSPACE :
		case KEY_DELETE :
			esc_pressed = 0;
			if(r_len > 0) {
				r_len--;
				__xtty_remove_char(xtty,1);
			}
			break;

		case KEY_ESC :
			esc_pressed = 1;
			break;

		case KEY_LEFT_BRACKETS :
			if(!esc_pressed) {
				input[r_len++] = c;
				w_len = write(xtty->fd,&c,1);
				if(w_len < 0)		
				perror("\n\r xtty_read_line:write failed.");
				
			}
			break;

		case KEY_UP_OR_A :
			if(!esc_pressed) {
				input[r_len++] = c;
				w_len = write(xtty->fd,&c,1);
				if(w_len < 0)		
				perror("\n\r xtty_read_line:write failed.");
				
			} else {
				if(__xtty_is_enable_history(xtty)) {
					char *p = __xtty_history_prev(xtty);
					if(p) {
						__xtty_remove_char(xtty,r_len);
						r_len = strlen(p);
						strncpy(input,p,r_len);
						w_len = write(xtty->fd,p,r_len);
						if(w_len < 0)		
						perror("\n\r xtty_read_line:write failed.");
					}
				}
			}
			break;

		case KEY_DOWN_OR_B :
			if(!esc_pressed) {
				input[r_len++] = c;
				w_len = write(xtty->fd,&c,1);
				if(w_len < 0)		
				perror("\n\r xtty_read_line:write failed.");
			} else {
				if(__xtty_is_enable_history(xtty)) {
					char *p = __xtty_history_next(xtty);
					__xtty_remove_char(xtty,r_len);
					r_len = 0;
					if(p) {
						r_len = strlen(p);
						strncpy(input,p,r_len);
						w_len = write(xtty->fd,p,r_len);
						if(w_len < 0)		
						perror("\n\r xtty_read_line:write failed.");
					}
				}
			}
			break;

		default :
			if(esc_pressed) {
				esc_pressed = 0;
				break;
			}

			if(__is_printable_char(c)) {
				input[r_len++] = c;
				w_len = write(xtty->fd,&c,1);
				if(w_len < 0)		
				perror("\n\r xtty_read_line:write failed.");
				
				continue;
			}
		}

	} while((!finish) && (r_len<MAX_INPUT_BUFF_SIZE));

	input[r_len++] = buff[size-1] = '\0';
	len = size > r_len ? r_len : size-1;
	strncpy(buff,input,len);

	if(__xtty_is_enable_history(xtty))
		__xtty_history_add(xtty,input);

	__xtty_print_crlf(xtty);

	return len;
}

void xtty_destroy(struct xtty_info *xtty)
{
	if(!xtty)
		return;
	if(!xtty->name)
		free(xtty->name);
	if(!xtty->priv)
		free(xtty->priv);
    if (xtty->fd != -1)
	    close(xtty->fd);
	free(xtty);
}

int xtty_history(struct xtty_info *xtty,int enable)
{
	int ret = -1;
	struct history *h;
	if(!xtty)
		return ret;

	h = &((struct priv_data *)(xtty->priv))->h;
	if(0 == enable) {
		if(h->enable == 1) {
			__xtty_history_clear(xtty);
			h->enable = 0;
			ret = 0;
		}
	} else if(1 == enable) {
		if(h->enable == 0) {
			__xtty_history_clear(xtty);
			h->enable = 1;
			ret = 0;
		}
	} else {
		;
	}
	return ret;
}

int xtty_set_prefix(struct xtty_info *xtty,char * prefix)
{
	char *p;
	int len;

	if((!xtty) || (!prefix))
		return -1;

	len = strlen(prefix);
	if(len > MAX_PREFIX_LEN)
		len = MAX_PREFIX_LEN;

	p = ((struct priv_data *)(xtty->priv))->prefix;
	strncpy(p,prefix,len);
	p[len] = '\0';

	return 0;

}

char * xtty_get_prefix(struct xtty_info *xtty)
{
	if(!xtty)
		return NULL;
	return ((struct priv_data *)(xtty->priv))->prefix;
}

char * xtty_get_name(struct xtty_info *xtty)
{
	if(!xtty)
		return NULL;
	return xtty->name;
}

int xtty_get_fd(struct xtty_info *xtty)
{
	if(!xtty)
		return -1;
	return xtty->fd;
}


#define STDOUT 1

int fd_tty,fd_std;

void redirect_initStd(void)
{
    fd_std=dup(STDOUT);
}

void redirect_stdout(int fd)
{
    fflush(stdout);
    //fd_tty=open(tty_name,O_CREAT|O_RDONLY);
    //fd_tty = open(tty_name,(O_RDWR), 0644);
    //fd_tty = open((char *)ptsname(pon_mngt_xtty->fd),(O_RDWR), 0644);
    fd_tty = fd;

    if(-1 == fd_tty)
    {
        printf("Err. can't open file");
        return;
    }
    dup2(fd_tty,STDOUT);

    fflush(stdout);
    //close(fd_tty);
    //close(fd_std);
}

void resume_stdout()
{
    fflush(stdout);
    dup2(fd_std,STDOUT);

     fflush(stdout);
    //close(fd_std);
    //close(fd_tty);
}

