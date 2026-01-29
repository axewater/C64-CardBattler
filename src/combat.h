#ifndef COMBAT_H
#define COMBAT_H

#include <stdint.h>

/* Combat state */
typedef enum {
    COMBAT_PLAYER_TURN,
    COMBAT_ENEMY_TURN,
    COMBAT_VICTORY,
    COMBAT_DEFEAT
} CombatState;

/* Combat Functions */
void combat_init(uint8_t enemy_id);
void combat_start_turn(void);
void combat_player_play_card(uint8_t hand_index);
void combat_end_turn(void);
void combat_render(void);
CombatState combat_get_state(void);
void combat_run(void);

#endif /* COMBAT_H */
