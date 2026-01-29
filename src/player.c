#include "player.h"
#include "cards.h"
#include <string.h>

/* Global player instance */
Player player;

/* Initialize player with default stats */
void player_init(void) {
    memset(&player, 0, sizeof(Player));
    player.max_hp = STARTING_HP;
    player.hp = STARTING_HP;
    player.max_energy = STARTING_ENERGY;
    player.energy = STARTING_ENERGY;
    player.unlocks = 0x0007; /* Unlock starter cards (bits 0-2) */
}

/* Start a new run */
void player_start_run(void) {
    player.hp = player.max_hp;
    player.energy = player.max_energy;
    player.block = 0;
    player.run_progress = 0;
    player.deck_size = 0;
    player.hand_size = 0;
    player.discard_size = 0;
    player.draw_pile_size = 0;

    /* Build starter deck: 5x STRIKE, 4x DEFEND, 1x BASH */
    player.deck[0] = CARD_ID_STRIKE;
    player.deck[1] = CARD_ID_STRIKE;
    player.deck[2] = CARD_ID_STRIKE;
    player.deck[3] = CARD_ID_STRIKE;
    player.deck[4] = CARD_ID_STRIKE;
    player.deck[5] = CARD_ID_DEFEND;
    player.deck[6] = CARD_ID_DEFEND;
    player.deck[7] = CARD_ID_DEFEND;
    player.deck[8] = CARD_ID_DEFEND;
    player.deck[9] = CARD_ID_BASH;
    player.deck_size = 10;

    player.total_runs++;
}

/* Start combat encounter */
void player_start_combat(void) {
    player.block = 0;
    player.energy = player.max_energy;
    player.hand_size = 0;
    player.discard_size = 0;
    player.draw_pile_size = player.deck_size;

    /* Copy deck to draw pile */
    memcpy(player.hand, player.deck, player.deck_size);
}

/* Start player turn */
void player_start_turn(void) {
    player.energy = player.max_energy;
    player.block = 0; /* Block resets each turn */
}

/* End player turn */
void player_end_turn(void) {
    /* Discard hand - will be implemented with deck management */
}

/* Take damage (block absorbs first) */
void player_take_damage(uint8_t damage) {
    if (damage <= player.block) {
        player.block -= damage;
    } else {
        damage -= player.block;
        player.block = 0;
        if (damage >= player.hp) {
            player.hp = 0;
        } else {
            player.hp -= damage;
        }
    }
}

/* Gain block */
void player_gain_block(uint8_t amount) {
    if (player.block + amount > 255) {
        player.block = 255;
    } else {
        player.block += amount;
    }
}

/* Heal HP */
void player_heal(uint8_t amount) {
    if (player.hp + amount > player.max_hp) {
        player.hp = player.max_hp;
    } else {
        player.hp += amount;
    }
}

/* Check if player can afford to play a card */
uint8_t player_can_play_card(uint8_t card_id) {
    const Card* card = card_get(card_id);
    return player.energy >= card->cost;
}

/* Spend energy */
void player_spend_energy(uint8_t amount) {
    if (amount > player.energy) {
        player.energy = 0;
    } else {
        player.energy -= amount;
    }
}

/* Add card to deck */
void player_add_card_to_deck(uint8_t card_id) {
    if (player.deck_size < MAX_DECK_SIZE) {
        player.deck[player.deck_size] = card_id;
        player.deck_size++;
    }
}

/* Remove card from deck */
void player_remove_card_from_deck(uint8_t card_id) {
    uint8_t i, found = 0;

    for (i = 0; i < player.deck_size; i++) {
        if (found) {
            player.deck[i - 1] = player.deck[i];
        } else if (player.deck[i] == card_id) {
            found = 1;
        }
    }

    if (found) {
        player.deck_size--;
    }
}
