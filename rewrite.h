/*
 * Modified: Ulises Nicolini by Vianet 2015/05/11
 * email: developers@sawcache.com
 * web:http://sawcache.com
 * Version: 3.0
 * 
 * $Id: rewrite.h,v 1.2 1998/07/25 03:16:19 elkner Exp $
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
 * Thanks to Chris Foote, chris@senet.com.au - except parse_buff
 * not much to change here (i.e. don't like to go deeper into the pattern stuff)
 * ;-)
 *
 */

#ifndef REWRITE_H
#define REWRITE_H

extern int parse_buff(char *, char**, char **, char **, char **, char **,
		      ip_acl *, pattern_item *);

#endif
