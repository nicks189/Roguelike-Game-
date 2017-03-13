default: heap pathfinding character character_util dungeon pc npc event rlg

rlg: rlg.c
	gcc -ggdb -Wall -Werror character.o pathfinding.o event.o heap.o character_util.o dungeon.o pc.o npc.o rlg.c -o rlg -lncurses 

dungeon: dungeon.c
	gcc dungeon.c -ggdb -Wall -Werror -c  

event: event.c
	gcc event.c -ggdb -Wall -Werror -c

pc: pc.c
	gcc pc.c -ggdb -Wall -Werror -c  	 

npc: npc.c
	gcc npc.c -ggdb -Wall -Werror -c  	 

heap: heap.c
	gcc heap.c -ggdb -Wall -Werror -c  

pathfinding: pathfinding.c
	gcc pathfinding.c -ggdb -Wall -Werror -c   

character: character.c
	gcc character.c -ggdb -Wall -Werror -c  

character_util: character_util.c
	gcc character_util.c -ggdb -Wall -Werror -c  

clean:
	rm -f dungeon.o heap.o pathfinding.o character.o character_util.o npc.o pc.o event.o rlg *~


