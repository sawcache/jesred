/*
 * * Modified: Ulises Nicolini by Vianet 2015/05/11
 * email: developers@sawcache.com
 * web:http://sawcache.com
 * Version: 3.0
 * 
 * 
 * $Id: ip_list.c,v 1.2 1999/01/23 22:16:11 elkner Exp $
 *
 * Author:  Harvest/Squid derived       http://squid.nlanr.net/Squid/
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

/*
 * Copyright (c) 1994, 1995.  All rights reserved.
 *  
 *   The Harvest software was developed by the Internet Research Task
 *   Force Research Group on Resource Discovery (IRTF-RD):
 *  
 *         Mic Bowman of Transarc Corporation.
 *         Peter Danzig of the University of Southern California.
 *         Darren R. Hardy of the University of Colorado at Boulder.
 *         Udi Manber of the University of Arizona.
 *         Michael F. Schwartz of the University of Colorado at Boulder.
 *         Duane Wessels of the University of Colorado at Boulder.
 *  
 *   This copyright notice applies to software in the Harvest
 *   ``src/'' directory only.  Users should consult the individual
 *   copyright notices in the ``components/'' subdirectories for
 *   copyright information about other software bundled with the
 *   Harvest source code distribution.
 *  
 * TERMS OF USE
 *   
 *   The Harvest software may be used and re-distributed without
 *   charge, provided that the software origin and research team are
 *   cited in any use of the system.  Most commonly this is
 *   accomplished by including a link to the Harvest Home Page
 *   (http://harvest.cs.colorado.edu/) from the query page of any
 *   Broker you deploy, as well as in the query result pages.  These
 *   links are generated automatically by the standard Broker
 *   software distribution.
 *   
 *   The Harvest software is provided ``as is'', without express or
 *   implied warranty, and with no support nor obligation to assist
 *   in its use, correction, modification or enhancement.  We assume
 *   no liability with respect to the infringement of copyrights,
 *   trade secrets, or any patents, and are not responsible for
 *   consequential damages.  Proper use of the Harvest software is
 *   entirely the responsibility of the user.
 *  
 * DERIVATIVE WORKS
 *  
 *   Users may make derivative works from the Harvest software, subject 
 *   to the following constraints:
 *  
 *     - You must include the above copyright notice and these 
 *       accompanying paragraphs in all forms of derivative works, 
 *       and any documentation and other materials related to such 
 *       distribution and use acknowledge that the software was 
 *       developed at the above institutions.
 *  
 *     - You must notify IRTF-RD regarding your distribution of 
 *       the derivative work.
 *  
 *     - You must clearly notify users that your are distributing 
 *       a modified version and not the original Harvest software.
 *  
 *     - Any derivative product is also subject to these copyright 
 *       and use restrictions.
 *  
 *   Note that the Harvest software is NOT in the public domain.  We
 *   retain copyright, as specified above.
 *  
 * HISTORY OF FREE SOFTWARE STATUS
 *  
 *   Originally we required sites to license the software in cases
 *   where they were going to build commercial products/services
 *   around Harvest.  In June 1995 we changed this policy.  We now
 *   allow people to use the core Harvest software (the code found in
 *   the Harvest ``src/'' directory) for free.  We made this change
 *   in the interest of encouraging the widest possible deployment of
 *   the technology.  The Harvest software is really a reference
 *   implementation of a set of protocols and formats, some of which
 *   we intend to standardize.  We encourage commercial
 *   re-implementations of code complying to this set of standards.  
 */

#ifdef __FreeBSD__
#  include <stddef.h>
#  include <sys/types.h>
#endif /* __FreeBSD__ */

#include<netinet/in.h>
#include<arpa/inet.h>

#include "ip_list.h"
#include "util.h"
#include "log.h"
#define NULL 0
#include <stdlib.h>
#include <string.h>

static int ip_acl_match(struct in_addr c, const ip_acl *a);


static int
ip_acl_match(struct in_addr c, const ip_acl *a)
{
    static struct in_addr h;

    h.s_addr = c.s_addr & a->mask.s_addr;
    if (h.s_addr == a->addr.s_addr)
        return 1;
    else
        return 0;
}

ip_access_type
ip_access_check(struct in_addr address, const ip_acl *list)
{
    const ip_acl *p = NULL;
    /* address ... network byte-order IP addr */

#ifdef DEBUG
    if (!list) {
        log(DEBG, "ACL: denied %s\n", inet_ntoa(address));
        return IP_DENY;
    }
    for (p = list; p; p = p->next) {
        if (ip_acl_match(address, p)) {
            log(DEBG, "ACL: %s %s\n", p->access==IP_DENY ? "denied" : "allowed",
                inet_ntoa(address));
            return p->access;
        }
    }
    log(DEBG, "ACL: denied %s\n", inet_ntoa(address));
    return IP_DENY;
#else
    if (!list)
        return IP_DENY;
    for (p = list; p; p = p->next) {
        if (ip_acl_match(address, p))
            return p->access;
    }
    return IP_DENY;
#endif
}

void
addToIPACL(ip_acl **list, const char *ip_str)
{
    ip_acl *p, *q;
    int a1, a2, a3, a4, m1;
    struct in_addr lmask;
    int inv = 0;
    int c;

    if (!ip_str) {
        return;
    }
    if (! (*list)) {
        /* empty list */
        *list = xcalloc(1, sizeof(ip_acl));
        (*list)->next = NULL;
        q = *list;
    } else {
        /* find end of list */
        p = *list;
        while (p->next)
            p = p->next;
        q = xcalloc(1, sizeof(ip_acl));
        q->next = NULL;
        p->next = q;
    }

    /* decode ip address */
    if (*ip_str == '!') {
        ip_str++;
        inv++;
    }
    a1 = a2 = a3 = a4 = 0;
    c = sscanf(ip_str, "%d.%d.%d.%d/%d", &a1, &a2, &a3, &a4, &m1);
    if (m1 < 0 || m1 > 32) {
        log(ERROR, "addToIPACL: Ignoring invalid IP acl line '%s'\n",
              ip_str);
        return;
    }
    q->access = inv ? IP_DENY : IP_ALLOW;
    q->addr.s_addr = htonl(a1 * 0x1000000 + a2 * 0x10000 + a3 * 0x100 + a4);
    lmask.s_addr = m1 ? htonl(0xfffffffful << (32 - m1)) : 0;
    q->mask.s_addr = lmask.s_addr;
}

void
ip_acl_destroy(ip_acl **a)
{
    ip_acl *b;
    ip_acl *n;
    for (b = *a; b; b = n) {
        n = b->next;
        safe_free(b);
    }
    *a = NULL;
}
