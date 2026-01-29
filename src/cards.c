#include "cards.h"

/* Card Name String Table */
const char* const card_names[MAX_CARDS_IN_DATABASE] = {
    "STRIKE",       /* 0 */
    "DEFEND",       /* 1 */
    "BASH",         /* 2 */
    "HEAVY STRIKE", /* 3 */
    "IRON WALL",    /* 4 */
    "DRAW",         /* 5 */
    "ENERGY",       /* 6 */
    "EXECUTE",      /* 7 */
    "CLEAVE",       /* 8 */
    "SHIELD BASH",  /* 9 */
    "POWER STRIKE", /* 10 */
    "ARMORED",      /* 11 */
    "QUICK SLASH",  /* 12 */
    "FORTIFY",      /* 13 */
    "BERSERK"       /* 14 */
};

/* Card Database - const to store in ROM */
/* Format: { id, cost, attack, block, type, effects, target, name_idx } */
const Card card_database[MAX_CARDS_IN_DATABASE] = {
    /* Starter Cards */
    { CARD_ID_STRIKE, 1, 6, 0, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ENEMY, 0 },
    { CARD_ID_DEFEND, 1, 0, 5, CARD_TYPE_SKILL, EFFECT_NONE, TARGET_SELF, 1 },
    { CARD_ID_BASH, 2, 8, 0, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ENEMY, 2 },

    /* Unlockable Cards - Tier 1 */
    { CARD_ID_HEAVY_STRIKE, 2, 14, 0, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ENEMY, 3 },
    { CARD_ID_IRON_WALL, 2, 0, 12, CARD_TYPE_SKILL, EFFECT_NONE, TARGET_SELF, 4 },
    { CARD_ID_DRAW, 1, 0, 0, CARD_TYPE_SKILL, EFFECT_DRAW, TARGET_SELF, 5 },

    /* Unlockable Cards - Tier 2 */
    { CARD_ID_ENERGY, 0, 0, 0, CARD_TYPE_SKILL, EFFECT_ENERGY, TARGET_SELF, 6 },
    { CARD_ID_EXECUTE, 2, 10, 0, CARD_TYPE_ATTACK, EFFECT_EXECUTE, TARGET_ENEMY, 7 },
    { CARD_ID_CLEAVE, 1, 4, 0, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ALL, 8 },

    /* Unlockable Cards - Tier 3 */
    { CARD_ID_SHIELD_BASH, 2, 6, 6, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ENEMY, 9 },

    /* Additional Cards - Placeholders */
    { 10, 3, 16, 0, CARD_TYPE_ATTACK, EFFECT_EXHAUST, TARGET_ENEMY, 10 },
    { 11, 2, 0, 8, CARD_TYPE_SKILL, EFFECT_NONE, TARGET_SELF, 11 },
    { 12, 0, 3, 0, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ENEMY, 12 },
    { 13, 1, 0, 7, CARD_TYPE_SKILL, EFFECT_NONE, TARGET_SELF, 13 },
    { 14, 1, 10, 0, CARD_TYPE_ATTACK, EFFECT_NONE, TARGET_ENEMY, 14 }
};

/* Get card by ID */
const Card* card_get(uint8_t card_id) {
    uint8_t i;
    for (i = 0; i < MAX_CARDS_IN_DATABASE; i++) {
        if (card_database[i].id == card_id) {
            return &card_database[i];
        }
    }
    return &card_database[0]; /* Default to STRIKE */
}

/* Get card name */
const char* card_get_name(uint8_t card_id) {
    const Card* card = card_get(card_id);
    return card_names[card->name_idx];
}

/* Check if card is unlocked (bit flag) */
uint8_t card_is_unlocked(uint8_t card_id, uint16_t unlock_flags) {
    /* Starter cards (0-2) always unlocked */
    if (card_id <= CARD_ID_BASH) {
        return 1;
    }
    /* Check bit flag */
    return (unlock_flags & (1 << card_id)) != 0;
}

/* Unlock a card */
void card_unlock(uint8_t card_id, uint16_t* unlock_flags) {
    if (card_id < 16) {
        *unlock_flags |= (1 << card_id);
    }
}
