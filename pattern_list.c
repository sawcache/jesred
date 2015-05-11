/*
 * 
 * Modified: Ulises Nicolini by Vianet 2015/05/11
 * email: developers@sawcache.com
 * web:http://sawcache.com
 * Version: 3.0
 * 
 * 
 * $Id: pattern_list.c,v 1.2 1998/07/25 02:32:45 elkner Exp $
 *
 * Author:  Squirm derived      http://www.senet.com.au/squirm/
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
 * Thanks to Chris Foote, chris@senet.com.au - not much to change here ;-)
 *
 */

#include<stdio.h>
#include<sys/types.h>

#ifdef LOCAL_REGEX
#include "regex.h"
#else
#include<regex.h>
#endif

#include "main.h"
#include "log.h"
#include "util.h"
#include "pattern_list.h"
#include <stdlib.h>
#include <string.h>

void add_to_plist(pattern_item, pattern_item **);
#ifdef USE_ACCEL
char * get_accel(char *, int *, int);
#endif

void
add_to_patterns(char *pattern, pattern_item **plist)
{
    char first[BUFSIZE];
    char second[BUFSIZE];
    char type[BUFSIZE];
#ifdef USE_ACCEL
    char accel[BUFSIZE];
#endif
    regex_t compiled;
    pattern_item rpattern;
    int abort_type = 0;
    int parenthesis;
    int stored;
    
    /*  The regex_flags that we use are:
        REG_EXTENDED 
        REG_NOSUB 
        REG_ICASE; */
    
    int regex_flags = REG_NOSUB;
    
    rpattern.type = NORMAL;
    rpattern.case_sensitive = 1;
#ifdef USE_ACCEL   
    stored = sscanf(pattern, "%s %s %s %s", type, first, second, accel);
#else
    stored = sscanf(pattern, "%s %s %s", type, first, second);
#endif
    if((stored < 2) || (stored > 4)) {
        log(ERROR, "unable to get a pair of patterns in add_to_patterns() "
            "for [%s]\n", pattern);
        echo_mode = 1;
        return;
    }
  
    if(stored == 2)
        strcpy(second, "");
    
    if(strcmp(type, "abort") == 0) {
        rpattern.type = ABORT;
        abort_type = 1;
    }
    
    
    
  
    if(strcmp(type, "regexi") == 0) {
        regex_flags |= REG_ICASE;
        rpattern.case_sensitive = 0;
    }
    
    if(!abort_type) {
        parenthesis = count_parenthesis (first);
        if (parenthesis < 0) {      
            /* The function returned an invalid result,
               indicating an invalid string */
            log (ERROR, "count_parenthesis() returned "
                 "left count did not match right count for line: [%s]\n",
                 pattern);
            echo_mode = 1;
            return;
        }
        else if (parenthesis > 0) {    
            regex_flags |= REG_EXTENDED;
            rpattern.type = EXTENDED;
            regex_flags ^= REG_NOSUB;
        }
    }
  
    if(regcomp(&compiled, first, regex_flags)) {
        log(ERROR, "Invalid regex [%s] in pattern file\n", first);
        echo_mode = 1;
        return;
    }
    rpattern.cpattern = compiled;
    rpattern.pattern = (char *)malloc(sizeof(char) * (strlen(first) +1));
    if(rpattern.pattern == NULL) {
        log(ERROR, "unable to allocate memory in add_to_patterns()\n");
        echo_mode = 1;
        return;
    }
    strcpy(rpattern.pattern, first);
    rpattern.replacement = (char *)malloc(sizeof(char) * (strlen(second) +1));
    if(rpattern.replacement == NULL) {
        log(ERROR, "unable to allocate memory in add_to_patterns()\n");
        echo_mode = 1;
        return;
    }
    strcpy(rpattern.replacement, second);

#ifdef USE_ACCEL
    /* use accelerator string if it exists */
    if(stored == 4) {
        rpattern.has_accel = 1;
        rpattern.accel = get_accel(accel, &rpattern.accel_type, 
                                   rpattern.case_sensitive);
        if(rpattern.accel == NULL) {
            log(ERROR, "unable to allocate memory from get_accel()\n");
            echo_mode = 1;
            return;
        }
    }
    else {
        rpattern.has_accel = 0;
        rpattern.accel = NULL;
    }
#endif

    
    add_to_plist(rpattern, plist);
    
}

#ifdef USE_ACCEL
char *
get_accel(char *accel, int *accel_type, int case_sensitive) {
    /* returns the stripped accelerator string or NULL if memory can't be
       allocated 
       converts the accel string to lower case if(case_sensitive) */
    
    /* accel_type is assigned one of the values:
       #define ACCEL_NORMAL 1
       #define ACCEL_START  2
       #define ACCEL_END    3     */
    
    int len, i;
    char *new_accel = NULL;
    *accel_type = 0;
  
    len = strlen(accel);
    if(accel[0] == '^')
        *accel_type = ACCEL_START;
    if(accel[len - 1] == '$')
        *accel_type = ACCEL_END;
    if(! *accel_type)
        *accel_type = ACCEL_NORMAL;
    
    /* copy the strings */
    new_accel = (char *)malloc(sizeof(char) * strlen(accel));
    strcpy(new_accel,accel);
    if(*accel_type == ACCEL_START || *accel_type == ACCEL_END) {
        if(new_accel == NULL)
            return NULL;
        if(*accel_type == ACCEL_START) {
            if(case_sensitive)
                for(i = 0; i < len; i++)
                    new_accel[i] = accel[i+1];
            else
                for(i = 0; i < len; i++)
                    new_accel[i] = tolower(accel[i+1]);
        }
        if(*accel_type == ACCEL_END) {
            if(case_sensitive)
                for(i = 0; i < len - 1; i++)
                    new_accel[i] = accel[i];
            else
                for(i = 0; i < len - 1; i++)
                    new_accel[i] = tolower(accel[i]);
            new_accel[i] = '\0';
        }
    }
    else {
        if(!case_sensitive) {
            for(i = 0; i < len; i++)
                new_accel[i] = tolower(accel[i]);
            new_accel[i] = '\0';
        }
    }
    return new_accel;
}
#endif

void
add_to_plist(pattern_item pattern, pattern_item **plist) {
    pattern_item *curr;
    pattern_item *new;
    
    curr = NULL;
    new = NULL;

    if (! (*plist)) {
        /* empty list */
        *plist = xcalloc(1, sizeof(pattern_item));
        new = *plist;
    } else {
        /* find end of list */
        curr = *plist;
        while(curr->next)
            curr = curr->next;
        new = xcalloc(1, sizeof(pattern_item));
        curr->next = new;
    }
    if(! new) {
        log(ERROR, "unable to allocate memory in add_to_plist()\n");
        /* exit(3); */
        echo_mode = 1;
        return;
    }
    new->pattern = pattern.pattern;
    new->replacement = pattern.replacement;
    new->type = pattern.type;
#ifdef USE_ACCEL
    new->has_accel = pattern.has_accel;
    new->accel = pattern.accel;
    new->accel_type = pattern.accel_type;
#endif
    new->case_sensitive = pattern.case_sensitive;
    
    /* not sure whether we need to copy each item in the struct */
    new->cpattern = pattern.cpattern;
    new->next = NULL;
}

int count_parenthesis (char *pattern)
{
    int lcount = 0;
    int rcount = 0;
    int i;
    
    /* Traverse string looking for ( and ) */
    for (i = 0; i < strlen (pattern); i++) {
        /* We have found a left ( */
        if (pattern [i] == '(') {
            /* Do not count if there is a backslash */
            if ((i != 0) && (pattern [i-1] == '\\'))
                continue;
            else
                lcount++;
        }    
        if (pattern [i] == ')') {
            if ((i != 0) && (pattern [i-1] == '\\'))
                continue;
            else
                rcount++;
        }
    }
    /* Check that left == right */
    if (lcount != rcount)
        return (-1);
    return (lcount);
}

void
plist_destroy(pattern_item **a)
{
    pattern_item *b;
    pattern_item *n;
    for (b = *a; b; b = n) {
        n = b->next;
        safe_free(b->pattern);
        safe_free(b->replacement);
#ifdef USE_ACCEL
        if (b->accel)
            safe_free(b->accel);
#endif
        regfree(&(b->cpattern));
        safe_free(b);
    }
    *a = NULL;
}
