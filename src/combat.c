#include "combat.h"
#include "player.h"
#include "enemy.h"
#include "deck.h"
#include "cards.h"
#include "ui.h"
#include <conio.h>
#include <string.h>

static CombatState combat_state;
static char combat_log[40];

/* Initialize combat */
void combat_init(uint8_t enemy_id) {
    /* Initialize enemy */
    enemy_init(enemy_id);

    /* Initialize deck */
    deck_init();

    /* Reset player combat state */
    player.energy = player.max_energy;
    player.block = 0;

    /* Draw starting hand */
    deck_draw_to_hand_size(5);

    combat_state = COMBAT_PLAYER_TURN;
    strcpy(combat_log, "Combat begins!");
}

/* Start a new turn */
void combat_start_turn(void) {
    /* Reset energy and block */
    player.energy = player.max_energy;
    player.block = 0;

    /* Discard hand */
    deck_discard_hand();

    /* Draw new hand */
    deck_draw_to_hand_size(5);

    combat_state = COMBAT_PLAYER_TURN;
}

/* Player plays a card */
void combat_player_play_card(uint8_t hand_index) {
    const Card* card;
    uint8_t card_id;
    uint8_t damage;

    if (hand_index >= hand_size) {
        return;
    }

    card_id = hand[hand_index];
    card = card_get(card_id);

    /* Check if player can afford it */
    if (!player_can_play_card(card_id)) {
        strcpy(combat_log, "Not enough energy!");
        return;
    }

    /* Spend energy */
    player_spend_energy(card->cost);

    /* Apply card effects */
    switch (card->type) {
        case CARD_TYPE_ATTACK:
            damage = card->attack;

            /* EXECUTE effect: bonus damage to low HP */
            if (card->effects & EFFECT_EXECUTE) {
                if (current_enemy.hp <= current_enemy.max_hp / 2) {
                    damage += 10;
                }
            }

            enemy_take_damage(damage);
            strcpy(combat_log, "You attack for ");
            /* Would append number here */
            break;

        case CARD_TYPE_SKILL:
            if (card->block > 0) {
                player_gain_block(card->block);
                strcpy(combat_log, "You gain block!");
            }

            /* DRAW effect */
            if (card->effects & EFFECT_DRAW) {
                deck_draw_card();
                deck_draw_card();
                strcpy(combat_log, "You draw 2 cards!");
            }

            /* ENERGY effect */
            if (card->effects & EFFECT_ENERGY) {
                player.energy += 2;
                strcpy(combat_log, "You gain energy!");
            }
            break;

        default:
            break;
    }

    /* Discard the card */
    deck_play_card(hand_index);

    /* Check if enemy is dead */
    if (enemy_is_dead()) {
        combat_state = COMBAT_VICTORY;
    }
}

/* End player turn */
void combat_end_turn(void) {
    /* Enemy executes action */
    combat_state = COMBAT_ENEMY_TURN;

    /* Enemy acts */
    enemy_execute_action();

    /* Check if player is dead */
    if (player.hp == 0) {
        combat_state = COMBAT_DEFEAT;
        return;
    }

    /* Start new turn */
    combat_start_turn();
}

/* Get current combat state */
CombatState combat_get_state(void) {
    return combat_state;
}

/* Render combat UI */
void combat_render(void) {
    uint8_t i;
    const Card* card;
    char buf[40];

    ui_clear_screen();

    /* Status bar */
    ui_print_at_color(1, 0, "HP:", COLOR_RED);
    ui_print_number(5, 0, player.hp);
    ui_print_at(7, 0, "/");
    ui_print_number(8, 0, player.max_hp);

    ui_print_at_color(15, 0, "EN:", COLOR_CYAN);
    ui_print_number(19, 0, player.energy);
    ui_print_at(21, 0, "/");
    ui_print_number(22, 0, player.max_energy);

    ui_print_at_color(27, 0, "BLK:", COLOR_LIGHTBLUE);
    ui_print_number(32, 0, player.block);

    /* Enemy display */
    ui_print_at_color(15, 3, enemy_get_name(current_enemy.id), COLOR_YELLOW);

    ui_print_at_color(13, 4, "HP:", COLOR_RED);
    ui_print_number(17, 4, current_enemy.hp);
    ui_print_at(19, 4, "/");
    ui_print_number(20, 4, current_enemy.max_hp);

    /* Enemy intent */
    ui_print_at_color(12, 5, "INTENT: ", COLOR_WHITE);
    if (current_enemy.intent == INTENT_ATTACK) {
        ui_print_at_color(20, 5, "ATTACK ", COLOR_RED);
        ui_print_number(27, 5, current_enemy.intent_value);
    } else if (current_enemy.intent == INTENT_DEFEND) {
        ui_print_at_color(20, 5, "DEFEND ", COLOR_LIGHTBLUE);
        ui_print_number(27, 5, current_enemy.intent_value);
    }

    /* Combat log */
    ui_print_at_color(1, 8, combat_log, COLOR_GRAY3);

    /* Hand */
    ui_print_at_color(1, 11, "HAND:", COLOR_YELLOW);

    for (i = 0; i < hand_size && i < 5; i++) {
        card = card_get(hand[i]);

        /* Card number */
        ui_print_at_color(1, 13 + i * 2, "[", COLOR_WHITE);
        ui_print_number(2, 13 + i * 2, i + 1);
        ui_print_at_color(3, 13 + i * 2, "] ", COLOR_WHITE);

        /* Card name */
        ui_print_at_color(5, 13 + i * 2, card_get_name(card->id), COLOR_YELLOW);

        /* Card stats */
        if (card->attack > 0) {
            ui_print_at_color(20, 13 + i * 2, "ATK:", COLOR_RED);
            ui_print_number(24, 13 + i * 2, card->attack);
        }
        if (card->block > 0) {
            ui_print_at_color(27, 13 + i * 2, "BLK:", COLOR_LIGHTBLUE);
            ui_print_number(31, 13 + i * 2, card->block);
        }

        /* Cost */
        ui_print_at_color(34, 13 + i * 2, "[$", COLOR_CYAN);
        ui_print_number(36, 13 + i * 2, card->cost);
        ui_print_at_color(38, 13 + i * 2, "]", COLOR_CYAN);
    }

    /* Controls */
    ui_print_at(1, 23, "[1-5]Play [E]nd Turn");
}

/* Run combat loop (called from gamestate) */
void combat_run(void) {
    uint8_t key;

    combat_render();

    while (combat_state == COMBAT_PLAYER_TURN) {
        key = ui_get_key();

        if (key >= '1' && key <= '5') {
            combat_player_play_card(key - '1');
            combat_render();
        } else if (key == 'e' || key == 'E') {
            combat_end_turn();
            break;
        }
    }
}
