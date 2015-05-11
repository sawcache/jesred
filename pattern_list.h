/*
 * $Id: pattern_list.h,v 1.3 1998/07/25 03:05:40 elkner Exp $
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

#ifndef PATTERN_H
#define PATTERN_H

#define NORMAL   1
#define EXTENDED 2
#define ABORT    3

#ifdef USE_ACCEL
#define ACCEL_NORMAL 1
#define ACCEL_START  2
#define ACCEL_END    3
#endif

typedef struct _pattern_item {
    char *pattern;
    char *replacement;
    int case_sensitive;
    int type;
#ifdef USE_ACCEL
    int has_accel;
    int accel_type;
    char *accel;
#endif
    regex_t cpattern;
    struct _pattern_item *next;
} pattern_item;

extern void add_to_patterns(char *, pattern_item **);
extern void plist_destroy(pattern_item **);

#endif
