/*
 * 
 * Modified: Ulises Nicolini by Vianet 2015/05/11
 * email: developers@sawcache.com
 * web:http://sawcache.com
 * Version: 3.0
 * 
 * 
 * $Id: main.c,v 1.3 1998/08/15 00:00:50 elkner Exp $
 *
 * Author:  Squirm derived       http://www.senet.com.au/squirm/
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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/signal.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#ifdef LOCAL_REGEX
#include "regex.h"
#else
#include<regex.h>
#endif

#include "main.h"
#include "pattern_list.h"
#include "config.h"
#include "util.h"
#include "log.h"
#include "version.h"
#include "rewrite.h"

static void Usage (void);
static void GetOptions(int argc, char *argv[]);

int echo_mode = 0;
int sig_hup = 0;
int interactive = 0;
int allow_siblings = 0;
#ifdef DEBUG
int debug_mode = 0;
#endif
FILE *fd_redirect = NULL;
FILE *fd_rewrite = NULL;


int main(int argc, char **argv)
{
    char *f_allow = NULL;
    char *f_rules = NULL;
    char *f_rewrite = NULL;
    char *f_redirect = NULL;


    int first_run = 1;
    char buff[BUFSIZE];
    char redirect_url[BUFSIZE];
    char *url,  *ident, *method, *ch_id;
    int finished = 0;
    int buff_status = 0;
   
    pattern_item *pattern_list = NULL;
    
    /* go into interactive mode if we're run as root */
    if((int)getuid() == 0) {
	interactive = 1;
	fprintf(stderr, "%s running as UID 0: writing logs to stderr\n",APPNAME);
    }
    signal(SIGHUP, HUPhandler);
    signal(SIGKILL, KILLhandler);
    GetOptions(argc, argv);

   
	int val;
	
	sig_hup = 0;
	log(INFO, "Freeing up old linked lists\n");
	
	plist_destroy(&pattern_list);
	closeLogs();
	
	/* read configuration file */
	read_config( &f_rules, &f_redirect, &f_rewrite);
	
	if (! interactive)
	    openLogs(&f_redirect, &f_rewrite);
	
	/* read rewrite rules */
	read_rules(&f_rules, &pattern_list);
	
	
	if(echo_mode)
	    log(ERROR, "Invalid condition - continuing in ECHO mode\n");
	    log(INFO, "%s (PID %d) started\n", APPNAME, (int)getpid());
	
	 while(1){
	   
	   /* read standard input until EOF */
	   if (!fgets(buff, BUFSIZE, stdin) || feof(stdin)){
		/* break loop if EOF */
                break;
            }

	    if(echo_mode) {
		puts("");
		fflush(stdout);
		continue;
	    }
	    
	    /* log the string passed to satandard input in DEBUG mode*/
	    log(DEBG,"PID %d  Input String: %s",(int)getpid(),buff);

	     
	    /* separate the four fields from the single input line of stdin */
	    buff_status = parse_buff(buff, &ch_id, &url);
	    
	    /* error during parsing the passed line from squid - no rewrite */
	    if(buff_status) {
		printf("%s ERR\n",ch_id);
		fflush(stdout);
		continue;
	    }
	    /* check echo_mode again (sighup)*/
	    if(echo_mode) {
		puts("");
		fflush(stdout);
		log(ERROR, "Invalid condition - continuing in ECHO mode\n");
		continue;
	    }
	    /* find a rule for rewriting the URL */
	    val = pattern_compare(url, redirect_url, pattern_list);
	    if( val < 1 ) {
		/* no rule found = 0, or ABORT rule -N */
		printf("%s ERR\n",ch_id);
		fflush(stdout);
		continue;
	    }
	    else {
		/* redirect_url contains the replacement URL */
		if ( redirect_url[0] == '\0' ) {
		    /* regex[i] abort, i.e. empty replacement URL */
		    printf("%s ERR\n",ch_id);
		    fflush(stdout);
		}
		else {
		    printf("%s OK store-id=%s\n",ch_id,redirect_url) ;
		    fflush(stdout);
		    log(MATCH, "%s %s %d\n", url, redirect_url, val);
		    log(DEBG,"PID %d Output string: %s OK store-id=%s\n",(int)getpid(),ch_id,redirect_url) ;

		}
	    }
	}

    log(DEBG,"PID %d exit now\n",(int)getpid()) ;
    
    plist_destroy(&pattern_list);
    closeLogs();
    safe_free(f_rewrite);
    safe_free(f_redirect);
    return 0;
}

static void
GetOptions(int argc, char *argv[])
{
    extern char *optarg;
    extern int optind;
    int c;
    
    while ((c = getopt(argc, argv, "hv")) != -1) {
        switch (c) {
	    case 'h' :
		Usage();
		break;
            case 'v' :
                printf("%s %s  (C) by %s  (%s)\n%s\n",
                       APPNAME, VERSION, AUTHOR, AUTHOR_EMAIL, APPURL);
		exit(0);
            default:
                Usage();
                break;
	}
    }
}

static void
Usage(void)
{
    
    fprintf(stderr,
            "Usage: %s [-h] [-v]\n\n"
            "   -h         print this and exit\n"
            "   -v         show version and exit\n",
            APPNAME);
    exit(1);
}
