#include <conio.h>
#include <string.h>
#include "ui.h"

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

void ui_wait_key(void) {
    while (!kbhit()) {
        /* Wait for keypress */
    }
    cgetc();
}

uint8_t ui_get_key(void) {
    if (kbhit()) {
        return cgetc();
    }
    return 0;
}
