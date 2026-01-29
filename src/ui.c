#include <conio.h>
#include <string.h>
#include <c64.h>
#include "ui.h"
#include "music.h"

/* External waitvsync function from cc65 library */
extern void waitvsync(void);

/* Screen position to memory offset */
static uint16_t screen_offset(uint8_t x, uint8_t y) {
    return (uint16_t)y * SCREEN_WIDTH + x;
}

void ui_init(void) {
    /* Set background and border to black */
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    ui_clear_screen();
}

void ui_clear_screen(void) {
    clrscr();
    textcolor(COLOR_WHITE);
}

void ui_print_at(uint8_t x, uint8_t y, const char* text) {
    gotoxy(x, y);
    cputs(text);
}

void ui_print_at_color(uint8_t x, uint8_t y, const char* text, uint8_t color) {
    uint16_t offset = screen_offset(x, y);
    uint8_t i = 0;

    while (text[i] != '\0' && x + i < SCREEN_WIDTH) {
        SCREEN_MEM[offset + i] = text[i];
        COLOR_MEM[offset + i] = color;
        i++;
    }
}

void ui_print_number(uint8_t x, uint8_t y, uint8_t num) {
    char buf[4];
    uint8_t i = 0;

    /* Convert number to string (0-255) */
    if (num >= 100) {
        buf[i++] = '0' + (num / 100);
        num %= 100;
    }
    if (num >= 10 || i > 0) {
        buf[i++] = '0' + (num / 10);
        num %= 10;
    }
    buf[i++] = '0' + num;
    buf[i] = '\0';

    ui_print_at(x, y, buf);
}

void ui_draw_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    uint8_t i;

    /* Top border */
    ui_set_char(x, y, PETSCII_CORNER_TL);
    for (i = 1; i < w - 1; i++) {
        ui_set_char(x + i, y, PETSCII_HLINE);
    }
    ui_set_char(x + w - 1, y, PETSCII_CORNER_TR);

    /* Side borders */
    for (i = 1; i < h - 1; i++) {
        ui_set_char(x, y + i, PETSCII_VLINE);
        ui_set_char(x + w - 1, y + i, PETSCII_VLINE);
    }

    /* Bottom border */
    ui_set_char(x, y + h - 1, PETSCII_CORNER_BL);
    for (i = 1; i < w - 1; i++) {
        ui_set_char(x + i, y + h - 1, PETSCII_HLINE);
    }
    ui_set_char(x + w - 1, y + h - 1, PETSCII_CORNER_BR);
}

void ui_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t ch, uint8_t color) {
    uint8_t i, j;

    for (j = 0; j < h; j++) {
        uint16_t offset = screen_offset(x, y + j);
        for (i = 0; i < w; i++) {
            SCREEN_MEM[offset + i] = ch;
            COLOR_MEM[offset + i] = color;
        }
    }
}

void ui_set_char(uint8_t x, uint8_t y, uint8_t ch) {
    SCREEN_MEM[screen_offset(x, y)] = ch;
}

void ui_set_color(uint8_t x, uint8_t y, uint8_t color) {
    COLOR_MEM[screen_offset(x, y)] = color;
}

void ui_wait_frame(void) {
    /* Use built-in waitvsync from cc65 library */
    waitvsync();
}

void ui_wait_key(void) {
    while (!kbhit()) {
        music_update();  /* Keep music playing while waiting */
    }
    cgetc();
}

uint8_t ui_get_key(void) {
    if (kbhit()) {
        return cgetc();
    }
    return 0;
}

void ui_print_number_at_color(uint8_t x, uint8_t y, uint8_t num, uint8_t color) {
    char buf[4];
    uint8_t i = 0;

    /* Convert number to string (0-255) */
    if (num >= 100) {
        buf[i++] = '0' + (num / 100);
        num %= 100;
    }
    if (num >= 10 || i > 0) {
        buf[i++] = '0' + (num / 10);
        num %= 10;
    }
    buf[i++] = '0' + num;
    buf[i] = '\0';

    ui_print_at_color(x, y, buf, color);
}

void ui_color_region(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    uint8_t i, j;
    for (j = 0; j < h; j++) {
        uint16_t offset = screen_offset(x, y + j);
        for (i = 0; i < w; i++) {
            COLOR_MEM[offset + i] = color;
        }
    }
}

/* Shortened card names (5 chars max) for card frames */
static const char* const card_short_names[15] = {
    "STRKE", "DFEND", "BASH ", "HEAVY", "IWALL",
    "DRAW ", "ENRGY", "EXECU", "CLEAV", "SHBSH",
    "POWER", "ARMOR", "QSLSH", "FRTFY", "BSERK"
};

void ui_draw_card_frame(uint8_t x, uint8_t y, uint8_t card_id, uint8_t type,
                       uint8_t attack, uint8_t block, uint8_t cost,
                       uint8_t can_afford, uint8_t selected) {
    uint8_t frame_color;
    uint8_t text_color;
    uint16_t offset;
    uint8_t i;

    /* Determine frame color based on card type */
    if (!can_afford) {
        frame_color = COLOR_GRAY1;
        text_color = COLOR_GRAY2;
    } else if (selected) {
        frame_color = COLOR_YELLOW;
        text_color = COLOR_WHITE;
    } else {
        switch (type) {
            case 0: /* CARD_TYPE_ATTACK */
                frame_color = COLOR_RED;
                text_color = COLOR_WHITE;
                break;
            case 1: /* CARD_TYPE_SKILL */
                frame_color = COLOR_CYAN;
                text_color = COLOR_WHITE;
                break;
            case 2: /* CARD_TYPE_POWER */
                frame_color = COLOR_PURPLE;
                text_color = COLOR_WHITE;
                break;
            default:
                frame_color = COLOR_WHITE;
                text_color = COLOR_WHITE;
                break;
        }
    }

    /* Draw top border: .-----. */
    offset = screen_offset(x, y);
    SCREEN_MEM[offset] = '.';
    COLOR_MEM[offset] = frame_color;
    for (i = 1; i < 6; i++) {
        SCREEN_MEM[offset + i] = '-';
        COLOR_MEM[offset + i] = frame_color;
    }
    SCREEN_MEM[offset + 6] = '.';
    COLOR_MEM[offset + 6] = frame_color;

    /* Draw middle line with card name: |STRKE| */
    offset = screen_offset(x, y + 1);
    SCREEN_MEM[offset] = '|';
    COLOR_MEM[offset] = frame_color;
    for (i = 0; i < 5 && card_id < 15; i++) {
        SCREEN_MEM[offset + 1 + i] = card_short_names[card_id][i];
        COLOR_MEM[offset + 1 + i] = text_color;
    }
    SCREEN_MEM[offset + 6] = '|';
    COLOR_MEM[offset + 6] = frame_color;

    /* Draw stats line: |6   1| (attack on left, cost on right) */
    offset = screen_offset(x, y + 2);
    SCREEN_MEM[offset] = '|';
    COLOR_MEM[offset] = frame_color;

    /* Attack or Block value */
    if (attack > 0) {
        if (attack >= 10) {
            SCREEN_MEM[offset + 1] = '0' + (attack / 10);
            SCREEN_MEM[offset + 2] = '0' + (attack % 10);
        } else {
            SCREEN_MEM[offset + 1] = '0' + attack;
            SCREEN_MEM[offset + 2] = ' ';
        }
        COLOR_MEM[offset + 1] = COLOR_RED;
        COLOR_MEM[offset + 2] = COLOR_RED;
    } else if (block > 0) {
        if (block >= 10) {
            SCREEN_MEM[offset + 1] = '0' + (block / 10);
            SCREEN_MEM[offset + 2] = '0' + (block % 10);
        } else {
            SCREEN_MEM[offset + 1] = '0' + block;
            SCREEN_MEM[offset + 2] = ' ';
        }
        COLOR_MEM[offset + 1] = COLOR_LIGHTBLUE;
        COLOR_MEM[offset + 2] = COLOR_LIGHTBLUE;
    } else {
        SCREEN_MEM[offset + 1] = ' ';
        SCREEN_MEM[offset + 2] = ' ';
        COLOR_MEM[offset + 1] = text_color;
        COLOR_MEM[offset + 2] = text_color;
    }

    /* Spaces in middle */
    SCREEN_MEM[offset + 3] = ' ';
    SCREEN_MEM[offset + 4] = ' ';
    COLOR_MEM[offset + 3] = text_color;
    COLOR_MEM[offset + 4] = text_color;

    /* Cost */
    SCREEN_MEM[offset + 5] = '0' + cost;
    COLOR_MEM[offset + 5] = COLOR_CYAN;

    SCREEN_MEM[offset + 6] = '|';
    COLOR_MEM[offset + 6] = frame_color;

    /* Draw bottom border: `-----' */
    offset = screen_offset(x, y + 3);
    SCREEN_MEM[offset] = '`';
    COLOR_MEM[offset] = frame_color;
    for (i = 1; i < 6; i++) {
        SCREEN_MEM[offset + i] = '-';
        COLOR_MEM[offset + i] = frame_color;
    }
    SCREEN_MEM[offset + 6] = '\'';
    COLOR_MEM[offset + 6] = frame_color;
}
