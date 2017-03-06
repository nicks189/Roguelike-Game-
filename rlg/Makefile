default: heap pathfinding character character_util dungeon5_0

dungeon5_0: dungeon5_0.c
	gcc -ggdb -Wall -Werror -O character.o pathfinding.o heap.o character_util.o -o dungeon dungeon5_0.c -lncurses

heap: heap.c
	gcc heap.c -ggdb -Wall -Werror -c -O

pathfinding: pathfinding.c
	gcc pathfinding.c -ggdb -Wall -Werror -c -O

character: character.c
	gcc character.c -ggdb -Wall -Werror -c -O

character_util: character_util.c
	gcc character_util.c -ggdb -Wall -Werror -c -O

clean:
	rm -f dungeon heap.o pathfinding.o character.o character_util.o *~


