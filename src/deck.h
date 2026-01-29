#ifndef DECK_H
#define DECK_H

#include <stdint.h>

/* Deck pile arrays */
extern uint8_t draw_pile[60];
extern uint8_t draw_pile_size;
extern uint8_t hand[10];
extern uint8_t hand_size;
extern uint8_t discard_pile[60];
extern uint8_t discard_size;

/* Deck Functions */
void deck_init(void);
void deck_shuffle_discard_into_draw(void);
uint8_t deck_draw_card(void);
void deck_discard_hand(void);
void deck_discard_card(uint8_t hand_index);
void deck_play_card(uint8_t hand_index);
void deck_draw_to_hand_size(uint8_t target_size);

#endif /* DECK_H */
