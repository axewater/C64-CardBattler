#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

#define MAX_DECK_SIZE 60
#define MAX_HAND_SIZE 10
#define STARTING_HP 50
#define STARTING_ENERGY 3

/* Player Structure (optimized for memory) */
typedef struct {
    uint8_t hp;
    uint8_t max_hp;
    uint8_t energy;
    uint8_t max_energy;
    uint8_t block;
    uint8_t deck_size;
    uint8_t hand_size;
    uint8_t discard_size;
    uint8_t draw_pile_size;
    uint8_t run_progress;     /* Current encounter (0-4) */
    uint16_t unlocks;         /* Bitmask of unlocked cards */
    uint8_t total_runs;       /* Persistent stat */
    uint8_t total_wins;       /* Persistent stat */
    uint8_t deck[MAX_DECK_SIZE];  /* Card IDs in deck */
    uint8_t hand[MAX_HAND_SIZE];  /* Card IDs in hand */
} Player;

/* Global player instance */
extern Player player;

/* Player Functions */
void player_init(void);
void player_start_run(void);
void player_start_combat(void);
void player_start_turn(void);
void player_end_turn(void);
void player_take_damage(uint8_t damage);
void player_gain_block(uint8_t amount);
void player_heal(uint8_t amount);
uint8_t player_can_play_card(uint8_t card_id);
void player_spend_energy(uint8_t amount);
void player_add_card_to_deck(uint8_t card_id);
void player_remove_card_from_deck(uint8_t card_id);

#endif /* PLAYER_H */
