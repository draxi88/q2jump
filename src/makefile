#
# Quake2 gamei386.so Makefile for Linux 2.0
#
# Jan '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#
# Probably requires GNU make
#

#
# Quake2 gamei386.so Makefile for Linux 2.0
#
# Jan '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#
# Probably requires GNU make
#
# This builds the gamei386.so for Linux based on the q2source_12_11.zip
# release.  
# Put his Makefile in the game subdirectory you get when you unzip
# q2source_12_11.zip.
#
# There are two compiler errors you'll get, the following fixes
# are necessary:
#
# In g_local.h (around line 828), you must change the 
#    typedef struct g_client_s { ... } gclient_t;
# to just:
#    struct g_client_s { ... };
# The typedef is already defined elsewhere (seems to compile fine under
# MSCV++ for Win32 for some reason).
#
# m_player.h has a Ctrl-Z at the end (damn DOS editors).  Remove it or
# gcc complains.
#
# Note that the source in q2source_12_11.zip is for version 3.05.  To
# get it to run with Linux 3.10, change the following in game.h:
#    #define	GAME_API_VERSION	1
# change it to:
#    #define	GAME_API_VERSION	2

ARCH=$(shell uname -m | sed -e s/i.86/i386/ -e s/amd64/x86_64/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/alpha/axp/)
CC=gcc
BASE_CFLAGS=-Dstricmp=strcasecmp

#use these cflags to optimize it
CFLAGS=$(BASE_CFLAGS) -march=native -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -falign-loops=2 \
	-falign-jumps=2 -falign-functions=2
#use these when debugging 
#CFLAGS=$(BASE_CFLAGS) -g

OBJDIR=linux

LDFLAGS=-ldl -lm
SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

.c.o:
	$(DO_CC)

GAME_OBJS = \
	g_ai.o g_chase.o g_cmds.o g_combat.o g_ctf.o g_func.o g_items.o \
	g_main.o g_misc.o g_monster.o g_phys.o g_save.o g_spawn.o g_svcmds.o \
	g_target.o g_trigger.o g_utils.o g_weapon.o m_move.o p_client.o \
	p_hud.o p_menu.o p_trail.o p_view.o p_weapon.o q_shared.o jumpmod.o \
	zb_util.o m_insane.o m_infantry.o m_gunner.o m_flipper.o m_berserk.o \
	m_boss3.o m_mutant.o m_soldier.o

game$(ARCH).$(SHLIBEXT) : $(GAME_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GAME_OBJS)

#############################################################################
# MISC
#############################################################################

clean:
	-rm -f $(GAME_OBJS)

depend:
	gcc -MM $(GAME_OBJS:.o=.c)

