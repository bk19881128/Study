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
** File name        : xtty.h
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

#ifndef __XTTY_H__
#define __XTTY_H__

struct xtty_info;

struct xtty_info * xtty_create(char *name);
void xtty_destroy(struct xtty_info *xtty);

int xtty_read(struct xtty_info *xtty, char *buff, int size);
int xtty_write(struct xtty_info *xtty, char *buff, int size);
int xtty_read_line(struct xtty_info *xtty, char *buff, int size);
int xtty_printf(struct xtty_info *xtty, const char *fmt, ...);

/* enable =1/0, start/stop records the history inputting. */
int xtty_history(struct xtty_info *xtty,int enable);

int xtty_set_prefix(struct xtty_info *xtty,char * prefix);
char * xtty_get_prefix(struct xtty_info *xtty);

char * xtty_get_name(struct xtty_info *xtty);
int xtty_get_fd(struct xtty_info *xtty);

void redirect_stdout(int fd);
void resume_stdout();
void redirect_initStd(void);


#endif
