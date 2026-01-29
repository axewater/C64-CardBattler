# C64 Card Battler - Developer Quickstart Guide

## Overview
A deck-building card battler for the Commodore 64, written in C using the cc65 toolchain. Features PETSCII graphics, turn-based combat, 15 unique cards, and visual effects including damage numbers, screen flashes, and animated card frames.

## Prerequisites

### Required Tools
- **cc65 toolchain**: C compiler for 6502/C64 (installed at `C:\bin\cc65`)
- **GNU Make**: Build automation (installed at `C:\bin\GnuWin32\bin`)
- **VICE Emulator**: x64sc for testing (must be in PATH or adjust Makefile)

### Installation Check
```bash
C:\bin\cc65\bin\cl65.exe --version
C:\bin\GnuWin32\bin\make.exe --version
x64sc --version
```

## Quick Start

### Build & Run
```bash
cd C:\Scripts\c64\cardbattler
/c/bin/GnuWin32/bin/make.exe        # Build
/c/bin/GnuWin32/bin/make.exe run    # Build and launch in VICE
/c/bin/GnuWin32/bin/make.exe clean  # Clean build artifacts
```

### Build Output
- **Binary**: `build/cardbattle.prg` (C64 executable)
- **Map File**: `build/cardbattle.map` (memory layout and symbols)
- **Memory Budget**: ~15 KB used of 38 KB available

## Project Structure

```
src/
├── main.c           # Entry point and main loop
├── gamestate.c/h    # Game state machine (menu, combat, rewards)
├── combat.c/h       # Combat system and turn logic
├── cards.c/h        # Card database (15 cards) and definitions
├── deck.c/h         # Deck management, hand, draw/discard piles
├── player.c/h       # Player stats (HP, energy, block, gold)
├── enemy.c/h        # Enemy AI and stats (3 enemy types)
├── encounter.c/h    # Encounter generation and progression
├── effects.c/h      # Visual effects system
├── music.c/h        # SID music system with smart gate handling
├── ui.c/h           # Screen rendering and PETSCII graphics
├── persistence.c/h  # Save/load (basic implementation)
└── random.c/h       # PRNG for procedural generation

build/               # Compiled objects and final .prg
```

## Key Systems

### Card System
- **15 Cards**: 3 starters + 12 unlockables
- **Card Types**: ATTACK (red), SKILL (cyan), POWER (purple)
- **Effects**: DRAW, ENERGY, EXECUTE (bonus damage), EXHAUST
- **Database**: `card_database[]` in `cards.c` (const, stored in ROM)

### Combat Flow
1. **Init**: `combat_init()` - Set up enemy, deck, draw hand
2. **Player Turn**: Play cards (1-5 keys) or end turn (E key)
3. **Card Resolution**: Apply attack/block/effects
4. **Enemy Turn**: Execute enemy intent (attack/defend)
5. **Victory/Defeat**: Transition to reward or game over

### Effects System
- **Max 4 simultaneous effects** (prevent queue overflow)
- **Effect Types**:
  - `FLASH`: Color pulse (6 frames)
  - `SHAKE`: Screen oscillation (6 frames)
  - `DAMAGE_NUM`: Popup number (8 frames)
  - `HIGHLIGHT`: Card selection flash (4 frames)
- **Integration**: Effects update/render in combat loop

### Music System
- **SID chip control**: Direct register manipulation for 3-voice synthesis
- **Smart gate handling**: Eliminates clicking artifacts by using legato playing for note-to-note transitions
- **Two tracks**:
  - `TRACK_MENU`: Dark A minor ambient theme with sparse melody and deep bass punctuation
  - `TRACK_COMBAT`: Epic D minor battle theme with dense melodic motion and heroic intervals
- **Voice assignments**:
  - Voice 1 (Triangle): Ethereal melody / Dense heroic line
  - Voice 2 (Pulse): Sustained pads / Harmonic countermelody
  - Voice 3 (Sawtooth): Deep percussive bass foundation
- **Gate optimization**: Only retriggers when coming from rest or for bass percussion, preventing audible pops

### Memory Layout
- **Screen**: `$0400-$07E7` (1000 bytes, 40x25 chars)
- **Color RAM**: `$D800-$DBE7` (1000 bytes)
- **Program**: `$0801+` (loads at BASIC start)
- **Available**: ~38 KB for code/data (C64 has ~39 KB RAM)

## Adding New Features

### Add a New Card
1. **Define ID**: Add to `cards.h` (e.g., `#define CARD_ID_FIREBALL 15`)
2. **Add Name**: Append to `card_names[]` in `cards.c`
3. **Add Data**: Append to `card_database[]` with stats
4. **Short Name**: Add 5-char name to `card_short_names[]` in `ui.c`
5. **Update Count**: Increment `MAX_CARDS_IN_DATABASE`

### Add a New Effect Type
1. **Define Type**: Add to `effects.h` (e.g., `#define EFFECT_TYPE_PULSE 5`)
2. **Implement Render**: Add case in `effects_render()` in `effects.c`
3. **Create Function**: Add `effects_add_pulse()` creator
4. **Integrate**: Call from combat logic where needed

### Add a New Enemy
1. **Add to Database**: `enemy_database[]` in `enemy.c`
2. **Add Name**: `enemy_names[]` array
3. **Define AI**: Implement in `enemy_calculate_intent()`
4. **Update Count**: Increment `MAX_ENEMY_TYPES`

### Add a New Music Track
1. **Define Track ID**: Add to `music.h` (e.g., `#define TRACK_BOSS 2`)
2. **Create Note Patterns**: Define arrays in `music.c` for each voice (e.g., `boss_voice1_notes[]`)
3. **Create Pattern Structs**: Define `Pattern` structures wrapping note arrays
4. **Create Track Struct**: Define `Track` structure referencing voice patterns
5. **Update `music_play()`**: Add track selection case in `music.c`
6. **Integrate**: Call `music_play(TRACK_ID)` in appropriate game state

## Development Tips

### Debugging
- **Map File**: Check `build/cardbattle.map` for symbol addresses
- **Memory Usage**: Look at segment sizes (CODE, RODATA, BSS)
- **VICE Monitor**: Press ALT+H in VICE for machine language monitor
- **Printf**: Use `cprintf()` from conio.h for debug output

### Performance
- **Direct Memory Write**: Use `SCREEN_MEM[]` and `COLOR_MEM[]` directly
- **Avoid malloc**: Use static arrays (no dynamic allocation)
- **Const Data**: Mark lookup tables as `const` to store in ROM
- **Early Returns**: Check `effects_active_count()` before updating

### Code Style
- **C89 Standard**: cc65 uses ANSI C (no C99 features)
- **Declare at Top**: All variables must be declared at function start
- **8-bit Types**: Use `uint8_t` for most values (0-255)
- **Screen Coords**: X (0-39), Y (0-24) for 40x25 text screen

### Memory Optimization
- **Current Usage**: ~13.2 KB (plenty of headroom)
- **If Tight**: Reduce `MAX_ACTIVE_EFFECTS`, simplify card frames
- **Check Growth**: Monitor map file after changes
- **Const Everything**: Move static data to ROM with `const`

## Common Tasks

### Test a Specific Card
1. Modify `deck_init()` in `deck.c` to add card to starting deck
2. Rebuild and run
3. Play card in first combat encounter

### Change Card Costs
1. Edit `card_database[]` in `cards.c`
2. Change `cost` field (second byte)
3. Rebuild

### Adjust Effect Timing
1. Edit effect creator in `effects.c` (e.g., `effects_add_flash()`)
2. Change `timer` value (higher = longer duration)
3. Rebuild and test (effects run at ~60 FPS)

### Modify Enemy Stats
1. Edit `enemy_database[]` in `enemy.c`
2. Adjust HP, attack ranges, or block values
3. Rebuild

## Build Flags

### Current Flags (in Makefile)
- `-t c64`: Target Commodore 64
- `-O`: Enable basic optimizations
- `-Or`: Enable register optimizations
- `-Cl`: Make local variables static when possible
- `-I$(SRC_DIR)`: Include path for headers
- `-m`: Generate map file

### Debug Build (if needed)
Remove `-O -Or -Cl` and add `-g` for debug symbols (increases size).



## Architecture Decisions

### Why Direct Memory Access?
C64 has memory-mapped video. Writing to `$0400` (screen) and `$D800` (color) is faster than conio functions for bulk updates.

### Why Static Effects Queue?
No dynamic allocation on C64. Fixed-size queue (4 effects max) prevents memory fragmentation and keeps memory usage predictable.

### Why Horizontal Card Layout?
Better use of 40-char screen width. Vertical layout wasted space; horizontal fits 5 cards with room for details.

### Why Frame-Based Animation?
Effects update each frame in the input loop (non-blocking). Allows gameplay to continue while effects animate.

### Why Smart Gate Handling in Music?
SID chip generates audible clicks/pops when gate bit is cleared and immediately re-set. Original implementation retriggered on every note change, causing crackling. Smart gate handling uses:
- **Legato playing**: For note-to-note transitions on melody/harmony voices, only update frequency registers without touching the gate bit
- **Selective retriggering**: Only retrigger gate when (1) coming from rest (gate was off), or (2) voice 3 bass for percussive effect
- **State tracking**: Track `gate_active` and `last_note` per voice to determine when retriggering is needed
- **Result**: Smooth, professional sound quality with natural ADSR envelope transitions


