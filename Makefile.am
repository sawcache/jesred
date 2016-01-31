ACLOCAL_AMFLAGS = -I m4

# Your preferred Compiler
# HINT: If you want to run the program on an UltraSPARC driven machine,
#       I recommend to use Sun C-Compiler >= 4.0 or gcc >= 2.8.0 with
#       optimization level >= 4 for best performance.
CC=cc

INCLUDE         = -I.
LDFLAGS         =

# Compiler Options

#AC_CFLAGS       = -xO5 -mr -s -xcrossfile #-g -xsb     	# SUN cc >= 4.0
#AC_CFLAGS	= -O3 -s				# SGI cc, AIX cc
AC_CFLAGS	= -O5 -s				# GNU cc

# extra Libraries ( we need this for inet_addr(char *) only )

XTRA_LIBS       = -lnsl		# Solaris 2.x
#XTRA_LIBS	=		# AIX 4.2, Linux 2.x.x, IRIX 5.x, 6.x

# if you want to have the option, to enable debug message logging,
# uncomment the following line, but beware: This DECREASES THE PERFORMANCE
# of the program a lot !!!

DEBUG	= -DDEBUG

# If you don't have regex support on your OS standard librar{y|ies}
# or you do not want to use it, get gnu regex 0.12 or higher,
# compile it as described in its INSTALL (you do not need to install it!),
# uncomment the lines below (XOBJS, DEFS) and substitute regex-0.12 with the
# directory, where GNU regex.h and regex.o are located.
# Hint: At least on Solaris >= 2.5 you get best performance, when you
# use the regex stuff from stdlibc. If you really need the GNU stuff,
# compile it with highest optimization (default is no optimization!),
# which gives you about 20% speed up ... GNU regex is available via:
# ftp://ftp.tu-chemnitz.de/pub/gnu/regex-0.12.tar.gz    (GNU mirror) or
# ftp://prep.ai.mit.edu/pub/gnu/regex-0.12.tar.gz

# XOBJS	= regex-0.12/regex.o
# DEFS	= -DLOCAL_REGEX -Iregex-0.12

# If you want to use ACCEL patterns (compatibility to squirm 1.0Beta),
# add -DUSE_ACCEL to DEFS.
#
# Since in my tests it didn't gave me much performance improvement, I don't
# use them but prefer a more readable redirect.rules files (and so a little
# bit smaller jesred wrt. memory usage). So why to compile it in, when
# I don't want to use ACCEL ;-)
# If jesred is compiled without -DUSE_ACCEL the ACCEL strings in the
# redirect rules are ignored, if there are some.
#
# DEFS = -DUSE_ACCEL

# That's it - no further changes should be necessary ;-)
# ===========================================================================

PROGS	= jesred
CFLAGS  = $(AC_CFLAGS) $(INCLUDE)
LIBS    = -L. $(XTRA_LIBS)


bin_PROGRAMS = jesred
jesred_SOURCES = ip_list.c \
	log.c \
	main.c \
	pattern_list.c \
	rewrite.c \
	util.c \
	ip_list.h  log.h  main.h  path.h  pattern_list.h  rewrite.h  util.h  version.h

all:	$(PROGS)

depend:
	makedepend $(CFLAGS) $(DEFS) $(CFILES)

.DONE:	query

query:
	-sbquery nosuchthing

clean-local:
	-rm -rf  $(OBJS) *pure_* core $(PROGS)
