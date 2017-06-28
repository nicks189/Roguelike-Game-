CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

CFLAGS = -Ofast -Wall -Werror -ggdb -pg -funroll-loops
CXXFLAGS = -std=c++11 -Ofast -Wall -Werror -Wno-narrowing -ggdb -funroll-loops
LDFLAGS = -lncurses

SOURCE = source

BUILDDIR = build

BIN = rlg
_OBJS = rlg.o heap.o dungeon.o pathfinding.o character.o event.o \
       pc.o npc.o factory.o dice.o item.o display.o projectile.o \
			 effect.o mapMaker.o

_MAPOBJS = heap.o dungeon.o pathfinding.o character.o event.o \
       pc.o npc.o factory.o dice.o item.o display.o projectile.o \
			 effect.o mapMaker.o

OBJS = $(patsubst %,$(BUILDDIR)/%,$(_OBJS))
MAPOBJS = $(patsubst %,$(BUILDDIR)/%,$(_MAPOBJS))

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

$(BUILDDIR)/%.o: $(SOURCE)/%.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $< -o $@
	@$ mv *.d $(BUILDDIR)

.PHONY: all clean clobber etags

map: source/mapMakerMain.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) $(MAPOBJS) -o mapMaker $< $(LDFLAGS)

clean: 
	@$(ECHO) Removing all generated files
	@$(RM) build/*.o build/*.d $(BIN) TAGS gmon.out mapMaker core compile_commands.json vgcore.*

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags source/*.[ch]
