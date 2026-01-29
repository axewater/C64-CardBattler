#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

/* SID chip base address */
#define SID_BASE 0xD400

/* Voice 1 registers (Melody) */
#define SID_V1_FREQ_LO  (*((uint8_t*)(SID_BASE + 0)))
#define SID_V1_FREQ_HI  (*((uint8_t*)(SID_BASE + 1)))
#define SID_V1_PW_LO    (*((uint8_t*)(SID_BASE + 2)))
#define SID_V1_PW_HI    (*((uint8_t*)(SID_BASE + 3)))
#define SID_V1_CONTROL  (*((uint8_t*)(SID_BASE + 4)))
#define SID_V1_AD       (*((uint8_t*)(SID_BASE + 5)))
#define SID_V1_SR       (*((uint8_t*)(SID_BASE + 6)))

/* Voice 2 registers (Harmony) */
#define SID_V2_FREQ_LO  (*((uint8_t*)(SID_BASE + 7)))
#define SID_V2_FREQ_HI  (*((uint8_t*)(SID_BASE + 8)))
#define SID_V2_PW_LO    (*((uint8_t*)(SID_BASE + 9)))
#define SID_V2_PW_HI    (*((uint8_t*)(SID_BASE + 10)))
#define SID_V2_CONTROL  (*((uint8_t*)(SID_BASE + 11)))
#define SID_V2_AD       (*((uint8_t*)(SID_BASE + 12)))
#define SID_V2_SR       (*((uint8_t*)(SID_BASE + 13)))

/* Voice 3 registers (Bass) */
#define SID_V3_FREQ_LO  (*((uint8_t*)(SID_BASE + 14)))
#define SID_V3_FREQ_HI  (*((uint8_t*)(SID_BASE + 15)))
#define SID_V3_PW_LO    (*((uint8_t*)(SID_BASE + 16)))
#define SID_V3_PW_HI    (*((uint8_t*)(SID_BASE + 17)))
#define SID_V3_CONTROL  (*((uint8_t*)(SID_BASE + 18)))
#define SID_V3_AD       (*((uint8_t*)(SID_BASE + 19)))
#define SID_V3_SR       (*((uint8_t*)(SID_BASE + 20)))

/* Global registers */
#define SID_FILTER_FC_LO (*((uint8_t*)(SID_BASE + 21)))
#define SID_FILTER_FC_HI (*((uint8_t*)(SID_BASE + 22)))
#define SID_FILTER_RES   (*((uint8_t*)(SID_BASE + 23)))
#define SID_FILTER_MODE  (*((uint8_t*)(SID_BASE + 24)))
#define SID_VOLUME       (*((uint8_t*)(SID_BASE + 24)))

/* Waveform control bits */
#define SID_WAVE_TRIANGLE 0x10
#define SID_WAVE_SAWTOOTH 0x20
#define SID_WAVE_PULSE    0x40
#define SID_WAVE_NOISE    0x80
#define SID_GATE          0x01
#define SID_SYNC          0x02
#define SID_RING          0x04
#define SID_TEST          0x08

/* Track IDs */
#define TRACK_MENU 0
#define TRACK_COMBAT 1

/* Note index constants (0 = rest) */
#define NOTE_REST 0

/* Public API */
void music_init(void);
void music_update(void);
void music_play(uint8_t track_id);
void music_stop(void);

#endif /* MUSIC_H */
