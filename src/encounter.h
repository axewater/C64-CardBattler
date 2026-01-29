#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include <stdint.h>

#define MAX_ENCOUNTERS 5

/* Encounter Functions */
void encounter_init(void);
uint8_t encounter_get_enemy_for_floor(uint8_t floor);
void encounter_advance(void);
uint8_t encounter_is_run_complete(void);
void encounter_victory_rewards(void);

#endif /* ENCOUNTER_H */
