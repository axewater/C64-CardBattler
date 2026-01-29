#ifndef UI_H
#define UI_H

#include <stdint.h>

/* C64 Screen and Color Memory */
#define SCREEN_MEM ((uint8_t*)0x0400)
#define COLOR_MEM  ((uint8_t*)0xD800)
#define SCREEN_WIDTH  40
#define SCREEN_HEIGHT 25

/* VIC-II Registers */
#define VIC_RASTER     ((uint8_t*)0xD012)
#define VIC_BORDER     ((uint8_t*)0xD020)
#define VIC_BACKGROUND ((uint8_t*)0xD021)

/* C64 Colors - use system defines from c64.h via conio.h */
/* No need to redefine - they're already in the system headers */

/* PETSCII Characters */
#define PETSCII_HEART     83  /* S in reverse */
#define PETSCII_SPADE     65  /* A in reverse */
#define PETSCII_HLINE     64  /* horizontal line */
#define PETSCII_VLINE     93  /* vertical bar */
#define PETSCII_CORNER_TL 85  /* top-left corner */
#define PETSCII_CORNER_TR 73  /* top-right corner */
#define PETSCII_CORNER_BL 74  /* bottom-left corner */
#define PETSCII_CORNER_BR 75  /* bottom-right corner */
#define PETSCII_SOLID     160 /* Solid block */
#define PETSCII_BANG      33  /* ! for execute */
#define PETSCII_STAR      42  /* * for sparkles */

/* UI Functions */
void ui_init(void);
void ui_clear_screen(void);
void ui_print_at(uint8_t x, uint8_t y, const char* text);
void ui_print_at_color(uint8_t x, uint8_t y, const char* text, uint8_t color);
void ui_print_number(uint8_t x, uint8_t y, uint8_t num);
void ui_draw_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void ui_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t ch, uint8_t color);
void ui_set_char(uint8_t x, uint8_t y, uint8_t ch);
void ui_set_color(uint8_t x, uint8_t y, uint8_t color);
void ui_wait_key(void);
uint8_t ui_get_key(void);
void ui_print_number_at_color(uint8_t x, uint8_t y, uint8_t num, uint8_t color);
void ui_color_region(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void ui_draw_card_frame(uint8_t x, uint8_t y, uint8_t card_id, uint8_t type,
                       uint8_t attack, uint8_t block, uint8_t cost,
                       uint8_t can_afford, uint8_t selected);
void ui_wait_frame(void);

#endif /* UI_H */
