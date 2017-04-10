CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

CFLAGS = -Ofast -Wall -Werror -ggdb -pg -funroll-loops
CXXFLAGS = -std=c++11 -Wall -Werror -Wno-narrowing -ggdb -funroll-loops
LDFLAGS = -lncurses -pg

BIN = rlg
OBJS = rlg.o heap.o dungeon.o pathfinding.o character.o event.o \
       pc.o npc.o factory.o dice.o item.o display.o

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber etags

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS gmon.out core vgcore.* 

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.[ch]
