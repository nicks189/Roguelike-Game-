default: heap pathfinding character character_util dungeon pc npc event rlg link

OBJS = rlg.o dungeon.o event.o pc.o npc.o heap.o pathfinding.o character.o character_util.o 

link: $(OBJS) 
	g++ -std=c++11 -Ofast -ggdb -Wall -Werror character.o pathfinding.o event.o heap.o character_util.o dungeon.o pc.o npc.o rlg.o -o rlg -lncurses 

rlg: rlg.c
	gcc -Ofast rlg.c -ggdb -Wall -Werror -c

dungeon: dungeon.c
	gcc -Ofast dungeon.c -ggdb -Wall -Werror -c  

event: event.c
	gcc -Ofast event.c -ggdb -Wall -Werror -c

pc: pc.cpp
	g++ -std=c++11 -Ofast pc.cpp -ggdb -Wall -Werror -c  	 

npc: npc.cpp
	g++ -std=c++11 -Ofast npc.cpp -ggdb -Wall -Werror -c  	 

heap: heap.c
	gcc -Ofast heap.c -ggdb -Wall -Werror -c  

pathfinding: pathfinding.c
	gcc -Ofast pathfinding.c -ggdb -Wall -Werror -c   

character: character.cpp
	g++ -std=c++11 -Ofast character.cpp -ggdb -Wall -Werror -c  

character_util: character_util.cpp
	g++ -std=c++11 -Ofast character_util.cpp -ggdb -Wall -Werror -c  

clean:
	rm -f dungeon.o heap.o pathfinding.o character.o character_util.o npc.o pc.o event.o rlg.o rlg gmon.out valgrind.log *~


