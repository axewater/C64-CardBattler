#include "music.h"

/* Note frequency table for 4 octaves (48 notes) */
/* Note index: 0=rest, 1-12=octave2, 13-24=octave3, 25-36=octave4, 37-48=octave5 */
static const uint16_t note_frequencies[48] = {
    /* Octave 2: A2-G#2 */
    1076, 1140, 1208, 1280, 1356, 1437, 1523, 1614, 1710, 1812, 1920, 2034,
    /* Octave 3: A3-G#3 */
    2152, 2281, 2416, 2559, 2711, 2873, 3046, 3229, 3421, 3625, 3840, 4068,
    /* Octave 4: A4-G#4 */
    4305, 4562, 4833, 5119, 5423, 5746, 6092, 6458, 6843, 7251, 7680, 8137,
    /* Octave 5: A5-G#5 */
    8610, 9125, 9667, 10239, 10847, 11493, 12184, 12916, 13686, 14502, 15361, 16274
};

/* Music data structures */
typedef struct {
    uint8_t note;      /* Note index (0=rest, 1-48=notes) */
    uint8_t duration;  /* Ticks to hold */
} Note;

typedef struct {
    const Note* notes;
    uint8_t length;
    uint8_t loop_point;
} Pattern;

typedef struct {
    const Pattern* voice1_pattern;
    const Pattern* voice2_pattern;
    const Pattern* voice3_pattern;
    uint8_t ticks_per_note;
} Track;

typedef struct {
    uint8_t playing;
    uint8_t tick_counter;
    uint8_t voice1_pos;
    uint8_t voice1_ticks;
    uint8_t voice2_pos;
    uint8_t voice2_ticks;
    uint8_t voice3_pos;
    uint8_t voice3_ticks;
    const Track* current_track;
    uint8_t voice1_gate_active;
    uint8_t voice2_gate_active;
    uint8_t voice3_gate_active;
    uint8_t voice1_last_note;
    uint8_t voice2_last_note;
    uint8_t voice3_last_note;
} MusicPlayer;

/* Dark menu tune composition - A minor progression */

/* Voice 1: Sparse ethereal melody (Triangle wave) */
/* Notes: A4=25, C5=28, D5=30, E5=31, G4=23, A5=37 */
static const Note menu_voice1_notes[] = {
    {25, 120}, {0, 60}, {31, 30}, {28, 30},  /* A4 whole | Rest half, E5 qtr, C5 qtr */
    {25, 60}, {23, 30}, {31, 90}, {0, 30},   /* A4 half, G4 qtr | E5 dotted-half, Rest qtr */
    {28, 60}, {31, 60}, {37, 60}, {23, 60},  /* C5 half, E5 half | A5 half, G5 half */
    {31, 120}, {0, 60}, {30, 30}, {28, 30}   /* E5 whole | Rest half, D5 qtr, C5 qtr */
};

/* Voice 2: Sustained harmonic pads (Pulse wave) */
/* Notes: C4=16, F3=8, G3=11 */
static const Note menu_voice2_notes[] = {
    {16, 240}, {16, 240},  /* C4 whole x 2 (Am chord foundation) */
    {8, 240}, {8, 240},    /* F3 whole x 2 (Dm chord) */
    {11, 240}, {11, 240},  /* G3 whole x 2 (Em chord) */
    {16, 240}, {16, 240}   /* C4 whole x 2 (Am return) */
};

/* Voice 3: Deep bass punctuation (Sawtooth wave) */
/* Notes: A2=1, D2=6, E2=7 */
static const Note menu_voice3_notes[] = {
    {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30},  /* A2 pattern x 4 */
    {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30},
    {6, 30}, {0, 30}, {6, 30}, {0, 30}, {6, 30}, {0, 30}, {6, 30}, {0, 30},  /* D2 pattern x 4 */
    {6, 30}, {0, 30}, {6, 30}, {0, 30}, {6, 30}, {0, 30}, {6, 30}, {0, 30},
    {7, 30}, {0, 30}, {7, 30}, {0, 30}, {7, 30}, {0, 30}, {7, 30}, {0, 30},  /* E2 pattern x 4 */
    {7, 30}, {0, 30}, {7, 30}, {0, 30}, {7, 30}, {0, 30}, {7, 30}, {0, 30},
    {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30},  /* A2 return x 4 */
    {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30}, {1, 30}, {0, 30}
};

static const Pattern menu_voice1_pattern = {
    menu_voice1_notes,
    sizeof(menu_voice1_notes) / sizeof(Note),
    0
};

static const Pattern menu_voice2_pattern = {
    menu_voice2_notes,
    sizeof(menu_voice2_notes) / sizeof(Note),
    0
};

static const Pattern menu_voice3_pattern = {
    menu_voice3_notes,
    sizeof(menu_voice3_notes) / sizeof(Note),
    0
};

static const Track menu_track = {
    &menu_voice1_pattern,
    &menu_voice2_pattern,
    &menu_voice3_pattern,
    1  /* ticks per note base (actual duration comes from note data) */
};

/* Epic combat music - D minor battle theme (60 BPM, 15 ticks per 16th note) */

/* Voice 1: Dense heroic melody (Triangle wave) - D minor scale */
/* Notes: D4=18, F4=20, A4=25, C5=28, D5=30, E5=31, F5=32 */
static const Note combat_voice1_notes[] = {
    /* Intro: Rising arpeggios (8 measures) */
    {18, 15}, {20, 15}, {25, 15}, {30, 15}, {25, 15}, {20, 15}, {18, 15}, {20, 15},  /* D-F-A-D'-A-F-D-F */
    {25, 15}, {30, 15}, {32, 15}, {30, 15}, {28, 15}, {25, 15}, {20, 15}, {18, 15},  /* A-D'-F'-D'-C'-A-F-D */
    {30, 30}, {28, 15}, {25, 15}, {30, 30}, {32, 15}, {30, 15},                      /* D' half, C' 8th, A 8th, D' half, F' 8th, D' 8th */
    {28, 30}, {25, 15}, {20, 15}, {25, 60},                                          /* C' half, A 8th, F 8th, A whole */

    /* Main theme: Driving melody (8 measures) */
    {30, 15}, {30, 15}, {32, 15}, {30, 15}, {28, 15}, {30, 15}, {25, 15}, {28, 15},  /* D'-D'-F'-D'-C'-D'-A-C' */
    {30, 30}, {25, 15}, {20, 15}, {25, 30}, {18, 30},                                /* D' half, A 8th, F 8th, A half, D half */
    {30, 15}, {32, 15}, {31, 15}, {30, 15}, {28, 15}, {25, 15}, {28, 15}, {30, 15},  /* D'-F'-E'-D'-C'-A-C'-D' */
    {32, 60}, {30, 30}, {28, 30},                                                    /* F' whole, D' half, C' half */

    /* Bridge: Heroic intervals (8 measures) */
    {25, 30}, {30, 30}, {25, 30}, {28, 30},                                          /* A half, D' half, A half, C' half */
    {20, 30}, {32, 30}, {20, 30}, {30, 30},                                          /* F half, F' half, F half, D' half */
    {25, 15}, {28, 15}, {30, 15}, {32, 15}, {30, 15}, {28, 15}, {25, 15}, {20, 15},  /* A-C'-D'-F'-D'-C'-A-F */
    {18, 60}, {25, 30}, {30, 30},                                                    /* D whole, A half, D' half */

    /* Climax: Rhythmic drive (8 measures) */
    {30, 15}, {0, 15}, {30, 15}, {32, 15}, {30, 15}, {0, 15}, {28, 15}, {30, 15},    /* D' rest D' F' D' rest C' D' */
    {25, 15}, {0, 15}, {25, 15}, {28, 15}, {25, 15}, {0, 15}, {20, 15}, {25, 15},    /* A rest A C' A rest F A */
    {30, 30}, {32, 30}, {31, 30}, {30, 30},                                          /* D' half, F' half, E' half, D' half */
    {28, 60}, {30, 60}                                                               /* C' whole, D' whole (loop) */
};

/* Voice 2: Harmonic countermelody (Pulse wave) - Thirds and fifths */
/* Notes: F3=8, A3=13, C4=16, D4=18, F4=20, A4=25 */
static const Note combat_voice2_notes[] = {
    /* Intro harmony (8 measures) */
    {13, 60}, {16, 30}, {18, 30}, {20, 60}, {16, 60},                               /* A3 whole, C4 half, D4 half, F4 whole, C4 whole */
    {13, 60}, {20, 60}, {18, 120},                                                  /* A3 whole, F4 whole, D4 double-whole */

    /* Main theme support (8 measures) */
    {18, 30}, {20, 30}, {25, 30}, {20, 30},                                         /* D4-F4-A4-F4 (power chord movement) */
    {16, 60}, {13, 60},                                                             /* C4 whole, A3 whole */
    {20, 30}, {25, 30}, {20, 30}, {18, 30},                                         /* F4-A4-F4-D4 */
    {16, 60}, {18, 60},                                                             /* C4 whole, D4 whole */

    /* Bridge harmony (8 measures) */
    {13, 60}, {18, 60},                                                             /* A3 whole, D4 whole */
    {8, 60}, {20, 60},                                                              /* F3 whole, F4 whole */
    {13, 30}, {16, 30}, {18, 30}, {20, 30},                                         /* A3-C4-D4-F4 (half notes) */
    {13, 120},                                                                      /* A3 double-whole */

    /* Climax harmony (8 measures) */
    {18, 30}, {20, 30}, {18, 30}, {16, 30},                                         /* D4-F4-D4-C4 */
    {13, 30}, {16, 30}, {13, 30}, {8, 30},                                          /* A3-C4-A3-F3 */
    {18, 60}, {20, 60},                                                             /* D4 whole, F4 whole */
    {16, 60}, {18, 60}                                                              /* C4 whole, D4 whole (loop) */
};

/* Voice 3: Solid bass foundation (Sawtooth wave) - Quarter notes, NO constant retriggering */
/* Notes: D2=6, F2=8, C2=4, A2=1 */
static const Note combat_voice3_notes[] = {
    /* Intro bass (8 measures) - Root progression */
    {6, 60}, {6, 60}, {8, 60}, {8, 60},                                             /* D2 whole x2, F2 whole x2 */
    {4, 60}, {4, 60}, {1, 60}, {1, 60},                                             /* C2 whole x2, A2 whole x2 */

    /* Main theme bass (8 measures) - Steady quarter notes */
    {6, 60}, {8, 60}, {4, 60}, {1, 60},                                             /* D-F-C-A progression */
    {6, 60}, {8, 60}, {4, 60}, {1, 60},                                             /* Repeat */

    /* Bridge bass (8 measures) */
    {1, 60}, {1, 60}, {8, 60}, {8, 60},                                             /* A2 whole x2, F2 whole x2 */
    {6, 60}, {6, 60}, {1, 60}, {1, 60},                                             /* D2 whole x2, A2 whole x2 */

    /* Climax bass (8 measures) - Driving rhythm */
    {6, 60}, {8, 60}, {4, 60}, {1, 60},                                             /* D-F-C-A progression */
    {6, 60}, {8, 60}, {4, 60}, {6, 60}                                              /* D-F-C-D (loop to D) */
};

static const Pattern combat_voice1_pattern = {
    combat_voice1_notes,
    sizeof(combat_voice1_notes) / sizeof(Note),
    0
};

static const Pattern combat_voice2_pattern = {
    combat_voice2_notes,
    sizeof(combat_voice2_notes) / sizeof(Note),
    0
};

static const Pattern combat_voice3_pattern = {
    combat_voice3_notes,
    sizeof(combat_voice3_notes) / sizeof(Note),
    0
};

static const Track combat_track = {
    &combat_voice1_pattern,
    &combat_voice2_pattern,
    &combat_voice3_pattern,
    1  /* ticks per note base */
};

/* Global music player state */
static MusicPlayer music_player;

/* Helper: Set voice frequency with smart gate handling to prevent clicks */
static void set_voice_frequency(uint8_t voice, uint8_t note_index) {
    uint16_t freq;
    uint8_t freq_lo;
    uint8_t freq_hi;
    uint8_t* gate_active;
    uint8_t* last_note;
    uint8_t need_retrigger;

    /* Get state pointers for this voice */
    if (voice == 1) {
        gate_active = &music_player.voice1_gate_active;
        last_note = &music_player.voice1_last_note;
    } else if (voice == 2) {
        gate_active = &music_player.voice2_gate_active;
        last_note = &music_player.voice2_last_note;
    } else if (voice == 3) {
        gate_active = &music_player.voice3_gate_active;
        last_note = &music_player.voice3_last_note;
    } else {
        return;
    }

    /* Handle rests */
    if (note_index == 0 || note_index > 48) {
        /* Only clear gate if currently active (let release complete) */
        if (*gate_active) {
            if (voice == 1) {
                SID_V1_CONTROL &= ~SID_GATE;
            } else if (voice == 2) {
                SID_V2_CONTROL &= ~SID_GATE;
            } else if (voice == 3) {
                SID_V3_CONTROL &= ~SID_GATE;
            }
            *gate_active = 0;
        }
        *last_note = 0;
        return;
    }

    /* Get frequency for this note */
    freq = note_frequencies[note_index - 1];
    freq_lo = (uint8_t)(freq & 0xFF);
    freq_hi = (uint8_t)((freq >> 8) & 0xFF);

    /* Determine if we need to retrigger the gate:
     * - Gate was off (coming from rest) -> always retrigger
     * - Voice 3 (bass) -> always retrigger for percussive effect
     * - Otherwise (note-to-note on melody/harmony) -> legato, no retrigger
     */
    need_retrigger = (!(*gate_active)) || (voice == 3);

    if (voice == 1) {
        if (need_retrigger) {
            SID_V1_CONTROL &= ~SID_GATE;  /* Clear gate */
        }
        SID_V1_FREQ_LO = freq_lo;
        SID_V1_FREQ_HI = freq_hi;
        if (need_retrigger) {
            SID_V1_CONTROL |= SID_GATE;   /* Set gate */
        }
    } else if (voice == 2) {
        if (need_retrigger) {
            SID_V2_CONTROL &= ~SID_GATE;
        }
        SID_V2_FREQ_LO = freq_lo;
        SID_V2_FREQ_HI = freq_hi;
        if (need_retrigger) {
            SID_V2_CONTROL |= SID_GATE;
        }
    } else if (voice == 3) {
        if (need_retrigger) {
            SID_V3_CONTROL &= ~SID_GATE;
        }
        SID_V3_FREQ_LO = freq_lo;
        SID_V3_FREQ_HI = freq_hi;
        if (need_retrigger) {
            SID_V3_CONTROL |= SID_GATE;
        }
    }

    /* Update state */
    *gate_active = 1;
    *last_note = note_index;
}

void music_init(void) {
    uint8_t i;

    /* Silence all SID registers */
    for (i = 0; i < 25; i++) {
        (*((uint8_t*)(SID_BASE + i))) = 0;
    }

    /* Set master volume to 12 (out of 15) */
    SID_VOLUME = 12;

    /* Configure Voice 1: Triangle wave, ethereal melody */
    SID_V1_AD = 0x35;  /* Attack=3, Decay=5 */
    SID_V1_SR = 0x95;  /* Sustain=9, Release=5 */
    SID_V1_CONTROL = SID_WAVE_TRIANGLE;

    /* Configure Voice 2: Pulse wave, sustained pads */
    SID_V2_AD = 0x78;  /* Attack=7, Decay=8 */
    SID_V2_SR = 0x78;  /* Sustain=7, Release=8 */
    SID_V2_PW_LO = 0x00;  /* Pulse width 50% */
    SID_V2_PW_HI = 0x08;
    SID_V2_CONTROL = SID_WAVE_PULSE;

    /* Configure Voice 3: Sawtooth wave, deep bass */
    SID_V3_AD = 0x08;  /* Attack=0, Decay=8 */
    SID_V3_SR = 0x04;  /* Sustain=0, Release=4 (percussive) */
    SID_V3_CONTROL = SID_WAVE_SAWTOOTH;

    /* Initialize player state */
    music_player.playing = 0;
    music_player.tick_counter = 0;
    music_player.voice1_pos = 0;
    music_player.voice1_ticks = 0;
    music_player.voice2_pos = 0;
    music_player.voice2_ticks = 0;
    music_player.voice3_pos = 0;
    music_player.voice3_ticks = 0;
    music_player.current_track = 0;
    music_player.voice1_gate_active = 0;
    music_player.voice2_gate_active = 0;
    music_player.voice3_gate_active = 0;
    music_player.voice1_last_note = 0;
    music_player.voice2_last_note = 0;
    music_player.voice3_last_note = 0;
}

void music_update(void) {
    const Pattern* pattern;
    const Note* note;

    if (!music_player.playing || !music_player.current_track) {
        return;
    }

    /* Update Voice 1 */
    pattern = music_player.current_track->voice1_pattern;
    if (music_player.voice1_ticks == 0) {
        note = &pattern->notes[music_player.voice1_pos];
        set_voice_frequency(1, note->note);
        music_player.voice1_ticks = note->duration;
        music_player.voice1_pos++;
        if (music_player.voice1_pos >= pattern->length) {
            music_player.voice1_pos = pattern->loop_point;
        }
    }
    music_player.voice1_ticks--;

    /* Update Voice 2 */
    pattern = music_player.current_track->voice2_pattern;
    if (music_player.voice2_ticks == 0) {
        note = &pattern->notes[music_player.voice2_pos];
        set_voice_frequency(2, note->note);
        music_player.voice2_ticks = note->duration;
        music_player.voice2_pos++;
        if (music_player.voice2_pos >= pattern->length) {
            music_player.voice2_pos = pattern->loop_point;
        }
    }
    music_player.voice2_ticks--;

    /* Update Voice 3 */
    pattern = music_player.current_track->voice3_pattern;
    if (music_player.voice3_ticks == 0) {
        note = &pattern->notes[music_player.voice3_pos];
        set_voice_frequency(3, note->note);
        music_player.voice3_ticks = note->duration;
        music_player.voice3_pos++;
        if (music_player.voice3_pos >= pattern->length) {
            music_player.voice3_pos = pattern->loop_point;
        }
    }
    music_player.voice3_ticks--;
}

void music_play(uint8_t track_id) {
    /* Select track */
    if (track_id == TRACK_MENU) {
        music_player.current_track = &menu_track;
    } else if (track_id == TRACK_COMBAT) {
        music_player.current_track = &combat_track;
    } else {
        return;
    }

    /* Reset player state */
    music_player.voice1_pos = 0;
    music_player.voice1_ticks = 0;
    music_player.voice2_pos = 0;
    music_player.voice2_ticks = 0;
    music_player.voice3_pos = 0;
    music_player.voice3_ticks = 0;
    music_player.tick_counter = 0;
    music_player.voice1_gate_active = 0;
    music_player.voice2_gate_active = 0;
    music_player.voice3_gate_active = 0;
    music_player.voice1_last_note = 0;
    music_player.voice2_last_note = 0;
    music_player.voice3_last_note = 0;
    music_player.playing = 1;
}

void music_stop(void) {
    music_player.playing = 0;

    /* Clear gate bits to stop all voices */
    SID_V1_CONTROL &= ~SID_GATE;
    SID_V2_CONTROL &= ~SID_GATE;
    SID_V3_CONTROL &= ~SID_GATE;
}
