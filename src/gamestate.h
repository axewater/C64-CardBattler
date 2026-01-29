#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>

/* Game States */
typedef enum {
    STATE_TITLE,
    STATE_MENU,
    STATE_RUN_START,
    STATE_ENCOUNTER,
    STATE_COMBAT,
    STATE_VICTORY,
    STATE_REWARD,
    STATE_DEFEAT,
    STATE_WIN,
    STATE_UNLOCKS,
    STATE_QUIT
} GameState;

/* State Management */
void gamestate_init(void);
void gamestate_update(void);
GameState gamestate_get(void);
void gamestate_set(GameState new_state);

/* State Handlers */
void state_title(void);
void state_menu(void);
void state_run_start(void);
void state_encounter(void);
void state_combat(void);
void state_victory(void);
void state_reward(void);
void state_defeat(void);
void state_win(void);
void state_unlocks(void);

#endif /* GAMESTATE_H */
