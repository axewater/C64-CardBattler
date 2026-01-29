#ifndef CARDS_H
#define CARDS_H

#include <stdint.h>

/* Card Types */
#define CARD_TYPE_ATTACK  0
#define CARD_TYPE_SKILL   1
#define CARD_TYPE_POWER   2

/* Target Types */
#define TARGET_ENEMY      0
#define TARGET_SELF       1
#define TARGET_ALL        2

/* Card Effect Flags (bitmask) */
#define EFFECT_NONE       0x00
#define EFFECT_DRAW       0x01  /* Draw cards */
#define EFFECT_ENERGY     0x02  /* Gain energy */
#define EFFECT_EXECUTE    0x04  /* Bonus damage to low HP */
#define EFFECT_EXHAUST    0x08  /* Remove from combat */

/* Card IDs */
#define CARD_ID_STRIKE        0
#define CARD_ID_DEFEND        1
#define CARD_ID_BASH          2
#define CARD_ID_HEAVY_STRIKE  3
#define CARD_ID_IRON_WALL     4
#define CARD_ID_DRAW          5
#define CARD_ID_ENERGY        6
#define CARD_ID_EXECUTE       7
#define CARD_ID_CLEAVE        8
#define CARD_ID_SHIELD_BASH   9

#define MAX_CARDS_IN_DATABASE 15

/* Card Structure (8 bytes) */
typedef struct {
    uint8_t id;           /* Card identifier */
    uint8_t cost;         /* Energy cost */
    uint8_t attack;       /* Damage value */
    uint8_t block;        /* Shield value */
    uint8_t type;         /* Attack/Skill/Power */
    uint8_t effects;      /* Bitmask for special effects */
    uint8_t target;       /* Self/Enemy/All */
    uint8_t name_idx;     /* Index into string table */
} Card;

/* Card Name Strings */
extern const char* const card_names[];

/* Card Database */
extern const Card card_database[MAX_CARDS_IN_DATABASE];

/* Card Functions */
const Card* card_get(uint8_t card_id);
const char* card_get_name(uint8_t card_id);
uint8_t card_is_unlocked(uint8_t card_id, uint16_t unlock_flags);
void card_unlock(uint8_t card_id, uint16_t* unlock_flags);

#endif /* CARDS_H */
