#include "combat.h"
#include "player.h"
#include "enemy.h"
#include "deck.h"
#include "cards.h"
#include "ui.h"
#include "effects.h"
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

    /* Initialize effects system */
    effects_init();

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

    /* Clear combat log for new turn */
    strcpy(combat_log, "Your turn!");

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

    /* Highlight the played card */
    effects_add_card_highlight(hand_index);

    /* Apply card effects */
    switch (card->type) {
        case CARD_TYPE_ATTACK:
            damage = card->attack;

            /* EXECUTE effect: bonus damage to low HP */
            if (card->effects & EFFECT_EXECUTE) {
                if (current_enemy.hp <= current_enemy.max_hp / 2) {
                    damage += 10;
                    /* Extra bright flash for execute bonus */
                    effects_add_screen_flash(COLOR_ORANGE);
                }
            }

            enemy_take_damage(damage);

            /* Add attack effects - full screen flash! */
            effects_add_screen_flash(COLOR_RED);
            effects_add_damage(17, 4, damage, 1);

            strcpy(combat_log, "You attack for ");
            /* Would append number here */
            break;

        case CARD_TYPE_SKILL:
            if (card->block > 0) {
                player_gain_block(card->block);

                /* Add block effects - cyan screen flash */
                effects_add_screen_flash(COLOR_CYAN);
                effects_add_damage(32, 0, card->block, 0);

                strcpy(combat_log, "You gain block!");
            }

            /* DRAW effect */
            if (card->effects & EFFECT_DRAW) {
                deck_draw_card();
                deck_draw_card();

                /* Flash screen yellow for card draw */
                effects_add_screen_flash(COLOR_YELLOW);

                strcpy(combat_log, "You draw 2 cards!");
            }

            /* ENERGY effect */
            if (card->effects & EFFECT_ENERGY) {
                player.energy += 2;

                /* Flash screen green for energy */
                effects_add_screen_flash(COLOR_GREEN);

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
    uint8_t hp_before;
    uint8_t damage_dealt;
    uint8_t i;
    char num_buf[4];

    /* Enemy executes action */
    combat_state = COMBAT_ENEMY_TURN;

    /* Store HP before enemy acts */
    hp_before = player.hp;

    /* Enemy acts */
    enemy_execute_action();

    /* Update combat log with results */
    if (current_enemy.intent == INTENT_ATTACK) {
        damage_dealt = hp_before - player.hp;

        /* Build message: "Enemy attacks for X!" */
        strcpy(combat_log, "Enemy attacks for ");

        /* Convert damage to string and append */
        i = 0;
        if (damage_dealt >= 10) {
            num_buf[i++] = '0' + (damage_dealt / 10);
            num_buf[i++] = '0' + (damage_dealt % 10);
        } else {
            num_buf[i++] = '0' + damage_dealt;
        }
        num_buf[i++] = '!';
        num_buf[i] = '\0';

        strcat(combat_log, num_buf);
    } else if (current_enemy.intent == INTENT_DEFEND) {
        /* Build message: "Enemy gains X block!" */
        strcpy(combat_log, "Enemy gains ");

        i = 0;
        if (current_enemy.intent_value >= 10) {
            num_buf[i++] = '0' + (current_enemy.intent_value / 10);
            num_buf[i++] = '0' + (current_enemy.intent_value % 10);
        } else {
            num_buf[i++] = '0' + current_enemy.intent_value;
        }
        num_buf[i] = '\0';

        strcat(combat_log, num_buf);
        strcat(combat_log, " block!");
    }

    /* Re-render to show enemy action results */
    combat_render();

    /* Animate effects and wait for keypress */
    while (effects_active_count() > 0) {
        ui_wait_frame();  /* Wait for next frame (~60 FPS) */
        effects_update();
        effects_render();
    }

    /* Check if player is dead */
    if (player.hp == 0) {
        combat_state = COMBAT_DEFEAT;
        strcpy(combat_log, "You have been defeated!");
        combat_render();
        ui_wait_key();
        return;
    }

    /* Show "Press any key" message */
    ui_print_at_color(1, 22, "Press any key to continue...", COLOR_YELLOW);
    ui_wait_key();

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

    ui_clear_screen();

    /* Status bar - clear and redraw with consistent colors */
    ui_fill_rect(0, 0, 40, 1, ' ', COLOR_BLACK);

    ui_print_at_color(1, 0, "HP:", COLOR_RED);
    ui_print_number_at_color(5, 0, player.hp, COLOR_RED);
    ui_print_at_color(7, 0, "/", COLOR_RED);
    ui_print_number_at_color(8, 0, player.max_hp, COLOR_RED);

    ui_print_at_color(15, 0, "EN:", COLOR_CYAN);
    ui_print_number_at_color(19, 0, player.energy, COLOR_CYAN);
    ui_print_at_color(21, 0, "/", COLOR_CYAN);
    ui_print_number_at_color(22, 0, player.max_energy, COLOR_CYAN);

    ui_print_at_color(27, 0, "BLK:", COLOR_LIGHTBLUE);
    ui_print_number_at_color(32, 0, player.block, COLOR_LIGHTBLUE);

    /* Enemy display */
    ui_print_at_color(15, 3, enemy_get_name(current_enemy.id), COLOR_YELLOW);

    ui_print_at_color(13, 4, "HP:", COLOR_RED);
    ui_print_number_at_color(17, 4, current_enemy.hp, COLOR_RED);
    ui_print_at_color(19, 4, "/", COLOR_RED);
    ui_print_number_at_color(20, 4, current_enemy.max_hp, COLOR_RED);

    /* Enemy intent */
    ui_print_at_color(12, 5, "INTENT: ", COLOR_WHITE);
    if (current_enemy.intent == INTENT_ATTACK) {
        ui_print_at_color(20, 5, "ATTACK ", COLOR_RED);
        ui_print_number_at_color(27, 5, current_enemy.intent_value, COLOR_RED);
    } else if (current_enemy.intent == INTENT_DEFEND) {
        ui_print_at_color(20, 5, "DEFEND ", COLOR_LIGHTBLUE);
        ui_print_number_at_color(27, 5, current_enemy.intent_value, COLOR_LIGHTBLUE);
    }

    /* Combat log */
    ui_print_at_color(1, 8, combat_log, COLOR_GRAY3);

    /* Hand */
    ui_print_at_color(1, 11, "HAND:", COLOR_YELLOW);

    /* Draw cards horizontally with frames (5 cards max, 7 chars wide each) */
    {
        uint8_t highlighted = effects_get_highlighted_card();

        for (i = 0; i < hand_size && i < 5; i++) {
            uint8_t card_x = 2 + i * 8; /* 7 chars + 1 space */
            uint8_t can_afford;
            uint8_t is_selected;

            card = card_get(hand[i]);
            can_afford = player_can_play_card(hand[i]);
            is_selected = (highlighted == i);

            /* Draw card frame */
            ui_draw_card_frame(card_x, 13, card->id, card->type,
                              card->attack, card->block, card->cost,
                              can_afford, is_selected);

            /* Draw selection number below card */
            ui_print_at_color(card_x + 3, 17, "[", COLOR_WHITE);
            ui_print_number(card_x + 4, 17, i + 1);
            ui_print_at_color(card_x + 5, 17, "]", COLOR_WHITE);
        }
    }

    /* Controls */
    ui_print_at(1, 23, "[1-5]Play [E]nd Turn");
}

/* Run combat loop (called from gamestate) */
void combat_run(void) {
    uint8_t key;

    combat_render();

    while (combat_state == COMBAT_PLAYER_TURN) {
        /* Update and render effects */
        effects_update();
        if (effects_active_count() > 0) {
            effects_render();
        }

        key = ui_get_key();

        if (key >= '1' && key <= '5') {
            combat_player_play_card(key - '1');

            /* Let effects animate before redrawing */
            while (effects_active_count() > 0) {
                ui_wait_frame();  /* Wait for next frame (~60 FPS) */
                effects_update();
                effects_render();
            }

            combat_render();
        } else if (key == 'e' || key == 'E') {
            combat_end_turn();
            break;
        }
    }
}
