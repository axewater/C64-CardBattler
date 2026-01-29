#include "enemy.h"
#include "player.h"
#include "random.h"
#include "effects.h"
#include <conio.h>
#include <string.h>

/* Global current enemy */
Enemy current_enemy;

/* Enemy name table */
static const char* const enemy_names[] = {
    "SLIME",
    "GOBLIN",
    "CULTIST",
    "ELITE ORC",
    "DRAGON BOSS"
};

/* Enemy sprite characters (PETSCII) */
static const uint8_t enemy_sprites[] = {
    81,  /* SLIME - Q in reverse */
    87,  /* GOBLIN - W in reverse */
    69,  /* CULTIST - E in reverse */
    79,  /* ELITE ORC - O in reverse */
    68   /* DRAGON BOSS - D in reverse */
};

/* Initialize enemy by ID */
void enemy_init(uint8_t enemy_id) {
    memset(&current_enemy, 0, sizeof(Enemy));
    current_enemy.id = enemy_id;
    current_enemy.sprite_char = enemy_sprites[enemy_id];
    current_enemy.ai_state = 0;
    current_enemy.pattern_index = 0;

    /* Set stats based on enemy type */
    switch (enemy_id) {
        case ENEMY_ID_SLIME:
            current_enemy.max_hp = 20;
            current_enemy.attack_min = 3;
            current_enemy.attack_max = 6;
            current_enemy.is_elite = 0;
            break;

        case ENEMY_ID_GOBLIN:
            current_enemy.max_hp = 25;
            current_enemy.attack_min = 5;
            current_enemy.attack_max = 8;
            current_enemy.is_elite = 0;
            break;

        case ENEMY_ID_CULTIST:
            current_enemy.max_hp = 30;
            current_enemy.attack_min = 6;
            current_enemy.attack_max = 10;
            current_enemy.is_elite = 0;
            break;

        case ENEMY_ID_ELITE_ORC:
            current_enemy.max_hp = 45;
            current_enemy.attack_min = 8;
            current_enemy.attack_max = 12;
            current_enemy.is_elite = 1;
            break;

        case ENEMY_ID_BOSS_DRAGON:
            current_enemy.max_hp = 80;
            current_enemy.attack_min = 10;
            current_enemy.attack_max = 15;
            current_enemy.is_elite = 1;
            break;

        default:
            current_enemy.max_hp = 20;
            current_enemy.attack_min = 3;
            current_enemy.attack_max = 6;
            break;
    }

    current_enemy.hp = current_enemy.max_hp;
    enemy_calculate_intent();
}

/* Calculate what enemy will do next turn */
void enemy_calculate_intent(void) {
    uint8_t roll;

    /* Simple AI: mostly attack, occasionally defend */
    roll = random_range(0, 100);

    if (roll < 75) {
        /* Attack */
        current_enemy.intent = INTENT_ATTACK;
        current_enemy.intent_value = random_range(
            current_enemy.attack_min,
            current_enemy.attack_max + 1
        );
    } else {
        /* Defend */
        current_enemy.intent = INTENT_DEFEND;
        current_enemy.intent_value = random_range(5, 10);
    }

    /* Elite enemies have more complex patterns */
    if (current_enemy.is_elite) {
        current_enemy.pattern_index = (current_enemy.pattern_index + 1) % 3;

        if (current_enemy.pattern_index == 2) {
            /* Every 3rd turn, big attack */
            current_enemy.intent = INTENT_ATTACK;
            current_enemy.intent_value = current_enemy.attack_max + 3;
        }
    }
}

/* Execute enemy action */
void enemy_execute_action(void) {
    switch (current_enemy.intent) {
        case INTENT_ATTACK:
            player_take_damage(current_enemy.intent_value);

            /* Add damage effects on player - full screen flash! */
            effects_add_screen_flash(COLOR_RED);
            effects_add_damage(18, 1, current_enemy.intent_value, 0);
            effects_add_shake(1, 0, 35, 1);
            break;

        case INTENT_DEFEND:
            enemy_gain_block(current_enemy.intent_value);

            /* Add block effects on enemy - blue screen flash */
            effects_add_screen_flash(COLOR_BLUE);
            effects_add_damage(17, 4, current_enemy.intent_value, 1);
            break;

        default:
            break;
    }

    /* Block resets each turn */
    current_enemy.block = 0;

    /* Calculate next intent */
    enemy_calculate_intent();
}

/* Enemy takes damage */
void enemy_take_damage(uint8_t damage) {
    if (damage <= current_enemy.block) {
        current_enemy.block -= damage;
    } else {
        damage -= current_enemy.block;
        current_enemy.block = 0;
        if (damage >= current_enemy.hp) {
            current_enemy.hp = 0;
        } else {
            current_enemy.hp -= damage;
        }
    }
}

/* Enemy gains block */
void enemy_gain_block(uint8_t amount) {
    if (current_enemy.block + amount > 255) {
        current_enemy.block = 255;
    } else {
        current_enemy.block += amount;
    }
}

/* Check if enemy is dead */
uint8_t enemy_is_dead(void) {
    return current_enemy.hp == 0;
}

/* Get enemy name */
const char* enemy_get_name(uint8_t enemy_id) {
    if (enemy_id < 5) {
        return enemy_names[enemy_id];
    }
    return "UNKNOWN";
}

/* Get PETSCII character for intent display */
uint8_t enemy_get_intent_char(void) {
    switch (current_enemy.intent) {
        case INTENT_ATTACK:
            return 33; /* ! */
        case INTENT_DEFEND:
            return 83; /* S (shield) */
        default:
            return 63; /* ? */
    }
}
