default: heap pathfinding character character_util dungeon5_0 

dungeon5_0: dungeon5_0.c
	gcc -ggdb -Wall -Werror character.o pathfinding.o heap.o character_util.o -o dungeon dungeon5_0.c -lncurses  

heap: heap.c
	gcc heap.c -ggdb -Wall -Werror -c -Ofast

pathfinding: pathfinding.c
	gcc pathfinding.c -ggdb -Wall -Werror -c -Ofast

character: character.c
	gcc character.c -ggdb -Wall -Werror -c -Ofast

character_util: character_util.c
	gcc character_util.c -ggdb -Wall -Werror -c -Ofast

clean: 
	rm -f dungeon heap.o pathfinding.o character.o character_util.o *~ 


