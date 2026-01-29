#include "persistence.h"
#include "player.h"
#include <string.h>
#include <cbm.h>

/* Calculate simple checksum */
static uint8_t calculate_checksum(const SaveFile* save) {
    uint8_t sum = 0;
    uint8_t i;
    const uint8_t* bytes = (const uint8_t*)save;

    /* Sum all bytes except checksum itself */
    for (i = 0; i < sizeof(SaveFile) - 1; i++) {
        sum += bytes[i];
    }

    return sum;
}

/* Save game to disk */
uint8_t save_game(void) {
    SaveFile save;
    int8_t file;

    /* Create save data */
    save_create_from_player(&save);

    /* Calculate checksum */
    save.checksum = calculate_checksum(&save);

    /* Open file for writing */
    cbm_close(2);
    if (cbm_open(2, 8, CBM_WRITE, SAVE_FILE_NAME) != 0) {
        return 0; /* Failed to open */
    }

    /* Write save data */
    if (cbm_write(2, &save, sizeof(SaveFile)) != sizeof(SaveFile)) {
        cbm_close(2);
        return 0; /* Write failed */
    }

    cbm_close(2);
    return 1; /* Success */
}

/* Load game from disk */
uint8_t load_game(void) {
    SaveFile save;
    uint8_t checksum;

    /* Open file for reading */
    cbm_close(2);
    if (cbm_open(2, 8, CBM_READ, SAVE_FILE_NAME) != 0) {
        return 0; /* File doesn't exist or can't open */
    }

    /* Read save data */
    if (cbm_read(2, &save, sizeof(SaveFile)) != sizeof(SaveFile)) {
        cbm_close(2);
        return 0; /* Read failed */
    }

    cbm_close(2);

    /* Verify signature */
    if (save.signature[0] != 'C' || save.signature[1] != 'A' ||
        save.signature[2] != 'R' || save.signature[3] != 'D') {
        return 0; /* Invalid signature */
    }

    /* Verify checksum */
    checksum = calculate_checksum(&save);
    if (checksum != save.checksum) {
        return 0; /* Corrupted save */
    }

    /* Apply save data to player */
    save_apply_to_player(&save);

    return 1; /* Success */
}

/* Apply save data to player */
void save_apply_to_player(SaveFile* save) {
    player.unlocks = save->unlocks;
    player.total_runs = save->total_runs;
    player.total_wins = save->total_wins;
}

/* Create save data from player */
void save_create_from_player(SaveFile* save) {
    memset(save, 0, sizeof(SaveFile));

    /* Set signature */
    save->signature[0] = 'C';
    save->signature[1] = 'A';
    save->signature[2] = 'R';
    save->signature[3] = 'D';

    save->version = 1;
    save->unlocks = player.unlocks;
    save->total_runs = player.total_runs;
    save->total_wins = player.total_wins;
}
