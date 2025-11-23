# MAZE RUNNER

<div align="center">

![MAZE RUNNER Splash Screen](Screenshots/MAZE_SPLASH_SCREEN.PNG)

**A Retro 3D Raycasting Shooter for MS-DOS**

*Classic DOS Gaming with Custom Pixel Art*

[![License](https://img.shields.io/badge/License-Educational%20Use-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-MS--DOS-green.svg)](https://en.wikipedia.org/wiki/MS-DOS)
[![Language](https://img.shields.io/badge/Language-C-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Version](https://img.shields.io/badge/Version-2.20-blue.svg)](https://github.com/VonHoltenCodes/Von_Holten-Maze-Game)

[Features](#features) • [Screenshots](#screenshots) • [Installation](#installation) • [Building](#building-from-source) • [Technical Details](#technical-details)

</div>

---

## Overview

**MAZE RUNNER v2.20** is a retro-style first-person 3D shooter that runs on genuine MS-DOS and vintage hardware. Battle through three increasingly challenging levels filled with diverse enemies including creepers, spidermen, tomatoes, snowmen, and classic minions using real-time raycasting graphics—the same technology that powered classics like Wolfenstein 3D and Doom. Features a cinematic animated intro and procedural starry sky!

Navigate detailed stone block mazes, manage your ammo and health, and use the mini-map to find the flashing exit. Survive all 3 levels to win!

---

## Features

### Core Gameplay
- 🎮 **Real-time 3D Raycasting Engine** - Authentic Wolfenstein 3D-style rendering
- 🔫 **Combat System** - Shoot purple gremlins and jumping minions
- 🎯 **Ammo Management** - Collect ammo pickups and refills between levels
- ❤️ **Health System** - Monitor your HP and collect health pickups
- 🗺️ **Mini-Map** - Top-right corner map shows maze layout, enemies, and flashing exit
- 📊 **HUD Display** - Organized bottom-left stack (HP bar, ammo, score, accuracy)
- 🎨 **Custom Sprites** - Original minion game pixel art by VonHoltenCodes

### Visual Excellence
- 🧱 **Detailed Stone Walls** - 16×16 pixel blocks with mortar lines, cracks, and 3D beveling
- 🟫 **Realistic Brick Floors** - 8×4 pixel bricks with offset rows and 3D edge effects
- 👾 **Purple Gremlins** - Custom enemy sprites with proper magenta colors
- 🏃 **Jumping Minions** - Secondary enemy type from VonHoltenCodes minion game
- 🔴 **Flashing Exit Walls** - Red/yellow animated exit indicators on mini-map
- 💀 **Death Screen** - Game over screen with restart option (Y) or credits (N)

### Audio Experience
- 🔊 **Super Loud Gun Sounds** - Enhanced 10× volume for satisfying shooting
- 👣 **Footstep Audio** - Sound Blaster support with PC speaker fallback
- 🎵 **Victory Tones** - Level completion celebrations

### Technical Excellence
- 🖥️ **VGA Mode 13h Graphics** - Classic 320×200 resolution, 16 standard colors
- 🖼️ **Double-Buffered Rendering** - Smooth, flicker-free graphics
- 💾 **Tiny Footprint** - Only 149 KB executable (fits on floppy!)
- 🕹️ **Vintage Hardware Compatible** - Runs on 386DX/33 and above
- ⚡ **Optimized Performance** - Smooth gameplay on period-correct hardware

---

## Screenshots

### Gameplay Features
![Gameplay](Screenshots/MAZE_GAMEPLAY.PNG)
*Real-time 3D raycasting with purple gremlins, detailed stone walls, mini-map, and organized HUD*

### Visual Details
- **Stone Walls**: 16×16 pixel blocks with black mortar lines, random cracks, weathering effects
- **Brick Floors**: 8×4 pixel bricks in red/brown/grey with offset rows
- **Mini-Map**: 60×60 pixel overhead view (top-right) with flashing red/yellow exit
- **HUD Stack**: HP bar, ammo counter, score, and accuracy (bottom-left)
- **Enemies**: Purple gremlins and jumping minions from VonHoltenCodes minion game

---

## The Three Levels

### 🟣 Level 1: Gremlin Invasion
- **Theme:** Balanced maze with purple gremlins
- **Difficulty:** Beginner-friendly
- **Features:** Mixed corridors with strategic ammo/health placement
- **Exit:** Flashing wall at (row 22, columns 11-13)

### 🟡 Level 2: Minion Assault
- **Theme:** Wider corridors with jumping minions
- **Difficulty:** Intermediate
- **Features:** More enemies, strategic chokepoints
- **Ammo Refill:** Full 50 ammo at level start

### 🔴 Level 3: Ultimate Challenge
- **Theme:** Tight passages with mixed enemies
- **Difficulty:** Expert
- **Features:** Claustrophobic 1-tile corridors, complex paths
- **Victory:** Complete this level to see credits and win!

---

## System Requirements

See [SYSTEM_REQUIREMENTS.md](SYSTEM_REQUIREMENTS.md) for detailed compatibility information.

### Minimum Requirements
- **CPU:** Intel 386DX/33 MHz or compatible
- **RAM:** 4 MB
- **Graphics:** VGA compatible (320×200)
- **OS:** MS-DOS 5.0 or later
- **Storage:** 1.44 MB floppy disk or 200 KB hard drive space

### Recommended Requirements
- **CPU:** Intel 486DX2/66 MHz or Pentium
- **RAM:** 8 MB
- **Graphics:** VGA
- **OS:** MS-DOS 6.22 or Windows 95/98
- **Audio:** Sound Blaster or compatible

### Tested Platforms
- ✅ Real MS-DOS machines (386, 486, Pentium)
- ✅ Windows 98 SE
- ✅ DOSBox (all versions)
- ✅ Vintage hardware (see SYSTEM_REQUIREMENTS.md)

---

## Installation

### Floppy Disk Installation (Authentic Experience)
1. Insert MAZE RUNNER floppy into drive A:
2. Type `A:` and press Enter
3. Type `MAZE_ULT.EXE` and press Enter
4. Press any key at controls screen to start

### Hard Drive Installation
1. Create a directory: `MD C:\MAZE`
2. Copy executable: `COPY A:\MAZE_ULT.EXE C:\MAZE`
3. Change to directory: `CD C:\MAZE`
4. Run the game: `MAZE_ULT.EXE`

### DOSBox (Modern Systems)
1. Download and install [DOSBox](https://www.dosbox.com/)
2. Mount the game directory: `MOUNT C ~/maze-runner`
3. Change to C drive: `C:`
4. Run the game: `MAZE_ULT.EXE`

**Recommended DOSBox Settings:**
```ini
[cpu]
core=auto
cycles=10000

[render]
aspect=true
scaler=none

[sblaster]
sbtype=sb16
sbbase=220
irq=7
dma=1
hdma=5
```

---

## Controls

### Movement
| Key | Action |
|-----|--------|
| `↑` **Up Arrow** | Move forward |
| `↓` **Down Arrow** | Move backward |
| `←` **Left Arrow** | Rotate/turn left |
| `→` **Right Arrow** | Rotate/turn right |

### Combat
| Key | Action |
|-----|--------|
| `SPACE` | **Shoot weapon** |

### System
| Key | Action |
|-----|--------|
| `ESC` | Quit game (shows credits) |
| `Y` | Restart game (on death screen) |
| `N` | Show credits (on death screen) |

### Gameplay Tips
- **SHOOT PURPLE GREMLINS & MINIONS** - Eliminate all enemies!
- **COLLECT AMMO & HEALTH PICKUPS** - Keep your supplies stocked
- **FIND FLASHING EXIT ON MINIMAP** - Look for red/yellow flashing walls
- **SURVIVE 3 LEVELS TO WIN!** - Beat all levels to see victory screen

---

## Building from Source

### Prerequisites
- **DJGPP Cross-Compiler** for Linux/macOS
- **Python 3** for sprite conversion
- **Make** (optional)

### Quick Build

```bash
# Compile the game
i586-pc-msdosdjgpp-gcc -o MAZE_ULT.EXE maze_enhanced_v2.c maze_data.c sound.c -lm -O2 -Wall

# File size should be approximately 149 KB
ls -lh MAZE_ULT.EXE
```

### Converting Custom Sprites

The game uses custom sprites from the VonHoltenCodes minion game. To convert additional sprites:

```bash
# Convert minion game sprites to VGA format
python3 convert_minion_sprites.py

# This generates:
# - src/enemy_gremlin.h (32×32 purple gremlin)
# - src/minion_jump.h (32×32 jumping minion)
# - src/minion_idle.h (32×32 idle minion)
# - src/enemy_gremlin_boss.h (64×64 boss gremlin)
```

**Sprite Conversion Details:**
- Converts PNG (RGBA) to VGA format (16 standard colors 0-15)
- Special purple detection: Maps purple tones to magenta (5) and light magenta (13)
- Transparent pixels (alpha < 128) map to color 0 (black)
- Uses NEAREST neighbor scaling to preserve pixel art

### Project Structure

```
Von_Holten-Maze-Game/
├── maze_enhanced_v2.c        # Main game engine (raycasting, combat, HUD)
├── maze_data.c               # Level layouts with HOLOGRAM walls
├── sound.c                   # Sound Blaster and PC speaker audio
├── convert_minion_sprites.py # PNG to VGA sprite converter
├── src/
│   ├── enemy_gremlin.h       # Purple gremlin sprite (32×32)
│   ├── minion_jump.h         # Jumping minion sprite (32×32)
│   ├── minion_idle.h         # Idle minion sprite (32×32)
│   └── enemy_gremlin_boss.h  # Boss gremlin sprite (64×64)
├── MAZE_ULT.EXE              # Compiled executable (149 KB)
├── SYSTEM_REQUIREMENTS.md    # Vintage hardware compatibility guide
├── VERSION_2.20_UPDATED_CREDITS_VERSION.md
├── VERSION_2.19_PURPLE_GREMLINS_DETAILED_WALLS.md
└── Screenshots/              # Game screenshots
```

### Compilation Options

```bash
# Optimized build (recommended)
i586-pc-msdosdjgpp-gcc -o MAZE_ULT.EXE maze_enhanced_v2.c maze_data.c sound.c -lm -O2 -Wall

# Debug build with symbols
i586-pc-msdosdjgpp-gcc -o MAZE_ULT.EXE maze_enhanced_v2.c maze_data.c sound.c -lm -g -Wall

# Maximum optimization (smaller binary)
i586-pc-msdosdjgpp-gcc -o MAZE_ULT.EXE maze_enhanced_v2.c maze_data.c sound.c -lm -O3 -s -Wall
```

---

## Technical Details

### Graphics Architecture
- **Rendering Mode:** VGA Mode 13h (0x13)
- **Resolution:** 320×200 pixels
- **Color Depth:** 8-bit using standard 16 VGA colors (0-15 only)
- **Rendering Technique:** DDA Raycasting (Digital Differential Analyzer)
- **Buffer System:** Double-buffered (backBuffer) for smooth rendering
- **Sprite Rendering:** Column-by-column with depth buffer (zBuffer)
- **Field of View:** Standard raycasting FOV
- **Wall Texture:** Procedural 16×16 stone blocks with mortar, cracks, 3D beveling
- **Floor Texture:** Procedural 8×4 brick pattern with offset rows

### Wall Texture Details
- **Block Size:** 16×16 pixels
- **Mortar Lines:** 2-pixel black lines between blocks
- **3D Bevel Effect:** Bright highlights on top/left edges, dark shadows on bottom/right
- **Crack Patterns:** Random black cracks in stone surface
- **Block Variety:** 30% gray stone, 30% brownstone, 40% mixed blocks
- **Weathering:** Color variation with brown/red accent streaks

### Floor Texture Details
- **Brick Size:** 8×4 pixels
- **Grout Lines:** 1-pixel black lines between bricks
- **Brick Pattern:** Offset rows (every other row shifted by half brick width)
- **Colors:** Red (60%), brown (30%), light red (10%)
- **3D Effect:** Edge darkening for depth appearance

### Mini-Map System
- **Size:** 60×60 pixels
- **Position:** Top-right corner (5px padding)
- **Features:**
  - Maze walls (gray)
  - Player position (bright green dot with direction line)
  - Enemies (red dots)
  - Exit walls (flashing red/yellow at 250ms intervals)
  - HOLOGRAM walls (magenta)

### HUD Organization
- **Position:** Bottom-left corner organized stack
- **HP Bar:** Visual health bar with numeric percentage
- **Ammo Counter:** "AMMO: XX" display
- **Score:** "SCORE: XXXX" tracking
- **Accuracy:** "HITS: XX/XX" shots hit vs fired
- **Crosshairs:** Center screen for aiming

### Audio System
- **Primary Output:** Sound Blaster DSP (isAudioAvailable() check)
- **Gun Sounds:** 10× enhanced volume (80ms, 60ms, 40ms descending tones)
- **Footsteps:** Sound Blaster playToneBlocking() for footstep audio
- **Fallback:** PC Speaker (port I/O) if Sound Blaster unavailable
- **Victory Tones:** Level completion celebration sounds

### Combat System
- **Weapon:** Pistol sprite (60×40 pixels from Kenney.nl, CC0 1.0)
- **Ammo Capacity:** 50 rounds maximum
- **Ammo Pickups:** Random spawns in maze (respawn after collection)
- **Health Pickups:** Random spawns in maze (respawn after collection)
- **Level Refill:** Full 50 ammo refill when advancing to new level
- **Accuracy Tracking:** Shots hit vs shots fired displayed in HUD

### Sprite System
- **Enemy Sprites:** Custom VonHoltenCodes minion game pixel art
- **Purple Gremlins:** 32×32 pixels, magenta colors (5 & 13)
- **Jumping Minions:** 32×32 pixels, yellow minion character
- **Weapon Sprite:** 60×40 pixels pistol (Kenney.nl CC0 1.0)
- **Sprite Orientation:** Normal Y-axis rendering (right-side up)
- **Transparency:** Color 0 (black) treated as transparent

### Game Engine
- **Movement System:** Smooth analog movement
- **Rotation Speed:** Arrow key-based rotation
- **Collision Detection:** Grid-based with wall checking
- **Map Format:** 24×24 integer grid per level
- **Special Walls:** HOLOGRAM walls (type 3) allow walk-through at exits
- **Death System:** Game over screen with restart (Y) or credits (N)
- **Credits System:** Comprehensive open source/freeware attribution

### Code Statistics
- **Language:** ANSI C (C89/C90)
- **Compiler:** DJGPP (i586-pc-msdosdjgpp-gcc)
- **DOS Extender:** DOS/4GW (32-bit DOS memory support)
- **Binary Size:** 149 KB (uncompressed, fits on 1.44 MB floppy)
- **Source Lines:** ~2,800 lines (maze_enhanced_v2.c)
- **Memory Usage:** ~64 KB framebuffer + sprite data + heap

---

## Version History

### v2.20 (2025-11-22) - Updated Credits & Version Info
- ✅ Updated credits to credit minion sprites to VonHoltenCodes
- ✅ Updated version numbers to v2.19 throughout (DOS loader, exit, credits)
- ✅ Enhanced controls screen with gameplay tips
- ✅ Added "SPACE - SHOOT" control to instructions
- ✅ Added gameplay tips (purple gremlins, pickups, minimap, 3 levels)

### v2.19 (2025-11-22) - Purple Gremlins & Detailed Walls
- ✅ Fixed purple gremlin colors (now magenta instead of gray)
- ✅ Corrected sprite orientation (right-side up)
- ✅ Added detailed stone block walls (16×16 with mortar, cracks, beveling)
- ✅ Improved wall texture with 3D depth effects
- ✅ Added weathering and color variety to walls

### v2.18 (2025-11-22) - Minion Game Sprites
- ✅ Replaced generic sprites with custom VonHoltenCodes minion game sprites
- ✅ Integrated purple gremlin enemy (32×32)
- ✅ Integrated jumping minion enemy (32×32)
- ✅ Created Python sprite converter (PNG to VGA format)
- ✅ Special purple color detection in converter

### v2.17 (2025-11-22) - HUD Reorganization & Sprite Fixes
- ✅ Reorganized HUD - mini-map top-right, stats bottom-left
- ✅ Fixed sprite orientation (enemies standing upright)
- ✅ Improved floor bricks to 8×4 pixels with realistic appearance
- ✅ Added 3D edge darkening to bricks
- ✅ Proper brick offset rows for realistic pattern

### v2.12-2.16 - Death Screen, Credits, Sprite Orientation
- ✅ Death screen with continue option (Y/N)
- ✅ Super loud gun sounds (10× volume)
- ✅ Comprehensive credits screen (all uppercase)
- ✅ Credits acknowledge DJGPP, DOS/4GW, sprite sources
- ✅ Multiple sprite orientation fixes

### v2.11 - HOLOGRAM Walls & Exit System
- ✅ Restored HOLOGRAM walk-through panels with zig-zag pattern
- ✅ Added flashing exit walls (red/yellow animation)
- ✅ HOLOGRAM walls added to maze data at exit positions

### v2.8-2.10 - Ammo & Floor Improvements
- ✅ Complete ammo management system
- ✅ Ammo refill to full 50 on new level
- ✅ Ammo and health pickups with respawn
- ✅ Improved brick floor (3×2, then 8×4 pixels)
- ✅ Dark stone wall textures
- ✅ Red/brown/grey brick variety

### v2.4-2.7 - Graphics & Audio Enhancements
- ✅ Weapon sprite integration (Kenney pistol)
- ✅ Enemy sprites (soldier, zombie)
- ✅ VGA color compatibility fixes (standard 16 colors only)
- ✅ Sound Blaster gunshot audio
- ✅ Procedural wall and floor textures

### v2.3 and Earlier
- ✅ Crosshairs
- ✅ Gunfire sounds
- ✅ Bullet tracers
- ✅ HUD improvements
- ✅ Basic raycasting engine

### v1.0 (Original)
- ✅ Basic 3-level maze system
- ✅ VGA Mode 13h rendering
- ✅ Sound Blaster audio
- ✅ Simple maze navigation

---

## Credits

### Development
**Created by:** Trent Von Holten (VonHoltenCodes)
**GitHub:** [@VonHoltenCodes](https://github.com/VonHoltenCodes)
**Year:** 2025
**Version:** 2.19

### Open Source & Freeware Used

#### Custom Sprites
- **MINION GAME SPRITES** (VonHoltenCodes)
  - Custom enemy and character sprites
  - Purple gremlin (32×32)
  - Jumping minion (32×32)
  - Idle minion (32×32)
  - Boss gremlin (64×64)
  - Source: https://github.com/VonHoltenCodes/vonholtencodes-site/tree/master/minions_game/assets/sprites

#### Third-Party Assets
- **KENNEY.NL WEAPON SPRITE** (CC0 1.0 License)
  - Pistol weapon sprite (60×40)
  - Source: https://kenney.nl
  - License: Creative Commons CC0 1.0 Universal

#### Development Tools
- **DJGPP CROSS-COMPILER** (GPL)
  - DOS development toolchain
  - GCC compiler for MS-DOS
  - Source: http://www.delorie.com/djgpp/

- **DOS/4GW DOS EXTENDER** (Freeware)
  - 32-bit DOS memory support
  - Enables >640KB memory access
  - Bundled with DJGPP

### Inspiration & Technology
- **Wolfenstein 3D** (id Software, 1992) - Raycasting engine inspiration
- **DOOM** (id Software, 1993) - First-person shooter mechanics
- **Classic DOS Gaming** - Retro aesthetic and gameplay

### Special Thanks
- **id Software** - For pioneering raycasting technology
- **DJGPP Team** - For the excellent DOS cross-compiler
- **DOSBox Developers** - For preserving DOS gaming
- **Kenney** - For CC0 licensed game assets
- **Retro Computing Community** - For keeping vintage gaming alive

---

## License

**Educational and Recreational Use**

This software is provided for educational purposes and retro gaming enthusiasts. You are free to:
- ✅ Play the game on any compatible hardware
- ✅ Study the source code
- ✅ Modify for personal use
- ✅ Share with the retro computing community
- ✅ Learn from the raycasting implementation

**Restrictions:**
- ❌ Commercial use prohibited without permission
- ❌ Credit must be maintained in derivative works

**Asset Licenses:**
- Minion game sprites: © VonHoltenCodes (2025)
- Kenney weapon sprite: CC0 1.0 Universal (Public Domain)
- DJGPP: GPL License
- DOS/4GW: Freeware distribution

---

## Support & Contact

### Issues & Bug Reports
Found a bug? Open an issue on GitHub:
**Repository:** [Von_Holten-Maze-Game](https://github.com/VonHoltenCodes/Von_Holten-Maze-Game)

### Community
- **X/Twitter:** [@VonHoltenCodes](https://twitter.com/VonHoltenCodes)
- **GitHub:** [@VonHoltenCodes](https://github.com/VonHoltenCodes)
- **Website:** [VonHoltenCodes.com](https://vonholtencodes.com)

### Documentation
- `README.md` - This file (complete game documentation)
- `SYSTEM_REQUIREMENTS.md` - Vintage hardware compatibility guide
- `VERSION_2.20_UPDATED_CREDITS_VERSION.md` - Latest version changelog
- `VERSION_2.19_PURPLE_GREMLINS_DETAILED_WALLS.md` - Graphics update details
- Source code comments - Extensive inline documentation

---

## Known Issues & Notes

### Compatibility
- ✅ Works perfectly on DOSBox with default settings
- ✅ Tested on real 486DX2/66 hardware (smooth performance)
- ✅ Compatible with Windows 98 DOS mode
- ⚠️ Sound Blaster detection may vary on emulators (PC speaker fallback available)
- ⚠️ Very fast CPUs may cause speed issues (use DOSBox cycles control)

### Gameplay Notes
- Purple gremlins use magenta colors (5 & 13) in VGA palette
- Sprite rendering uses normal Y-axis orientation (not flipped)
- Ammo refills to full 50 rounds when advancing to new level
- Mini-map exit flashes at 250ms intervals (red/yellow alternating)
- Death screen: Y restarts at level 1, N shows credits
- ESC during gameplay shows credits and exits

---

## Roadmap

### Completed Features ✅
- [x] Purple gremlin and minion sprites from VonHoltenCodes minion game
- [x] Detailed stone block walls with mortar, cracks, and 3D beveling
- [x] Realistic brick floors with offset rows
- [x] Mini-map with flashing exit indicator
- [x] Organized HUD (top-right mini-map, bottom-left stats)
- [x] Super loud gun sounds (10× volume)
- [x] Death screen with restart option
- [x] Comprehensive credits screen
- [x] Complete ammo management system
- [x] Health pickup system
- [x] Accuracy tracking (hits/shots fired)

### Potential Future Features
- [ ] Boss enemy encounters (64×64 boss gremlin sprite available)
- [ ] Additional levels with unique layouts
- [ ] Collectible items (keycards, power-ups)
- [ ] Door/trigger system
- [ ] High score table with persistence
- [ ] AdLib/OPL2 music support
- [ ] Additional weapon types
- [ ] Enemy AI improvements (patrol patterns, aggro radius)

*Contributions and suggestions welcome!*

---

<div align="center">

**MAZE RUNNER v2.19**

*Shoot Purple Gremlins • Collect Ammo & Health • Find the Flashing Exit • Survive 3 Levels to Win!*

**Built with ❤️ for the retro gaming community**

**Powered by VonHoltenCodes Custom Pixel Art**

[⬆ Back to Top](#maze-runner)

</div>
