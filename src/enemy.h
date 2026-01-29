#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

/* Enemy IDs */
#define ENEMY_ID_SLIME       0
#define ENEMY_ID_GOBLIN      1
#define ENEMY_ID_CULTIST     2
#define ENEMY_ID_ELITE_ORC   3
#define ENEMY_ID_BOSS_DRAGON 4

/* Intent Types (what enemy will do next turn) */
#define INTENT_ATTACK  0
#define INTENT_DEFEND  1
#define INTENT_BUFF    2
#define INTENT_DEBUFF  3

/* Enemy Structure (12 bytes) */
typedef struct {
    uint8_t id;
    uint8_t hp;
    uint8_t max_hp;
    uint8_t block;
    uint8_t intent;         /* Next action type */
    uint8_t intent_value;   /* Damage/block amount */
    uint8_t ai_state;       /* AI pattern phase */
    uint8_t sprite_char;    /* PETSCII character */
    uint8_t pattern_index;  /* Current position in pattern */
    uint8_t attack_min;     /* Min attack damage */
    uint8_t attack_max;     /* Max attack damage */
    uint8_t is_elite;       /* Elite/boss flag */
} Enemy;

/* Global current enemy */
extern Enemy current_enemy;

/* Enemy Functions */
void enemy_init(uint8_t enemy_id);
void enemy_calculate_intent(void);
void enemy_execute_action(void);
void enemy_take_damage(uint8_t damage);
void enemy_gain_block(uint8_t amount);
uint8_t enemy_is_dead(void);
const char* enemy_get_name(uint8_t enemy_id);
uint8_t enemy_get_intent_char(void);

#endif /* ENEMY_H */
