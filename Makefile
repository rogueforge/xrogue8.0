#
# Makefile for xrogue
#
DISTNAME=xrogue8.0.3
PROGRAM=xrogue

O=o

HDRS=	rogue.h mach_dep.h network.h

OBJS1 =	vers.$(O) actions.$(O) bolt.$(O) chase.$(O) command.$(O) daemon.$(O) \
        daemons.$(O) eat.$(O) effects.$(O) fight.$(O) encumb.$(O) help.$(O) \
        init.$(O) io.$(O) list.$(O) main.$(O) maze.$(O) misc.$(O) monsters.$(O)
OBJS2 = mons_def.$(O) move.$(O) n_level.$(O) options.$(O) outside.$(O) pack.$(O) \
        passages.$(O) player.$(O) potions.$(O) rings.$(O) rip.$(O) rooms.$(O) \
        save.$(O) scrolls.$(O) sticks.$(O) things.$(O) trader.$(O) util.$(O) \
        weapons.$(O) wear.$(O) wizard.$(O) rogue.$(O) state.$(O) xcrypt.$(O)
OBJS  = $(OBJS1) $(OBJS2)

CFILES=	vers.c actions.c bolt.c chase.c command.c daemon.c daemons.c eat.c \
	effects.c fight.c encumb.c help.c init.c io.c list.c main.c maze.c \
	misc.c monsters.c mons_def.c move.c n_level.c options.c outside.c \
	pack.c passages.c player.c potions.c rings.c rip.c rooms.c save.c \
	scrolls.c sticks.c things.c trader.c util.c weapons.c wear.c wizard.c \
	rogue.c state.c xcrypt.c

MISC  = Makefile README.md LICENSE $(PROGRAM).sln $(PROGRAM).vcproj

CC    = cc
CFLAGS= -O3
CRLIB = -lcurses
RM    = rm -f
TAR   = tar 
.SUFFIXES: .obj

.c.obj:
	$(CC) $(CFLAGS) /c $*.c

$(PROGRAM): $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(CRLIB) -o $@

clean:
	$(RM) $(OBJS1)
	$(RM) $(OBJS2)
	$(RM) core a.exe a.out a.exe.stackdump $(PROGRAM) $(PROGRAM).exe $(PROGRAM).tar $(PROGRAM).tar.gz $(PROGRAM).zip

dist.src:
	make clean
	tar cf $(DISTNAME)-src.tar $(CFILES) $(HDRS) $(MISC)
	gzip -f $(DISTNAME)-src.tar

dist.linux:
	make clean
	make CC=gcc CFLAGS="-O3 -Wall -pedantic" $(PROGRAM)
	tar cf $(DISTNAME)-linux.tar $(PROGRAM) README.md LICENSE
	gzip -f $(DISTNAME)-linux.tar

dist.djgpp:
	make clean
	make CC=gcc CFLAGS="-I$(DJDIR)/include -O3 -Wall -pedantic" LDFLAGS="-L$(DJDIR)/lib" CRLIB="-lpdcurses" $(PROGRAM)
	rm -f $(DISTNAME)-djgpp.zip
	zip $(DISTNAME)-djgpp.zip $(PROGRAM).exe README.md LICENSE
 
dist.cygwin:
	make clean
	make CC=gcc LDFLAGS="-static" CFLAGS="-DNCURSES_STATIC -O3 -Wall -pedantic" $(PROGRAM)
	tar cf $(DISTNAME)-cygwin.tar $(PROGRAM).exe README.md LICENSE
	gzip -f $(DISTNAME)-cygwin.tar

dist.win32:
	nmake O="obj" RM="-del" clean
	nmake O="obj" CC="CL" CRLIB="..\pdcurses.lib shell32.lib user32.lib Advapi32.lib Ws2_32.lib" CFLAGS="-nologo -I.. -Ox -wd4033 -wd4716" $(PROGRAM)
	-del $(DISTNAME)-win32.zip
	zip $(DISTNAME)-win32.zip $(PROGRAM).exe README.md LICENSE
