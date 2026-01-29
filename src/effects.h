#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdint.h>

/* Effect Types */
#define EFFECT_TYPE_NONE        0
#define EFFECT_TYPE_FLASH       1
#define EFFECT_TYPE_SHAKE       2
#define EFFECT_TYPE_DAMAGE_NUM  3
#define EFFECT_TYPE_HIGHLIGHT   4
#define EFFECT_TYPE_SCREEN_FLASH 5

/* Maximum simultaneous effects */
#define MAX_ACTIVE_EFFECTS 4

/* Effect Structure (~8 bytes) */
typedef struct {
    uint8_t type;           /* Effect type (FLASH/SHAKE/etc) */
    uint8_t x;              /* X position */
    uint8_t y;              /* Y position */
    uint8_t width;          /* Area width */
    uint8_t height;         /* Area height */
    uint8_t timer;          /* Frames remaining */
    uint8_t param1;         /* Color, value, etc */
    uint8_t param2;         /* Secondary param */
} Effect;

/* Effect System Functions */
void effects_init(void);
void effects_update(void);
void effects_render(void);
uint8_t effects_active_count(void);
uint8_t effects_get_highlighted_card(void);

/* Effect Creation Functions */
void effects_add_flash(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void effects_add_shake(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void effects_add_damage(uint8_t x, uint8_t y, uint8_t value, uint8_t to_enemy);
void effects_add_card_highlight(uint8_t card_index);
void effects_add_screen_flash(uint8_t color);

#endif /* EFFECTS_H */
