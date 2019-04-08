#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

void io_init_terminal(void);
int move_pc(dungeon_space_t dungeon[][DUNGEON_X],
            character_t *character_map[][DUNGEON_X], character_t *pc,
            heap_t *mh, int fog);
void render_dungeon(dungeon_space_t dungeon[][DUNGEON_X],
                    character_t *character_map[][DUNGEON_X], character_t *pc,
                    heap_t *mh, int fog);
void render_dungeon_first(dungeon_space_t dungeon[][DUNGEON_X],
                          character_t *character_map[][DUNGEON_X],
                          character_t *pc, heap_t *mh, int fog);
void display_tunneling_map(dungeon_space_t dungeon[][DUNGEON_X]);
void display_non_tunneling_map(dungeon_space_t dungeon[][DUNGEON_X]);
void display_default_map(dungeon_space_t dungeon[][DUNGEON_X]);
void game_over(int result);
void invalid_move();
void invalid_key();
