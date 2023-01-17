#
# Makefile for xrogue
#
DISTNAME=xrogue8.0.1

HDRS=	rogue.h mach_dep.h network.h

OBJS=	vers.o actions.o bolt.o chase.o command.o daemon.o daemons.o eat.o \
	effects.o fight.o encumb.o help.o init.o io.o list.o main.o maze.o \
	misc.o monsters.o mons_def.o move.o n_level.o options.o outside.o \
	pack.o passages.o player.o potions.o rings.o rip.o rooms.o save.o \
	scrolls.o sticks.o things.o trader.o util.o weapons.o wear.o wizard.o \
	rogue.o state.o xcrypt.o

CFILES=	vers.c actions.c bolt.c chase.c command.c daemon.c daemons.c eat.c \
	effects.c fight.c encumb.c help.c init.c io.c list.c main.c maze.c \
	misc.c monsters.c mons_def.c move.c n_level.c options.c outside.c \
	pack.c passages.c player.c potions.c rings.c rip.c rooms.c save.c \
	scrolls.c sticks.c things.c trader.c util.c weapons.c wear.c wizard.c \
	rogue.c state.c xcrypt.c

MISC=   Makefile README.md LICENSE

CC    = cc
CFLAGS= -O3
CRLIB = -lcurses

xrogue: $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(CRLIB) -o $@

clean:
	rm -f $(OBJS) xrogue xrg a.out $(DISTNAME)

dist.src:
	make clean
	tar cf $(DISTNAME)-src.tar $(CFILES) $(HDRS) $(MISC)
	gzip -f $(DISTNAME)-src.tar

dist.linux:
	make clean
	make CC=gcc CFLAGS="-O3 -Wall -pedantic" xrogue
	tar cf $(DISTNAME)-linux.tar xrogue README.md LICENSE
	gzip -f $(DISTNAME)-linux.tar

dist.djgpp:
	make clean
	make CC=gcc CFLAGS="-I$(DJDIR)/include -O3 -Wall -pedantic" LDFLAGS="-L$(DJDIR)/lib" CRLIB="-lpdcurses" xrogue
	rm -f $(DISTNAME)-djgpp.zip
	zip $(DISTNAME)-djgpp.zip xrogue.exe README.md LICENSE
 
