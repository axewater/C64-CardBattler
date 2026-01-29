#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdint.h>

#define SAVE_FILE_NAME "cardbattle.sav"

/* Save file structure (64 bytes) */
typedef struct {
    char signature[4];    /* "CARD" */
    uint8_t version;      /* Save file version */
    uint16_t unlocks;     /* Card unlock bitmask */
    uint8_t total_runs;
    uint8_t total_wins;
    uint8_t reserved[54]; /* Future expansion */
    uint8_t checksum;
} SaveFile;

/* Persistence Functions */
uint8_t save_game(void);
uint8_t load_game(void);
void save_apply_to_player(SaveFile* save);
void save_create_from_player(SaveFile* save);

#endif /* PERSISTENCE_H */
