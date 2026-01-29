#include "deck.h"
#include "player.h"
#include "random.h"
#include <string.h>

/* Deck pile arrays */
uint8_t draw_pile[60];
uint8_t draw_pile_size = 0;
uint8_t hand[10];
uint8_t hand_size = 0;
uint8_t discard_pile[60];
uint8_t discard_size = 0;

/* Initialize deck for combat */
void deck_init(void) {
    uint8_t i;

    /* Clear all piles */
    hand_size = 0;
    discard_size = 0;
    draw_pile_size = 0;

    /* Copy player deck to draw pile */
    for (i = 0; i < player.deck_size; i++) {
        draw_pile[i] = player.deck[i];
    }
    draw_pile_size = player.deck_size;

    /* Shuffle draw pile */
    random_shuffle(draw_pile, draw_pile_size);
}

/* Shuffle discard pile into draw pile */
void deck_shuffle_discard_into_draw(void) {
    uint8_t i;

    /* Move discard pile to draw pile */
    for (i = 0; i < discard_size; i++) {
        draw_pile[draw_pile_size + i] = discard_pile[i];
    }
    draw_pile_size += discard_size;
    discard_size = 0;

    /* Shuffle the draw pile */
    random_shuffle(draw_pile, draw_pile_size);
}

/* Draw a single card from draw pile to hand */
uint8_t deck_draw_card(void) {
    /* If draw pile empty, shuffle discard into draw */
    if (draw_pile_size == 0) {
        if (discard_size == 0) {
            return 0; /* No cards left anywhere */
        }
        deck_shuffle_discard_into_draw();
    }

    /* Check hand isn't full */
    if (hand_size >= 10) {
        return 0;
    }

    /* Draw top card */
    draw_pile_size--;
    hand[hand_size] = draw_pile[draw_pile_size];
    hand_size++;

    return 1;
}

/* Discard entire hand */
void deck_discard_hand(void) {
    uint8_t i;

    for (i = 0; i < hand_size; i++) {
        discard_pile[discard_size] = hand[i];
        discard_size++;
    }

    hand_size = 0;
}

/* Discard a specific card from hand */
void deck_discard_card(uint8_t hand_index) {
    uint8_t i;

    if (hand_index >= hand_size) {
        return;
    }

    /* Add to discard pile */
    discard_pile[discard_size] = hand[hand_index];
    discard_size++;

    /* Shift remaining cards down */
    for (i = hand_index; i < hand_size - 1; i++) {
        hand[i] = hand[i + 1];
    }
    hand_size--;
}

/* Play a card from hand (discard it) */
void deck_play_card(uint8_t hand_index) {
    deck_discard_card(hand_index);
}

/* Draw cards until hand reaches target size */
void deck_draw_to_hand_size(uint8_t target_size) {
    while (hand_size < target_size) {
        if (!deck_draw_card()) {
            break; /* No more cards to draw */
        }
    }
}
