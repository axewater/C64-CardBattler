#include "effects.h"
#include "ui.h"
#include <conio.h>
#include <string.h>

/* Effect queue */
static Effect effects[MAX_ACTIVE_EFFECTS];

/* Saved screen state for shake effect */
static uint8_t shake_backup[40];
static uint8_t shake_backup_valid = 0;

/* Saved color backup for flash effect */
static uint8_t flash_color_backup[40 * 3]; /* Max 40 width * 3 height */
static uint8_t flash_backup_valid = 0;

/* Saved screen colors for full-screen flash */
static uint8_t saved_border_color = 0;
static uint8_t saved_bg_color = 0;

/* Initialize effects system */
void effects_init(void) {
    uint8_t i;
    for (i = 0; i < MAX_ACTIVE_EFFECTS; i++) {
        effects[i].type = EFFECT_TYPE_NONE;
    }
    shake_backup_valid = 0;
    flash_backup_valid = 0;
}

/* Get number of active effects */
uint8_t effects_active_count(void) {
    uint8_t i;
    uint8_t count = 0;
    for (i = 0; i < MAX_ACTIVE_EFFECTS; i++) {
        if (effects[i].type != EFFECT_TYPE_NONE) {
            count++;
        }
    }
    return count;
}

/* Find free effect slot */
static uint8_t effects_find_free_slot(void) {
    uint8_t i;
    for (i = 0; i < MAX_ACTIVE_EFFECTS; i++) {
        if (effects[i].type == EFFECT_TYPE_NONE) {
            return i;
        }
    }
    return 0xFF; /* No free slot */
}

/* Update all active effects */
void effects_update(void) {
    uint8_t i;

    for (i = 0; i < MAX_ACTIVE_EFFECTS; i++) {
        if (effects[i].type == EFFECT_TYPE_NONE) {
            continue;
        }

        /* Decrement timer */
        if (effects[i].timer > 0) {
            effects[i].timer--;
        }

        /* Remove expired effects */
        if (effects[i].timer == 0) {
            /* Restore shake backup if needed */
            if (effects[i].type == EFFECT_TYPE_SHAKE && shake_backup_valid) {
                uint8_t j;
                uint16_t offset = (uint16_t)effects[i].y * SCREEN_WIDTH + effects[i].x;
                for (j = 0; j < effects[i].width; j++) {
                    SCREEN_MEM[offset + j] = shake_backup[j];
                }
                shake_backup_valid = 0;
            }

            /* Restore flash colors if needed */
            if (effects[i].type == EFFECT_TYPE_FLASH && flash_backup_valid) {
                uint8_t j, k;
                uint16_t offset;
                uint8_t backup_idx = 0;
                for (j = 0; j < effects[i].height && j < 3; j++) {
                    offset = (uint16_t)(effects[i].y + j) * SCREEN_WIDTH + effects[i].x;
                    for (k = 0; k < effects[i].width && k < 40; k++) {
                        COLOR_MEM[offset + k] = flash_color_backup[backup_idx++];
                    }
                }
                flash_backup_valid = 0;
            }

            /* Restore screen colors for full-screen flash */
            if (effects[i].type == EFFECT_TYPE_SCREEN_FLASH) {
                *VIC_BORDER = saved_border_color;
                *VIC_BACKGROUND = saved_bg_color;
            }

            effects[i].type = EFFECT_TYPE_NONE;
        }
    }
}

/* Render all active effects */
void effects_render(void) {
    uint8_t i, j, k;
    uint16_t offset;

    for (i = 0; i < MAX_ACTIVE_EFFECTS; i++) {
        if (effects[i].type == EFFECT_TYPE_NONE) {
            continue;
        }

        switch (effects[i].type) {
            case EFFECT_TYPE_FLASH:
                /* Flash color in region */
                if (!flash_backup_valid) {
                    /* First frame - backup original colors */
                    uint8_t backup_idx = 0;
                    for (j = 0; j < effects[i].height && j < 3; j++) {
                        offset = (uint16_t)(effects[i].y + j) * SCREEN_WIDTH + effects[i].x;
                        for (k = 0; k < effects[i].width && k < 40; k++) {
                            flash_color_backup[backup_idx++] = COLOR_MEM[offset + k];
                        }
                    }
                    flash_backup_valid = 1;
                }

                /* Toggle between flash color and original */
                {
                    uint8_t backup_idx = 0;
                    for (j = 0; j < effects[i].height && j < 3; j++) {
                        offset = (uint16_t)(effects[i].y + j) * SCREEN_WIDTH + effects[i].x;
                        for (k = 0; k < effects[i].width && k < 40; k++) {
                            if ((effects[i].timer & 1) == 0) {
                                /* Even frames: flash color */
                                COLOR_MEM[offset + k] = effects[i].param1;
                            } else {
                                /* Odd frames: restore original */
                                COLOR_MEM[offset + k] = flash_color_backup[backup_idx];
                            }
                            backup_idx++;
                        }
                    }
                }
                break;

            case EFFECT_TYPE_SHAKE:
                /* Shake effect - oscillate position */
                if (effects[i].timer == effects[i].param1 - 1) {
                    /* First frame - backup original */
                    offset = (uint16_t)effects[i].y * SCREEN_WIDTH + effects[i].x;
                    for (j = 0; j < effects[i].width && j < 40; j++) {
                        shake_backup[j] = SCREEN_MEM[offset + j];
                    }
                    shake_backup_valid = 1;
                }

                /* Oscillate */
                if ((effects[i].timer & 1) == 0) {
                    /* Shift right */
                    offset = (uint16_t)effects[i].y * SCREEN_WIDTH + effects[i].x;
                    for (j = effects[i].width - 1; j > 0; j--) {
                        SCREEN_MEM[offset + j] = SCREEN_MEM[offset + j - 1];
                    }
                } else {
                    /* Shift left */
                    offset = (uint16_t)effects[i].y * SCREEN_WIDTH + effects[i].x;
                    for (j = 0; j < effects[i].width - 1; j++) {
                        SCREEN_MEM[offset + j] = SCREEN_MEM[offset + j + 1];
                    }
                }
                break;

            case EFFECT_TYPE_DAMAGE_NUM:
                /* Draw number with color - show for most of duration */
                if (effects[i].timer > 0) {
                    ui_print_number_at_color(effects[i].x, effects[i].y,
                                            effects[i].param2, effects[i].param1);
                }
                break;

            case EFFECT_TYPE_HIGHLIGHT:
                /* Card highlight - handled in card rendering */
                break;

            case EFFECT_TYPE_SCREEN_FLASH:
                /* Full-screen flash - change background and border */
                if (effects[i].timer == effects[i].param2) {
                    /* First frame - save original colors */
                    saved_border_color = *VIC_BORDER;
                    saved_bg_color = *VIC_BACKGROUND;
                }

                /* Toggle between flash color and original */
                if ((effects[i].timer & 1) == 0) {
                    /* Even frames: flash color */
                    *VIC_BORDER = effects[i].param1;
                    *VIC_BACKGROUND = effects[i].param1;
                } else {
                    /* Odd frames: restore original */
                    *VIC_BORDER = saved_border_color;
                    *VIC_BACKGROUND = saved_bg_color;
                }
                break;
        }
    }
}

/* Add flash effect */
void effects_add_flash(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    uint8_t slot = effects_find_free_slot();
    if (slot == 0xFF) return;

    effects[slot].type = EFFECT_TYPE_FLASH;
    effects[slot].x = x;
    effects[slot].y = y;
    effects[slot].width = w;
    effects[slot].height = h;
    effects[slot].timer = 20; /* 20 frames (~0.33 seconds) */
    effects[slot].param1 = color;
    effects[slot].param2 = 0;
}

/* Add shake effect */
void effects_add_shake(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    uint8_t slot = effects_find_free_slot();
    if (slot == 0xFF) return;

    effects[slot].type = EFFECT_TYPE_SHAKE;
    effects[slot].x = x;
    effects[slot].y = y;
    effects[slot].width = w;
    effects[slot].height = h;
    effects[slot].timer = 20; /* 20 frames (~0.33 seconds) */
    effects[slot].param1 = 20; /* Save initial timer for backup */
    effects[slot].param2 = 0;
}

/* Add damage number popup */
void effects_add_damage(uint8_t x, uint8_t y, uint8_t value, uint8_t to_enemy) {
    uint8_t slot = effects_find_free_slot();
    uint8_t color;

    if (slot == 0xFF) return;

    /* Choose color based on context */
    if (to_enemy) {
        color = COLOR_RED;      /* Damage to enemy */
    } else {
        color = COLOR_LIGHTRED; /* Damage to player - bright red */
    }

    effects[slot].type = EFFECT_TYPE_DAMAGE_NUM;
    effects[slot].x = x;
    effects[slot].y = y;
    effects[slot].width = 0;
    effects[slot].height = 0;
    effects[slot].timer = 20; /* 20 frames (~0.33 seconds) - matches flash duration */
    effects[slot].param1 = color;
    effects[slot].param2 = value;
}

/* Add card highlight effect */
void effects_add_card_highlight(uint8_t card_index) {
    uint8_t slot = effects_find_free_slot();
    if (slot == 0xFF) return;

    effects[slot].type = EFFECT_TYPE_HIGHLIGHT;
    effects[slot].x = 0;
    effects[slot].y = 0;
    effects[slot].width = 0;
    effects[slot].height = 0;
    effects[slot].timer = 4; /* 4 frames */
    effects[slot].param1 = card_index;
    effects[slot].param2 = 0;
}

/* Get currently highlighted card (returns 0xFF if none) */
uint8_t effects_get_highlighted_card(void) {
    uint8_t i;
    for (i = 0; i < MAX_ACTIVE_EFFECTS; i++) {
        if (effects[i].type == EFFECT_TYPE_HIGHLIGHT) {
            return effects[i].param1;
        }
    }
    return 0xFF; /* No highlight active */
}

/* Add full-screen flash effect */
void effects_add_screen_flash(uint8_t color) {
    uint8_t slot = effects_find_free_slot();
    if (slot == 0xFF) return;

    effects[slot].type = EFFECT_TYPE_SCREEN_FLASH;
    effects[slot].x = 0;
    effects[slot].y = 0;
    effects[slot].width = 0;
    effects[slot].height = 0;
    effects[slot].timer = 20; /* 20 frames (~0.33 seconds) */
    effects[slot].param1 = color;
    effects[slot].param2 = 20; /* Save initial timer */
}
