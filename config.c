/*
 * $Id: config.c,v 1.1 1998/05/17 23:48:09 elkner Exp $
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

#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#ifdef LOCAL_REGEX
#include "regex.h"
#else
#include<regex.h>
#endif

#include "log.h"
#include "path.h"
#include "main.h"
#include "pattern_list.h"
#include "config.h"
#include "util.h"

char *CleanLine(const char *);

const char *const w_space = " \t\n\r";


void
read_config(char **rules, char **redirect, char **rewrite) 
{
    FILE *fd;
    char *token, *token2;
    char buff[BUFSIZE];

    echo_mode = 0;
    allow_siblings = 0;
    strcpy(buff,DEFAULT_PATH);
    strcat(buff,"/");
    strcat(buff,"jesred.conf");
    if ( (fd=fopen(buff,"r")) == NULL) {
	token = strerror(errno);
	fprintf(stderr,"Can't open config file %s for reading: %s\n"
		"Using echo mode!!!\n",
		buff, token);
	echo_mode++;
	return;
    }
    /* just to be sure */
    
    safe_free(*rules);
    safe_free(*redirect);
    safe_free(*rewrite);
    while (fgets(buff, BUFSIZE, fd) != NULL) {
	token = CleanLine(buff);
	if ( token == NULL )
	    continue;
	token2 = strchr(token,'=');
	if (token2) {
	    *token2 = '\0';
	    token = CleanLine(token);
	    if (! token)
		continue;
	    token2++;
	    token2 = CleanLine(token2);
	    if (! token2)
		continue;
	    if ( ! strcasecmp(token,"rules"))
		*rules = strdup(token2);
	    else if (! strcasecmp(token,"redirect_log"))
		*redirect = strdup(token2);
	    else if (! strcasecmp(token,"rewrite_log"))
		*rewrite = strdup(token2);
#ifdef DEBUG	    
	    else if ( (! strcasecmp(token,"debug")) &&
		      (! strcasecmp(token2,"true")) )
		debug_mode++;
#endif	    
	    else if (! strcasecmp(token,"siblings")) {
		if (! strcasecmp(token2,"true"))
		    allow_siblings++;
	    }
	    else
		fprintf(stderr,
			"Unknown keyword or value \"%s = %s\" - ignored\n",
			token ? token : "", token2 ? token2 : "");
	}
    }
    fclose(fd);
}

/* attention - it changes the original string */
char *
CleanLine(const char *line) {
    char *token = NULL;
    int i;

    token = (char *) line;
    if (token[strlen(line)-1] == '\n')
	token[strlen(line)-1] = '\0';
    /* skip leading and trailing white space of the line */
    token += strspn(line, w_space);
    if ( *token == '#' ) {
        return (NULL);
    }
    for(i= strlen(token) -1 ; i >= 0; i--) {
	if (token[i] != ' ')
	    break;
	else
	    token[i] = '\0';
    }
    if ( *token == '\0' ) {
        return (NULL);
    }
    return (token);
}

/* load the squirm.patterns into linked list */

void
read_rules(char **file, pattern_item **plist) {
    char buff[BUFSIZE];
    FILE *fd;
    char *token = NULL;
    
    if ( ! *file || (fd = fopen(*file, "rt")) == NULL ) {
	echo_mode = 1;
	token = strerror(errno);
	log(ERROR, "unable to open redirect patterns file %s: %s\n",
	    *file ? *file : "", token);
	return;
    }
    log(INFO, "Reading Patterns from config %s\n", *file);
    
    while(!echo_mode && (fgets(buff, BUFSIZE, fd) != NULL)) {
	
	/* skip blank lines and comments */
	token = CleanLine(buff);
	if ( ! token)
	    continue;
	add_to_patterns(token, plist);
    }
    fclose(fd);
    safe_free(*file);
}
