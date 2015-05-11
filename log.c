/*
 * $Id: log.c,v 1.1 1998/05/17 15:24:54 elkner Exp $
 *
 * Author:  Jens Elkner  elkner@ivs.cs.uni-magdeburg.de
 * Project: Jesred       http://ivs.cs.uni-magdeburg.de/~elkner/webtools/jesred/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * http://www.gnu.org/copyleft/gpl.html or ./gpl.html
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include<stdlib.h>
#include<stdarg.h>
#include<stdio.h>
#include<sys/time.h>

#include "main.h"
#include "util.h"
#include "log.h"

FILE *openFile(char *);

void
openLogs(char **redirect, char **rewrite) 
{
    if ( *rewrite ) {
	fd_rewrite = openFile(*rewrite);
	safe_free(*rewrite);
    }
    else
	fd_rewrite = NULL;
    if ( *redirect ) {
	fd_redirect = openFile(*redirect);
	safe_free(*redirect);
    }
    else
	fd_redirect = NULL;
}


FILE *
openFile(char *file) 
{
    FILE *fd;
    
    if ((fd = fopen(file, "a+")) == NULL) {
	fprintf(stderr,"Warning: Can't open file %s for writing! \n"
		"Skipping all related messages!\n",file);
    }
    return fd;
}

void
log(log_code c, char *format, ...) {
    FILE *fd;
    
    char msg[BUFSIZE];
    va_list args;
    struct timeval current_time;
    
    va_start(args, format);
    if(vsprintf(msg, format, args) > (BUFSIZE - 1)) {
	/* string is longer than the maximum buffer we specified,
	   so just return */
	return;
    }
    va_end(args);
    
    if(interactive) {
	gettimeofday(&current_time, NULL);
	fprintf(stderr, "%d.%03d %s", (int) current_time.tv_sec,
		(int) current_time.tv_usec / 1000, msg);
	fflush(stderr);
	return;
    }
    if (c == MATCH ) {
	if ( ! fd_rewrite)
	    return;
	fd = fd_rewrite;
    }
    else if ( c == ERROR || c == INFO ) {
	if (! fd_redirect)
	    return;
	fd = fd_redirect;
    }
#ifdef DEBUG
    else if ( c == DEBG ) {
	if (! fd_redirect )
	    return;
	fd = fd_redirect;
    }
#endif
    else
	return;
    gettimeofday(&current_time, NULL);
    fprintf(fd, "%d.%03d %s", (int) current_time.tv_sec,
	    (int) current_time.tv_usec / 1000, msg);
    fflush(fd); /* should we really do that all the time ??? */
}

void closeLogs(void) 
{
    if ( fd_rewrite)
	fclose(fd_rewrite);
    if ( fd_redirect)
	fclose(fd_redirect);
}
