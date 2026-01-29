/*
 * C64 Card Battler Roguelike
 * A proof-of-concept card game for the Commodore 64
 */

#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "gamestate.h"
#include "player.h"
#include "random.h"
#include "persistence.h"
#include "music.h"

int main(void) {
    /* Initialize UI system */
    ui_init();

    /* Initialize random number generator */
    random_init();

    /* Initialize music system */
    music_init();

    /* Initialize player */
    player_init();

    /* Try to load save file */
    load_game();

    /* Initialize game state */
    gamestate_init();

    /* Main game loop */
    while (gamestate_get() != STATE_QUIT) {
        music_update();
        gamestate_update();
    }

    /* Save game before exit */
    save_game();

    /* Exit message */
    ui_clear_screen();
    ui_print_at(10, 12, "Thanks for playing!");

    return 0;
}
