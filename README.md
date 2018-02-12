########## README ##########

Created for Computer Science course at Iowa State University

READ BEFORE ATTEMPTING TO BUILD --
 - Ncurses is required to compile (check in /usr/include/). If you don't have
   it, search online for an installation guide for your distribution
 - Must have C++11
 - Move the temp folder (found in source/) into your home directory and name
   it .rlg327 so that the path to it is ~/.rlg327/ -- Alternatively, just run
   this command: mv source/temp ~/.rlg327

Runnning arguments --
 - --nummon <number> or -m <number> to change the number of monsters loaded
 - --items <number> or -i <number> to change the number of items loaded
 - --nofog or -g to disable fog of war
 - --rand <seed> or -r <seed> to select seed

Controls --
 - w - wear/consume screen
 - t - takeoff equipment screen
 - x - expunge item screen
 - d - drop item screen
 - i - inventory screen
 - I - inspection screen
 - c - display character screen
 - r - ranged combat screen (prompts for direction to attack)
 - L - look view
 - esc - exit to default view

Current features --
 - Working hit, dodge, speed, and defense stats. (Details below)
 - Ranged combat with ranged weapons and ammo
 - Ranged combat with spells (2 damage spells currently implemented)
 - Mana for spells
 - Intellect - increases mana and spell damage
 - Equipping light objects increases visisble radius
 - Dungeon levels increase as you go down stairs, decrease as you go up
 - Monsters and items have levels - higher level monsters/items are stronger
   and only spawn at higher dungeon levels (unless using --default switch)
 - Pass, destroy and pickup monsters are implemented
 - Food items restore hp, new water items restore mana.
 - Scrolls/books increase intellect. Scrolls teach new spells
 - Flasks can be consumed to boost various stats

Additional details --
 - Dodge/hit: All characters default with 70% hit chance. Additional hit rating
   increase percent to hit by that number, ie: 30 hit -> 100% hit chance. Dodge
   reduces hit chance by the dodge value by the same logic. Higher level
   monsters have higher hit/dodge ratings.
 - Intellect: Increases damage of spells by actual rating and increases mana
   by half of rating.
 - Using a ranged weapon requires ammunition to be equipped. Ammunition
   attribute determines how much ammo. Using ranged weapon decreases ammunition
   attribute by one.
 - By default, only poison ball spell is learned. A fireball and heal spell can
   be learned by picking up and consuming a Scroll of Fire or a Scroll of
   Wisdom, respectively.
 - As an aside, don't expect much in the way of balance.

Known bugs --
- Player turn is wasted when cancelling spell