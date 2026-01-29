#include "encounter.h"
#include "player.h"
#include "enemy.h"
#include "random.h"
#include "cards.h"

/* Initialize encounter system */
void encounter_init(void) {
    player.run_progress = 0;
}

/* Get enemy ID for current floor */
uint8_t encounter_get_enemy_for_floor(uint8_t floor) {
    /* Floor 0-1: Easy enemies */
    if (floor <= 1) {
        return random_range(0, 2); /* SLIME or GOBLIN */
    }
    /* Floor 2-3: Medium enemies */
    else if (floor <= 3) {
        return random_range(1, 3); /* GOBLIN or CULTIST */
    }
    /* Floor 4: Elite/Boss */
    else {
        return ENEMY_ID_BOSS_DRAGON;
    }
}

/* Advance to next encounter */
void encounter_advance(void) {
    player.run_progress++;
}

/* Check if run is complete */
uint8_t encounter_is_run_complete(void) {
    return player.run_progress >= MAX_ENCOUNTERS;
}

/* Handle victory rewards */
void encounter_victory_rewards(void) {
    /* Heal 10 HP */
    player_heal(10);

    /* TODO: Card rewards will be implemented in UI */
}
