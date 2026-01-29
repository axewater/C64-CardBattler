#include "gamestate.h"
#include "ui.h"
#include "player.h"
#include "enemy.h"
#include "encounter.h"
#include "combat.h"
#include "persistence.h"
#include "random.h"
#include "cards.h"
#include "music.h"
#include <conio.h>

static GameState current_state = STATE_TITLE;

/* State function pointer type */
typedef void (*StateFunc)(void);

/* State function table */
static const StateFunc state_functions[] = {
    state_title,
    state_menu,
    state_run_start,
    state_encounter,
    state_combat,
    state_victory,
    state_reward,
    state_defeat,
    state_win,
    state_unlocks
};

void gamestate_init(void) {
    current_state = STATE_TITLE;
}

void gamestate_update(void) {
    if (current_state < STATE_QUIT) {
        state_functions[current_state]();
    }
}

GameState gamestate_get(void) {
    return current_state;
}

void gamestate_set(GameState new_state) {
    current_state = new_state;
}

void state_title(void) {
    ui_clear_screen();
    music_play(TRACK_MENU);

    ui_print_at_color(8, 5, "C64 CARD BATTLER", COLOR_YELLOW);
    ui_print_at_color(12, 7, "ROGUELIKE", COLOR_CYAN);

    ui_print_at(10, 15, "Press any key...");

    ui_wait_key();
    gamestate_set(STATE_MENU);
}

void state_menu(void) {
    ui_clear_screen();

    ui_print_at_color(13, 5, "MAIN MENU", COLOR_YELLOW);

    ui_print_at(12, 10, "[N] New Run");
    ui_print_at(12, 12, "[Q] Quit");

    while (1) {
        uint8_t key;
        music_update();  /* Keep music playing */
        key = ui_get_key();
        if (key == 'n' || key == 'N') {
            gamestate_set(STATE_RUN_START);
            break;
        } else if (key == 'q' || key == 'Q') {
            gamestate_set(STATE_QUIT);
            break;
        }
    }
}

void state_run_start(void) {
    ui_clear_screen();
    music_stop();
    ui_print_at_color(10, 10, "Starting Run...", COLOR_GREEN);
    ui_print_at(10, 12, "Init player...");

    /* Initialize player for new run */
    player_start_run();

    ui_print_at(10, 13, "Init encounter...");
    encounter_init();

    ui_print_at(10, 15, "Press any key");
    ui_wait_key();
    gamestate_set(STATE_ENCOUNTER);
}

void state_encounter(void) {
    uint8_t enemy_id;

    ui_clear_screen();
    ui_print_at_color(10, 10, "Encounter ", COLOR_YELLOW);
    ui_print_number(20, 10, player.run_progress + 1);
    ui_print_at(22, 10, "/5");

    enemy_id = encounter_get_enemy_for_floor(player.run_progress);

    ui_print_at(12, 12, "A ");
    ui_print_at_color(14, 12, enemy_get_name(enemy_id), COLOR_RED);
    ui_print_at(14 + 20, 12, " appears!");

    ui_wait_key();
    gamestate_set(STATE_COMBAT);
}

void state_combat(void) {
    uint8_t enemy_id = encounter_get_enemy_for_floor(player.run_progress);

    /* Initialize combat */
    combat_init(enemy_id);

    /* Start epic combat music */
    music_play(TRACK_COMBAT);

    /* Run combat loop until resolution */
    while (combat_get_state() == COMBAT_PLAYER_TURN) {
        combat_run();
    }

    /* Check result */
    if (combat_get_state() == COMBAT_VICTORY) {
        gamestate_set(STATE_VICTORY);
    } else {
        gamestate_set(STATE_DEFEAT);
    }
}

void state_victory(void) {
    ui_clear_screen();
    ui_print_at_color(13, 12, "Victory!", COLOR_GREEN);

    /* Apply rewards */
    encounter_victory_rewards();

    ui_wait_key();

    /* Advance encounter */
    encounter_advance();

    /* Check if run is complete */
    if (encounter_is_run_complete()) {
        player.total_wins++;
        save_game();
        gamestate_set(STATE_WIN);
    } else {
        gamestate_set(STATE_REWARD);
    }
}

void state_reward(void) {
    uint8_t key;
    uint8_t card_choices[3];
    uint8_t i;

    ui_clear_screen();
    ui_print_at_color(12, 2, "Choose a Reward!", COLOR_YELLOW);

    ui_print_at(2, 4, "Healed 10 HP!");

    /* Generate 3 random card choices */
    for (i = 0; i < 3; i++) {
        card_choices[i] = random_range(0, 10);
    }

    ui_print_at(2, 7, "Choose a card to add:");

    for (i = 0; i < 3; i++) {
        ui_print_at(4, 9 + i * 2, "[");
        ui_print_number(5, 9 + i * 2, i + 1);
        ui_print_at(6, 9 + i * 2, "] ");
        ui_print_at_color(8, 9 + i * 2, card_get_name(card_choices[i]), COLOR_CYAN);
    }

    ui_print_at(2, 16, "[4] Skip card");

    /* Wait for choice */
    while (1) {
        key = ui_get_key();
        if (key >= '1' && key <= '3') {
            player_add_card_to_deck(card_choices[key - '1']);
            break;
        } else if (key == '4') {
            break;
        }
    }

    gamestate_set(STATE_ENCOUNTER);
}

void state_defeat(void) {
    ui_clear_screen();
    ui_print_at_color(10, 12, "You were defeated!", COLOR_RED);
    ui_wait_key();
    music_play(TRACK_MENU);
    gamestate_set(STATE_MENU);
}

void state_win(void) {
    ui_clear_screen();
    ui_print_at_color(8, 12, "You won the run!", COLOR_YELLOW);
    ui_wait_key();
    gamestate_set(STATE_UNLOCKS);
}

void state_unlocks(void) {
    ui_clear_screen();
    ui_print_at_color(11, 10, "Cards unlocked!", COLOR_GREEN);

    /* Unlock new cards based on achievement */
    card_unlock(CARD_ID_HEAVY_STRIKE, &player.unlocks);
    card_unlock(CARD_ID_IRON_WALL, &player.unlocks);

    ui_print_at(8, 12, "New cards available!");

    ui_wait_key();
    music_play(TRACK_MENU);
    gamestate_set(STATE_MENU);
}
