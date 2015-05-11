/*
 * $Id: log.h,v 1.1 1998/05/17 15:25:08 elkner Exp $
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
#ifndef LOG_H
#define LOG_H

typedef enum {
    MATCH,
    ERROR,
    DEBG,
    INFO
} log_code;

extern void log(log_code c, char *format, ...);
extern void closeLogs(void);
extern void openLogs(char **, char **);

#endif
