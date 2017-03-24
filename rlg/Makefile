default: heap pathfinding character dungeon pc npc event rlg link

OBJS = rlg.o dungeon.o event.o pc.o npc.o heap.o pathfinding.o character.o 

link: $(OBJS) 
	g++ -std=c++11 -ggdb -Wall -Werror character.o pathfinding.o event.o heap.o dungeon.o pc.o npc.o rlg.o -o rlg -lncurses 

rlg: rlg.cpp
	g++ -std=c++11 rlg.cpp -ggdb -Wall -Werror -c

dungeon: dungeon.cpp
	g++ -std=c++11 -Wno-narrowing dungeon.cpp -ggdb -Wall -Werror -c  

event: event.cpp
	g++ -std=c++11 event.cpp -ggdb -Wall -Werror -c

pc: pc.cpp
	g++ -std=c++11 pc.cpp -ggdb -Wall -Werror -c  	 

npc: npc.cpp
	g++ -std=c++11 npc.cpp -ggdb -Wall -Werror -c  	 

heap: heap.cpp
	g++ -std=c++11 heap.cpp -ggdb -Wall -Werror -c  

pathfinding: pathfinding.cpp
	g++ -std=c++11 pathfinding.cpp -ggdb -Wall -Werror -c   

character: character.cpp
	g++ -std=c++11 character.cpp -ggdb -Wall -Werror -c  

character_util: character_util.cpp
	g++ -std=c++11 character_util.cpp -ggdb -Wall -Werror -c  

clean:
	rm -f dungeon.o heap.o pathfinding.o character.o npc.o pc.o event.o rlg.o rlg gmon.out valgrind.log *~


