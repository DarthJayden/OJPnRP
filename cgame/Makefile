# Linux Makefile for Jedi Academy MP SDK
# By Patrick Hemmer
# Version 2
# 
# Created Nov 29, 2003
# The Void - http://www.divoid.net
#
# You may set your own optimizations on the CFLAGS line. If you dont know what optimizations are, then just leave it as is.
# Run 'make' to compile the code, once done, you will have a file called 'jampgamei386.so' in the 'game' directory. Copy this file to the 'base' folder of your server or to your mod folder.
# If for some strange reason, you are running on less than a 586 processor, change the i586 to i386 on the CFLAGS line.
#
# Revised by Raz0r on Jan 18, 2011
# 	Inline assembler will use intel syntax
#	Forcing a 32bit binary. Ignoring the -march switch in favour of -m32
#

# Compiler - GCC
CC = gcc
TARGET =  ../../cgamei386.so

# Compiler Flags - swap CONFIG for release/debug builds
#CFLAGS += -O3 -DNDEBUG -DFINAL_BUILD
CFLAGS += -g3 -D_DEBUG
CFLAGS += -I./
CFLAGS += -D_JK2
CFLAGS += -fPIC -Wall -masm=intel -m32# -fno-strict-aliasing -march=i586
CFLAGS += -D_JK2MP


OFILES += bg_g2_utils.o bg_misc.o bg_panimate.o bg_pmove.o bg_saber.o bg_saberLoad.o bg_saga.o bg_slidemove.o bg_vehicleLoad.o bg_weapons.o
OFILES += cg_camera.o cg_consolecmds.o cg_draw.o cg_drawtools.o cg_effects.o cg_ents.o cg_event.o cg_holster.o cg_info.o
OFILES += cg_light.o cg_localents.o cg_main.o cg_marks.o cg_newDraw.o cg_players.o cg_playerstate.o cg_predict.o cg_saga.o
OFILES += cg_scoreboard.o cg_servercmds.o cg_snapshot.o cg_strap.o cg_syscalls.o cg_turret.o cg_view.o cg_weaponinit.o 
OFILES += cg_weapons.o
OFILES += fx_blaster.o fx_bowcaster.o fx_bryarpistol.o fx_demp2.o fx_disruptor.o fx_flechette.o fx_force.o fx_heavyrepeater.o
OFILES += fx_rocketlauncher.o 


$(TARGET): $(OFILES)
	$(CC) -shared -m32 -o $(TARGET) $(OFILES) -lm

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

clean:
	rm -f $(OFILES)
	rm -f $(TARGET)
