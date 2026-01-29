# C64 Card Battler Roguelike

A card-based roguelike game for the Commodore 64, written in C using the cc65 compiler.

## Features

- Turn-based card combat system
- Roguelike progression with permadeath
- Card unlocks that persist between runs
- PETSCII graphics and UI
- 5 encounters per run with increasing difficulty
- Deck building and card management

## Prerequisites

### Required Tools

1. **cc65 Compiler Suite** (v2.19 or later)
   - Download from: https://cc65.github.io/
   - Windows: Use the precompiled binaries
   - Linux/Mac: Build from source or use package manager
   - Must have `cl65`, `ca65`, and `ld65` in your PATH

2. **VICE Emulator** (v3.5 or later)
   - Download from: https://vice-emu.sourceforge.io/
   - Required for testing and running the game
   - The `x64sc` emulator is used (accurate C64 emulation)

3. **GNU Make** or compatible
   - Windows: Install via MinGW, Cygwin, or MSYS2
   - Linux/Mac: Usually pre-installed

### Optional Tools

- **Text editor** with C syntax highlighting (VS Code, vim, etc.)
- **Real C64 hardware** with disk drive or SD2IEC device (for testing on actual hardware)

## Installation

### Installing cc65 on Windows

1. Download the Windows snapshot from https://cc65.github.io/
2. Extract to `C:\cc65` (or your preferred location)
3. Add `C:\cc65\bin` to your system PATH:
   - Right-click "This PC" → Properties → Advanced System Settings
   - Click "Environment Variables"
   - Edit the PATH variable and add `C:\cc65\bin`
4. Open a new command prompt and verify: `cl65 --version`

### Installing VICE on Windows

1. Download VICE from https://vice-emu.sourceforge.io/
2. Run the installer
3. Add VICE to your PATH (e.g., `C:\Program Files\VICE\bin`)
4. Verify with: `x64sc --version`

### Installing on Linux

```bash
# Debian/Ubuntu
sudo apt-get install cc65 vice

# Arch Linux
sudo pacman -S cc65 vice

# From source
git clone https://github.com/cc65/cc65.git
cd cc65
make
sudo make install
```

## Building the Game

1. Clone or download this repository
2. Open a terminal in the `cardbattler` directory
3. Run the build:

```bash
make
```

This will:
- Compile all `.c` files in `src/` to object files
- Link them into `build/cardbattle.prg`
- Generate a memory map at `build/cardbattle.map`

### Build Output

- **build/cardbattle.prg** - The C64 program file (under 40KB)
- **build/cardbattle.map** - Memory map showing symbol locations
- **build/*.o** - Object files for each C source file

## Running the Game

### In VICE Emulator

```bash
make run
```

Or manually:
```bash
x64sc build/cardbattle.prg
```

### On Real C64 Hardware

1. Transfer `build/cardbattle.prg` to a C64-compatible disk or SD card
2. On the C64, type:
   ```
   LOAD "CARDBATTLE.PRG",8,1
   RUN
   ```

### Controls

- **Arrow Keys / WASD** - Navigate menus
- **Number Keys (1-9)** - Select cards or options
- **P** - Play selected card
- **E** - End turn
- **ESC** - Return to menu
- **SPACE** - Confirm selections

## Project Structure

```
cardbattler/
├── Makefile                    # Build automation
├── c64-cardbattle.cfg          # cc65 linker configuration
├── README.md                   # This file
├── src/
│   ├── main.c                  # Entry point, main game loop
│   ├── gamestate.c/h           # State machine (menu, combat, rewards)
│   ├── cards.c/h               # Card definitions & operations
│   ├── combat.c/h              # Turn-based combat system
│   ├── deck.c/h                # Deck management (draw, discard, shuffle)
│   ├── enemy.c/h               # Enemy AI and behavior
│   ├── player.c/h              # Player stats, health, energy
│   ├── encounter.c/h           # Encounter generation & progression
│   ├── ui.c/h                  # PETSCII rendering functions
│   ├── persistence.c/h         # Save/load unlock system
│   └── random.c/h              # RNG for procedural generation
└── build/                      # Generated build artifacts
```

## Memory Budget

The C64 has limited memory (~38KB usable). The game is designed to fit within:
- **~20KB** Code
- **~8KB** BSS (uninitialized data)
- **~6KB** Data (initialized data)
- **~4KB** Stack/heap

Check current memory usage:
```bash
cat build/cardbattle.map | grep "Segment"
```

## Development

### Adding New Cards

Edit `src/cards.c` and add entries to the `card_database[]` array:

```c
{ CARD_ID_NEWCARD, 1, 8, 0, CARD_TYPE_ATTACK, 0, TARGET_ENEMY, 0 }
```

### Modifying Game Balance

- Card stats: `src/cards.c`
- Enemy stats: `src/enemy.c`
- Player starting HP/Energy: `src/player.c`
- Encounter difficulty: `src/encounter.c`

### Debugging

1. Build with debug info: `cl65 -g ...`
2. Use VICE monitor: Run VICE, then `ALT+H` to open monitor
3. Set breakpoints: `break $address`
4. Step through code: `z` (step), `g` (go), `r` (registers)

### Memory Profiling

In VICE:
1. Open monitor (`ALT+H`)
2. Check memory usage: `i` (show I/O status)
3. Dump memory regions: `d 0800 9fff`

## Troubleshooting

### Build Errors

**"cl65: Command not found"**
- cc65 is not installed or not in PATH
- Solution: Install cc65 and add to PATH

**"Error: Cannot open 'c64-cardbattle.cfg'"**
- Wrong directory or missing config file
- Solution: Run `make` from the `cardbattler` directory

**"Error: Unresolved external"**
- Missing function implementation or wrong link order
- Solution: Check that all `.c` files are being compiled

### Runtime Errors

**Program crashes immediately**
- Stack overflow or memory corruption
- Solution: Check `__STACKSIZE__` in `c64-cardbattle.cfg`

**Graphics glitches**
- Writing outside screen memory bounds
- Solution: Check array bounds in `ui.c`

**Game freezes**
- Infinite loop or waiting for input that never comes
- Solution: Use VICE monitor to break and check PC register

## Performance Optimization

- Use `uint8_t` for all counters and variables
- Mark constant data with `const` (stored in ROM)
- Avoid function calls in tight loops (inline critical functions)
- Use lookup tables instead of calculations
- Profile with VICE's cycle counter

## Save File Format

Save data is stored in `cardbattle.sav` on disk (64 bytes):
- Signature: "CARD" (4 bytes)
- Version: 1 (1 byte)
- Total runs: uint8_t
- Total wins: uint8_t
- Unlocks bitmask: uint16_t
- Checksum: uint8_t
- Reserved: 54 bytes

## Known Limitations

- Maximum 60 cards in deck (memory constraint)
- 5 cards in hand (UI constraint)
- No sound effects (planned for future)
- Single save slot

## Credits

Developed using:
- **cc65** by Ullrich von Bassewitz and contributors
- **VICE** by the VICE Team
- **C64** by Commodore International

## License

This is a demonstration project. Feel free to modify and use as you wish.

## Future Enhancements

- [ ] Sound effects using SID chip
- [ ] More card types (Powers, Status effects)
- [ ] Multiple character classes
- [ ] Relic system (passive bonuses)
- [ ] More enemy types
- [ ] Animated transitions
- [ ] High score tracking

## Contributing

This is a proof-of-concept project. Contributions welcome!

1. Fork the repository
2. Create a feature branch
3. Test thoroughly in VICE
4. Submit a pull request

## Contact

For bugs and feature requests, please open an issue on the project repository.

---

**Enjoy your card battling on the C64!**
