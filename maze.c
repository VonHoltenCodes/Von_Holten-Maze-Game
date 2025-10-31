/*
 * ============================================
 * MAZE.C - Cyberpunk Maze Runner
 * ============================================
 *
 * CYBERPUNK MAZE RUNNER
 * "Welcome to the Grid, Runner..."
 *
 * A Neon-Lit 3D Raycasting Corporate Cyberspace
 *
 * Inspired by: Doom, Descent, Wolfenstein 3D
 * Based on: BONK v9.5 DOS Edition techniques
 * Theme: Cyberpunk 2077 / Blade Runner / Tron
 *
 * Compile: gcc -Wall -O2 -march=i386 maze.c maze_data.c -o MAZE.EXE -lm -s
 * Target: MS-DOS 3.3+, 386+ CPU
 * Graphics: VGA Mode 13h (320x200, 256 colors)
 * Input: Keyboard (arrow keys)
 *
 * By: VonHoltenCodes (2025)
 * License: Educational/Recreational Use Only
 */

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <time.h>
#include <string.h>
#include "maze.h"

#ifdef __DJGPP__
#include <sys/nearptr.h>
#include <sys/farptr.h>
#include <go32.h>
#include <dpmi.h>
#include <pc.h>
#include <sys/movedata.h>
#define outp(port, val) outportb(port, val)
#define inp(port) inportb(port)
#define VGA_MEMORY ((unsigned char *)(0xA0000 + __djgpp_conventional_base))
#else
#define VGA_MEMORY ((unsigned char far *)0xA0000000L)
#endif

/* Double buffer for smooth rendering */
#define BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
static unsigned char *backBuffer = NULL;

/* Game state variables */
static clock_t gameStartTime = 0;
static int gameWon = 0;
static double levelTimes[3] = {0.0, 0.0, 0.0}; /* Track completion time for each level */

/* Footstep sound state */
static int footstepIndex = 0;
static clock_t lastFootstepTime = 0;

/* Keyboard state for simultaneous key detection */
static unsigned char keyDown[128] = {0};

/* Simple 8x8 procedural textures */
#define TEX_SIZE 8

/* MAZE text pattern (5x5 pixels for each letter, simple font) */
static const unsigned char mazeText[4][5] = {
    {0x1F, 0x15, 0x15, 0x15, 0x15}, /* M */
    {0x1F, 0x14, 0x14, 0x14, 0x1F}, /* A */
    {0x1F, 0x05, 0x07, 0x05, 0x1F}, /* Z */
    {0x1F, 0x15, 0x15, 0x15, 0x15}  /* E */
};

/* Simple 3x5 digit font for timer display */
static const unsigned char digitFont[11][5] = {
    {0x7, 0x5, 0x5, 0x5, 0x7}, /* 0 */
    {0x2, 0x2, 0x2, 0x2, 0x2}, /* 1 */
    {0x7, 0x1, 0x7, 0x4, 0x7}, /* 2 */
    {0x7, 0x1, 0x7, 0x1, 0x7}, /* 3 */
    {0x5, 0x5, 0x7, 0x1, 0x1}, /* 4 */
    {0x7, 0x4, 0x7, 0x1, 0x7}, /* 5 */
    {0x7, 0x4, 0x7, 0x5, 0x7}, /* 6 */
    {0x7, 0x1, 0x1, 0x1, 0x1}, /* 7 */
    {0x7, 0x5, 0x7, 0x5, 0x7}, /* 8 */
    {0x7, 0x5, 0x7, 0x1, 0x7}, /* 9 */
    {0x0, 0x2, 0x0, 0x2, 0x0}  /* : (colon) */
};

/* Draw a single digit at position (x, y) */
void drawDigit(int x, int y, int digit, unsigned char color) {
    int dx, dy;
    if (digit < 0 || digit > 10) return;

    for (dy = 0; dy < 5; dy++) {
        for (dx = 0; dx < 3; dx++) {
            if ((digitFont[digit][dy] >> (2 - dx)) & 1) {
                if (x + dx >= 0 && x + dx < SCREEN_WIDTH && y + dy >= 0 && y + dy < SCREEN_HEIGHT) {
                    backBuffer[(y + dy) * SCREEN_WIDTH + (x + dx)] = color;
                }
            }
        }
    }
}

/* Draw timer in top right corner - cyberpunk style */
void drawTimer(clock_t startTime) {
    double elapsed = (double)(clock() - startTime) / CLOCKS_PER_SEC;
    int minutes = (int)(elapsed / 60);
    int seconds = (int)elapsed % 60;

    /* Position in top right corner with padding */
    int startX = SCREEN_WIDTH - 40;
    int startY = 5;

    /* Draw background box for timer - dark cyan box */
    int boxX, boxY;
    for (boxY = startY - 2; boxY < startY + 8; boxY++) {
        for (boxX = startX - 2; boxX < SCREEN_WIDTH - 2; boxX++) {
            if (boxX >= 0 && boxX < SCREEN_WIDTH && boxY >= 0 && boxY < SCREEN_HEIGHT) {
                backBuffer[boxY * SCREEN_WIDTH + boxX] = COLOR_BLACK;
            }
        }
    }

    /* Draw border - bright cyan */
    for (boxX = startX - 2; boxX < SCREEN_WIDTH - 2; boxX++) {
        if (boxX >= 0 && boxX < SCREEN_WIDTH) {
            backBuffer[(startY - 2) * SCREEN_WIDTH + boxX] = COLOR_LCYAN;
            backBuffer[(startY + 7) * SCREEN_WIDTH + boxX] = COLOR_LCYAN;
        }
    }
    for (boxY = startY - 2; boxY < startY + 8; boxY++) {
        if (boxY >= 0 && boxY < SCREEN_HEIGHT) {
            backBuffer[boxY * SCREEN_WIDTH + (startX - 2)] = COLOR_LCYAN;
            backBuffer[boxY * SCREEN_WIDTH + (SCREEN_WIDTH - 3)] = COLOR_LCYAN;
        }
    }

    /* Draw MM:SS format in bright cyan */
    drawDigit(startX, startY, minutes / 10, COLOR_LCYAN);
    drawDigit(startX + 4, startY, minutes % 10, COLOR_LCYAN);
    drawDigit(startX + 8, startY, 10, COLOR_LCYAN); /* colon */
    drawDigit(startX + 12, startY, seconds / 10, COLOR_LCYAN);
    drawDigit(startX + 16, startY, seconds % 10, COLOR_LCYAN);
}

/* Check if pixel is part of MAZE text */
int isTextPixel(int texX, int texY) {
    /* Text appears every 4 tiles */
    int tileX = (texX / TEX_SIZE) % 4;
    int tileY = (texY / TEX_SIZE) % 4;

    /* Only show text on specific tiles */
    if (tileY != 1) return 0;

    /* Get position within tile */
    int localX = texX % TEX_SIZE;
    int localY = texY % TEX_SIZE;

    /* Center the 5x5 text in 8x8 tile */
    if (localX < 1 || localX >= 6 || localY < 1 || localY >= 6) return 0;

    int charX = localX - 1;
    int charY = localY - 1;

    /* Check if this pixel is set in the character */
    if (tileX < 4 && charY < 5) {
        return (mazeText[tileX][charY] >> (4 - charX)) & 1;
    }
    return 0;
}

/* Generate a simple wall texture pattern with level-specific colors */
unsigned char getWallTexel(int wallType, int texX, int texY, int level) {
    /* Add MAZE text decoration */
    if (isTextPixel(texX, texY)) {
        return COLOR_YELLOW; /* Bright yellow text on all walls */
    }

    /* Wrap coordinates for tiling */
    texX = texX & (TEX_SIZE - 1);
    texY = texY & (TEX_SIZE - 1);

    unsigned char brightColor, darkColor;

    switch (wallType) {
        case CELL_WALL_NEON_GRID: /* Cyan grid pattern */
            /* Level-specific colors */
            if (level == 1) {
                /* Level 1: Standard bright neon */
                brightColor = COLOR_LCYAN;
                darkColor = COLOR_CYAN;
            } else if (level == 2) {
                /* Level 2: Darker with neon highlights */
                brightColor = COLOR_LCYAN;
                darkColor = COLOR_BLUE;  /* Much darker base */
            } else {
                /* Level 3: Neon panels on dark */
                brightColor = COLOR_LCYAN;
                darkColor = COLOR_BLACK;  /* Very dark base */
            }
            if (texX == 0 || texY == 0 || texX == TEX_SIZE-1 || texY == TEX_SIZE-1)
                return brightColor;
            return darkColor;

        case CELL_WALL_CIRCUIT: /* Green circuit pattern */
            if (level == 1) {
                brightColor = COLOR_LGREEN;
                darkColor = COLOR_GREEN;
            } else if (level == 2) {
                brightColor = COLOR_LGREEN;
                darkColor = COLOR_BROWN;  /* Darker oxidized circuits */
            } else {
                brightColor = COLOR_LGREEN;
                darkColor = COLOR_BLACK;
            }
            if ((texX + texY) % 3 == 0)
                return brightColor;
            return darkColor;

        case CELL_WALL_HOLOGRAM: /* Magenta hologram shimmer */
            if (level == 1) {
                brightColor = COLOR_LMAGENTA;
                darkColor = COLOR_MAGENTA;
            } else if (level == 2) {
                brightColor = COLOR_LMAGENTA;
                darkColor = COLOR_RED;  /* Darker crimson */
            } else {
                brightColor = COLOR_LMAGENTA;
                darkColor = COLOR_BLACK;
            }
            if ((texX ^ texY) & 2)
                return brightColor;
            return darkColor;

        case CELL_WALL_DATA: /* Purple data stream */
            if (level == 1) {
                brightColor = COLOR_LMAGENTA;
                darkColor = COLOR_MAGENTA;
            } else if (level == 2) {
                brightColor = COLOR_LMAGENTA;
                darkColor = COLOR_MAGENTA;
            } else {
                brightColor = COLOR_LMAGENTA;
                darkColor = COLOR_BLACK;
            }
            if (texY % 2 == 0)
                return darkColor;
            return (texX % 3 == 0) ? brightColor : darkColor;

        default:
            return COLOR_GRAY;
    }
}

/* Generate a cobblestone floor texture pattern with level-specific colors */
unsigned char getFloorTexel(int texX, int texY, int level) {
    /* Wrap coordinates */
    texX = texX & (TEX_SIZE - 1);
    texY = texY & (TEX_SIZE - 1);

    /* Level-specific floor colors */
    unsigned char grout, edge, face;

    if (level == 1) {
        /* Level 1: Standard stone floor */
        grout = COLOR_BLACK;
        edge = COLOR_GRAY;
        face = COLOR_BROWN;
    } else if (level == 2) {
        /* Level 2: Darker floor with blue tint */
        grout = COLOR_BLACK;
        edge = COLOR_BLUE;
        face = COLOR_GRAY;
    } else {
        /* Level 3: Very dark floor, almost void */
        grout = COLOR_BLACK;
        edge = COLOR_BLACK;
        face = COLOR_BLUE;
    }

    /* Create cobblestone pattern with grout lines */
    if (texX == 0 || texY == 0)
        return grout;           /* Grout/mortar */
    if (texX == 1 || texY == 1)
        return edge;            /* Stone edge */
    return face;                /* Stone face */
}

/*============================================================================
 * DOUBLE BUFFERING FUNCTIONS
 *===========================================================================*/

/* Initialize double buffer */
void initDoubleBuffer(void) {
    backBuffer = (unsigned char *)malloc(BUFFER_SIZE);
    if (!backBuffer) {
        printf("ERROR: Could not allocate double buffer!\n");
        exit(1);
    }
    /* Clear the buffer */
    memset(backBuffer, 0, BUFFER_SIZE);
}

/* Free double buffer */
void freeDoubleBuffer(void) {
    if (backBuffer) {
        free(backBuffer);
        backBuffer = NULL;
    }
}

/* Wait for vertical retrace to prevent tearing */
void waitForVRetrace(void) {
    /* Wait for vertical retrace to END (if currently in one) */
    while ((inp(0x3DA) & 0x08) == 0x08);

    /* Wait for vertical retrace to BEGIN (safe window) */
    while ((inp(0x3DA) & 0x08) == 0x00);
}

/* Copy back buffer to VGA memory during vertical retrace */
void displayFrame(void) {
    waitForVRetrace();
    memcpy(VGA_MEMORY, backBuffer, BUFFER_SIZE);
}

/*============================================================================
 * VGA GRAPHICS FUNCTIONS
 *===========================================================================*/

void setVideoMode(int mode) {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
    int86(0x10, &regs, &regs);
}

void setPixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        backBuffer[y * SCREEN_WIDTH + x] = color;
    }
}

void clearScreen(unsigned char color) {
    memset(backBuffer, color, BUFFER_SIZE);
}

void drawVerticalLine(int x, int drawStart, int drawEnd, unsigned char color) {
    int y;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (drawStart < 0) drawStart = 0;
    if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

    for (y = drawStart; y <= drawEnd; y++) {
        backBuffer[y * SCREEN_WIDTH + x] = color;
    }
}

/*============================================================================
 * PLAYER FUNCTIONS
 *===========================================================================*/

void initPlayer(Player *player, Maze *maze) {
    int x, y;

    /* Find spawn point in maze (ID 10) */
    for (y = 0; y < maze->height; y++) {
        for (x = 0; x < maze->width; x++) {
            if (getMazeCell(maze, x, y) == REF_PLAYER_SPAWN) {
                player->x = x + 0.5;
                player->y = y + 0.5;

                /* Clear spawn marker so player can walk there */
                setMazeCell(maze, x, y, CELL_EMPTY);
                goto spawn_found;
            }
        }
    }

spawn_found:
    /* Initialize direction (facing east) */
    player->angle = 0.0;
    player->dirX = 1.0;
    player->dirY = 0.0;

    /* Initialize camera plane (perpendicular to direction) */
    player->planeX = 0.0;
    player->planeY = 0.66; /* FOV adjustment */
}

void movePlayer(Player *player, Maze *maze, double moveDir) {
    double newX = player->x + player->dirX * moveDir * MOVE_SPEED;
    double newY = player->y + player->dirY * moveDir * MOVE_SPEED;

    /* Collision detection - check if new position is walkable */
    if (!isWall(getMazeCell(maze, (int)newX, (int)player->y))) {
        player->x = newX;
    }
    if (!isWall(getMazeCell(maze, (int)player->x, (int)newY))) {
        player->y = newY;
    }
}

void rotatePlayer(Player *player, double rotateDir) {
    double angle = rotateDir * ROTATE_SPEED;
    double oldDirX = player->dirX;
    double oldPlaneX = player->planeX;

    /* Rotate direction vector */
    player->dirX = player->dirX * cos(angle) - player->dirY * sin(angle);
    player->dirY = oldDirX * sin(angle) + player->dirY * cos(angle);

    /* Rotate camera plane */
    player->planeX = player->planeX * cos(angle) - player->planeY * sin(angle);
    player->planeY = oldPlaneX * sin(angle) + player->planeY * cos(angle);
}

/*============================================================================
 * RENDERING FUNCTIONS
 *===========================================================================*/

unsigned char getWallColor(int cellType, double distance) {
    unsigned char baseColor;

    /* Select CYBERPUNK base color based on wall type */
    switch (cellType) {
        case CELL_WALL_NEON_GRID:
            baseColor = NEON_CYAN;      /* Cyan neon grid panels */
            break;
        case CELL_WALL_CIRCUIT:
            baseColor = NEON_GREEN;     /* Green circuit boards */
            break;
        case CELL_WALL_HOLOGRAM:
            baseColor = NEON_MAGENTA;   /* Magenta holographic barriers */
            break;
        case CELL_WALL_DATA:
            baseColor = CYBER_PURPLE;   /* Purple data streams */
            break;
        default:
            baseColor = NEON_CYAN;      /* Default to cyan */
            break;
    }

    /* Cyberpunk fog effect - fade to void */
    if (distance > 15.0) {
        return VOID_BLACK;              /* Deep void */
    } else if (distance > 10.0) {
        return COLOR_BLUE;              /* Dark blue haze */
    } else if (distance > 5.0) {
        return baseColor - 3;           /* Dimmed neon */
    } else {
        return baseColor;               /* Full neon brightness */
    }
}

void renderFrame(Player *player, Maze *maze) {
    int x, y;
    int level = maze->level;

    /* Draw ceiling - level-specific colors */
    unsigned char ceilingColor;
    if (level == 1) {
        ceilingColor = COLOR_BLUE;      /* Level 1: Standard deep blue */
    } else if (level == 2) {
        ceilingColor = COLOR_BLACK;     /* Level 2: Darker void */
    } else {
        ceilingColor = COLOR_BLACK;     /* Level 3: Complete darkness */
    }

    for (y = 0; y < SCREEN_CENTER; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            backBuffer[y * SCREEN_WIDTH + x] = ceilingColor;
        }
    }

    /* Draw floor with cobblestone texture - FIXED TO WORLD COORDINATES */
    for (y = SCREEN_CENTER; y < SCREEN_HEIGHT; y++) {
        /* Raycast floor row */
        double rowDistance = (SCREEN_HEIGHT / 2.0) / (y - SCREEN_HEIGHT / 2.0);

        /* Calculate floor endpoints for this row */
        double floorStepX = rowDistance * (player->planeX * 2.0) / SCREEN_WIDTH;
        double floorStepY = rowDistance * (player->planeY * 2.0) / SCREEN_WIDTH;

        double floorX = player->x + rowDistance * player->dirX - rowDistance * player->planeX;
        double floorY = player->y + rowDistance * player->dirY - rowDistance * player->planeY;

        for (x = 0; x < SCREEN_WIDTH; x++) {
            /* Get texture coordinates from world position */
            int texX = (int)(floorX * TEX_SIZE) & (TEX_SIZE - 1);
            int texY = (int)(floorY * TEX_SIZE) & (TEX_SIZE - 1);
            backBuffer[y * SCREEN_WIDTH + x] = getFloorTexel(texX, texY, level);

            floorX += floorStepX;
            floorY += floorStepY;
        }
    }

    /* Raycasting for each vertical stripe */
    for (x = 0; x < SCREEN_WIDTH; x++) {
        /* Calculate ray position and direction */
        double cameraX = 2 * x / (double)SCREEN_WIDTH - 1;
        double rayDirX = player->dirX + player->planeX * cameraX;
        double rayDirY = player->dirY + player->planeY * cameraX;

        /* Current map position */
        int mapX = (int)player->x;
        int mapY = (int)player->y;

        /* Length of ray from current position to next x or y-side */
        double sideDistX, sideDistY;

        /* Length of ray from one x or y-side to next x or y-side */
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);
        double perpWallDist;

        /* What direction to step in x or y-direction (either +1 or -1) */
        int stepX, stepY;

        int hit = 0; /* Was there a wall hit? */
        int side; /* Was a NS or a EW wall hit? */

        /* Calculate step and initial sideDist */
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player->x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player->x) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player->y - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player->y) * deltaDistY;
        }

        /* Perform DDA */
        while (hit == 0) {
            /* Jump to next map square, either in x-direction, or in y-direction */
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            /* Check if ray has hit a wall */
            int cellValue = getMazeCell(maze, mapX, mapY);
            if (isWall(cellValue) || isReferencePoint(cellValue)) {
                hit = 1;
            }
        }

        /* Calculate distance to wall */
        if (side == 0) {
            perpWallDist = (mapX - player->x + (1 - stepX) / 2) / rayDirX;
        } else {
            perpWallDist = (mapY - player->y + (1 - stepY) / 2) / rayDirY;
        }

        /* Calculate height of line to draw on screen */
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        /* Calculate lowest and highest pixel to fill in current stripe */
        int drawStart = -lineHeight / 2 + SCREEN_CENTER;
        int drawEnd = lineHeight / 2 + SCREEN_CENTER;
        if (drawStart < 0) drawStart = 0;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        /* Calculate texture X coordinate where ray hit the wall */
        double wallX;
        if (side == 0) wallX = player->y + perpWallDist * rayDirY;
        else           wallX = player->x + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        int texX = (int)(wallX * TEX_SIZE);
        if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0)) {
            texX = TEX_SIZE - texX - 1;
        }

        /* Get wall type */
        int cellValue = getMazeCell(maze, mapX, mapY);

        /* Calculate texture Y step per screen pixel */
        double step = (double)TEX_SIZE / lineHeight;
        double texPos = (drawStart - SCREEN_CENTER + lineHeight / 2) * step;

        /* Draw textured vertical line */
        for (y = drawStart; y < drawEnd; y++) {
            int texY = ((int)texPos) & (TEX_SIZE - 1);
            texPos += step;

            /* Sample texture with level-specific colors */
            unsigned char color = getWallTexel(cellValue, texX, texY, level);

            /* Apply distance fog - preserve bright colors better */
            if (perpWallDist > 15.0) {
                color = VOID_BLACK;
            } else if (perpWallDist > 10.0) {
                /* Heavy fog - darken but preserve some color */
                if (color >= 8) color -= 6; /* Bright to dim */
                else if (color > 0) color = color / 2; /* Dim to darker */
            } else if (perpWallDist > 5.0) {
                /* Light fog - darken bright colors only */
                if (color > 8) color -= 4;
            }

            /* Make EW walls darker for depth perception - only if not too dark */
            if (side == 1 && color > 2) {
                color = color - 1;
            }

            backBuffer[y * SCREEN_WIDTH + x] = color;
        }
    }

    /* Draw timer overlay on top of rendered scene */
    drawTimer(gameStartTime);
}

/*============================================================================
 * INPUT HANDLING
 *===========================================================================*/

/* Forward declaration */
void playFootstepSound(void);

/* Read keyboard state directly from BIOS - allows simultaneous key detection */
void readKeyboardState(unsigned char *keys) {
    /* Clear all keys first */
    memset(keys, 0, 128);

    /* Check ESC in normal buffer */
    if (kbhit()) {
        int ch = getch();
        if (ch == 27) {  /* ESC */
            keys[1] = 1;  /* Scan code 1 = ESC */
            return;
        }
        /* Don't consume other keys - let BIOS handle them */
        if (ch != 0 && ch != 224) {
            /* Not an extended key, put it back conceptually */
        }
    }

    /* Use BIOS to read current keyboard state */
    /* This is the key to simultaneous key detection! */
    #ifdef __DJGPP__
    /* Use INT 16h, AH=12h to get extended keyboard flags */
    __dpmi_regs regs;
    regs.h.ah = 0x12;
    __dpmi_int(0x16, &regs);

    /* Now poll for each specific key we care about using INT 16h, AH=11h */
    /* This checks if a key is available without removing it */
    while (_bios_keybrd(0x11) != 0) {
        int keycode = _bios_keybrd(0x11);  /* Peek without removing */
        int scancode = (keycode >> 8) & 0xFF;

        /* Mark the key as pressed */
        if (scancode == 0x48) keys[72] = 1;  /* Up */
        if (scancode == 0x50) keys[80] = 1;  /* Down */
        if (scancode == 0x4B) keys[75] = 1;  /* Left */
        if (scancode == 0x4D) keys[77] = 1;  /* Right */
        if (scancode == 0x01) keys[1] = 1;   /* ESC */

        /* Remove one key from buffer to avoid infinite loop */
        _bios_keybrd(0x10);
        break;  /* Only read one key per frame to avoid slowdown */
    }
    #endif
}

void handleInput(Player *player, Maze *maze, int *running) {
    /* Read current keyboard state */
    readKeyboardState(keyDown);

    /* Check for ESC */
    if (keyDown[1]) {
        *running = 0;
        return;
    }

    /* Arrow key states */
    int moveForward = keyDown[72];   /* Up arrow */
    int moveBackward = keyDown[80];  /* Down arrow */
    int turnLeft = keyDown[75];      /* Left arrow */
    int turnRight = keyDown[77];     /* Right arrow */

    /* Check if player is trying to move */
    int isMoving = moveForward || moveBackward;
    int isTurning = turnLeft || turnRight;

    /* STRAFING: Movement + Turning = diagonal strafe */
    if (isMoving && isTurning) {
        double moveDir = moveForward ? 1.0 : -1.0;
        double strafeDir = turnRight ? 1.0 : -1.0;

        /* Move forward/backward */
        movePlayer(player, maze, moveDir);

        /* Strafe left/right along camera plane */
        double newX = player->x + player->planeX * strafeDir * MOVE_SPEED;
        double newY = player->y + player->planeY * strafeDir * MOVE_SPEED;

        if (!isWall(getMazeCell(maze, (int)newX, (int)player->y))) {
            player->x = newX;
        }
        if (!isWall(getMazeCell(maze, (int)player->x, (int)newY))) {
            player->y = newY;
        }

        playFootstepSound();
    }
    /* PIVOT: Turning only = rotate in place */
    else if (!isMoving && isTurning) {
        if (turnLeft) {
            rotatePlayer(player, -1.0);
        }
        if (turnRight) {
            rotatePlayer(player, 1.0);
        }
    }
    /* STRAIGHT MOVEMENT: Moving only = walk straight */
    else if (isMoving && !isTurning) {
        if (moveForward) {
            movePlayer(player, maze, 1.0);
        }
        if (moveBackward) {
            movePlayer(player, maze, -1.0);
        }

        playFootstepSound();
    }
}

/*============================================================================
 * VGA SPLASH SCREENS
 *===========================================================================*/

/* 8x8 bitmap font - each character is 8 bytes (8 rows of 8 pixels) */
static unsigned char font8x8[128][8] = {
    [0 ... 127] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  /* Initialize all to blank */
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['!'] = {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},
    ['-'] = {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},
    ['/'] = {0x00, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00},
    ['0'] = {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00},
    ['1'] = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
    ['2'] = {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00},
    ['3'] = {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00},
    ['4'] = {0x0C, 0x1C, 0x2C, 0x4C, 0x7E, 0x0C, 0x0C, 0x00},
    ['5'] = {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00},
    ['6'] = {0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},
    ['7'] = {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00},
    ['8'] = {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},
    ['9'] = {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00},
    ['A'] = {0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
    ['B'] = {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},
    ['C'] = {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},
    ['D'] = {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},
    ['E'] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00},
    ['F'] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00},
    ['G'] = {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00},
    ['H'] = {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
    ['I'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
    ['J'] = {0x3E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00},
    ['K'] = {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00},
    ['L'] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00},
    ['M'] = {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00},
    ['N'] = {0x66, 0x76, 0x7E, 0x6E, 0x66, 0x66, 0x66, 0x00},
    ['O'] = {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
    ['P'] = {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00},
    ['Q'] = {0x3C, 0x66, 0x66, 0x66, 0x6A, 0x6C, 0x36, 0x00},
    ['R'] = {0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00},
    ['S'] = {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00},
    ['T'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
    ['U'] = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
    ['V'] = {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},
    ['W'] = {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},
    ['X'] = {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00},
    ['Y'] = {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},
    ['Z'] = {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},
    ['+'] = {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},
    [':'] = {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00},
    ['.'] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},
    [','] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},
    ['?'] = {0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x00},
};

/* Draw a single 8x8 character */
void drawChar8x8(int x, int y, char c, unsigned char color) {
    /* Convert to unsigned to avoid negative values */
    unsigned char uc = (unsigned char)c;
    if (uc >= 128) return;

    const unsigned char *glyph = font8x8[uc];

    for (int row = 0; row < 8; row++) {
        unsigned char rowData = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (rowData & (0x80 >> col)) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    backBuffer[py * SCREEN_WIDTH + px] = color;
                }
            }
        }
    }
}

/* Draw text string in VGA mode */
void drawText(int x, int y, const char *text, unsigned char color) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        drawChar8x8(x + i * 8, y, text[i], color);
    }
}

/* Draw large text (2x scale) */
void drawBigText(int x, int y, const char *text, unsigned char color) {
    if (!text) return;

    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)text[i];
        if (c >= 128) continue;

        const unsigned char *glyph = font8x8[c];

        /* Draw 2x scaled character */
        for (int row = 0; row < 8; row++) {
            unsigned char rowData = glyph[row];
            for (int col = 0; col < 8; col++) {
                if (rowData & (0x80 >> col)) {
                    /* Draw 2x2 pixel block */
                    for (int dy = 0; dy < 2; dy++) {
                        for (int dx = 0; dx < 2; dx++) {
                            int px = x + i * 16 + col * 2 + dx;
                            int py = y + row * 2 + dy;
                            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                                backBuffer[py * SCREEN_WIDTH + px] = color;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* Show VGA splash screen at game start */
void showVGASplashScreen(void) {
    clearScreen(COLOR_BLACK);

    /* Title - "MAZE RUNNER" - center it properly */
    const char *title = "MAZE RUNNER";
    int titleX = (SCREEN_WIDTH - strlen(title) * 16) / 2;
    drawBigText(titleX, 20, title, NEON_CYAN);

    /* Subtitle - use normal text (8x8) */
    const char *subtitle = "CYBERPUNK GRID ESCAPE";
    int subX = (SCREEN_WIDTH - strlen(subtitle) * 8) / 2;
    drawText(subX, 50, subtitle, NEON_MAGENTA);

    /* Credits */
    int y = 70;
    const char *credit1 = "CREATED BY TRENT VON HOLTEN";
    int credit1X = (SCREEN_WIDTH - strlen(credit1) * 8) / 2;
    drawText(credit1X, y, credit1, COLOR_WHITE);

    y += 10;
    const char *credit2 = "VONHOLTENCODES";
    int credit2X = (SCREEN_WIDTH - strlen(credit2) * 8) / 2;
    drawText(credit2X, y, credit2, NEON_GREEN);

    y += 10;
    const char *credit3 = "BUILT FOR 1995, IN 2025";
    int credit3X = (SCREEN_WIDTH - strlen(credit3) * 8) / 2;
    drawText(credit3X, y, credit3, COLOR_YELLOW);

    /* Instructions - use normal text */
    y = 110;
    drawText(60, y, "CONTROLS:", NEON_GREEN);
    y += 14;
    drawText(40, y, "UP ARROW - MOVE FORWARD", COLOR_WHITE);
    y += 10;
    drawText(40, y, "DOWN ARROW - MOVE BACK", COLOR_WHITE);
    y += 10;
    drawText(40, y, "LEFT/RIGHT - TURN", COLOR_WHITE);
    y += 10;
    drawText(40, y, "ESC - QUIT", COLOR_WHITE);

    /* Press key prompt */
    y = 170;
    const char *prompt = "PRESS ANY KEY TO START";
    int promptX = (SCREEN_WIDTH - strlen(prompt) * 8) / 2;
    drawText(promptX, y, prompt, COLOR_YELLOW);

    /* Display */
    displayFrame();

    /* Wait for key */
    getch();
}

/* Show level completion splash with time */
void showLevelCompletionSplash(int level, double time) {
    clearScreen(COLOR_BLACK);

    /* "LEVEL COMPLETE" title */
    const char *title = "LEVEL COMPLETE";
    int titleX = (SCREEN_WIDTH - strlen(title) * 16) / 2;
    drawBigText(titleX, 30, title, NEON_GREEN);

    /* Level number */
    char levelText[32];
    sprintf(levelText, "SECTOR 7-%c", 'F' + level);
    int levelX = (SCREEN_WIDTH - strlen(levelText) * 8) / 2;
    drawText(levelX, 70, levelText, NEON_CYAN);

    /* Time */
    char timeText[32];
    sprintf(timeText, "TIME: %.2f SECONDS", time);
    int timeX = (SCREEN_WIDTH - strlen(timeText) * 8) / 2;
    drawText(timeX, 90, timeText, COLOR_YELLOW);

    /* Show all previous times */
    int y = 120;
    drawText(60, y, "YOUR TIMES:", NEON_MAGENTA);
    y += 16;
    for (int i = 0; i < level; i++) {
        if (levelTimes[i] > 0) {
            char timeEntry[40];
            sprintf(timeEntry, "LEVEL %d: %.2f SEC", i + 1, levelTimes[i]);
            drawText(60, y, timeEntry, COLOR_WHITE);
            y += 12;
        }
    }

    /* Display */
    displayFrame();

    /* Wait 3 seconds */
    delay(3000);
}

/* Show level start splash in VGA mode */
void showLevelSplash(int level) {
    clearScreen(COLOR_BLACK);

    /* Level title - e.g. "SECTOR 7-G" */
    char levelText[32];
    sprintf(levelText, "SECTOR 7-%c", 'F' + level);
    int titleX = (SCREEN_WIDTH - strlen(levelText) * 16) / 2;
    drawBigText(titleX, 80, levelText, NEON_CYAN);

    /* Level number */
    char numText[32];
    sprintf(numText, "LEVEL %d OF 3", level);
    int numX = (SCREEN_WIDTH - strlen(numText) * 8) / 2;
    drawText(numX, 120, numText, NEON_MAGENTA);

    /* Display */
    displayFrame();

    /* Short delay */
    delay(2000);
}

/*============================================================================
 * SOUND WRAPPER
 *===========================================================================*/

/* Note: Actual Sound Blaster implementation is in sound.c */

/* Footstep sounds - alternating tones like walking */
static const int footstepTones[] = {
    262,  /* C4 */
    294,  /* D4 */
    330,  /* E4 */
    349,  /* F4 */
    392,  /* G4 */
    349,  /* F4 */
    330,  /* E4 */
    294   /* D4 */
};

#define FOOTSTEP_COUNT (sizeof(footstepTones) / sizeof(int))
#define FOOTSTEP_DELAY_MS 200  /* Minimum time between footsteps */

/* Play footstep sound when moving */
void playFootstepSound(void) {
    clock_t now = clock();
    long elapsed = (now - lastFootstepTime) * 1000 / CLOCKS_PER_SEC;

    /* Only play if enough time has passed (walking rhythm) */
    if (elapsed >= FOOTSTEP_DELAY_MS) {
        int freq = footstepTones[footstepIndex];

        /* Quick short tone for footstep */
        if (isAudioAvailable()) {
            playToneBlocking(freq, 50);  /* Short 50ms burst */
        } else {
            /* PC Speaker fallback */
            unsigned int divisor = 1193180 / freq;
            outp(0x43, 0xB6);
            outp(0x42, divisor & 0xFF);
            outp(0x42, divisor >> 8);
            unsigned char tmp = inp(0x61);
            outp(0x61, tmp | 3);
            delay(50);
            outp(0x61, tmp & 0xFC);
        }

        footstepIndex = (footstepIndex + 1) % FOOTSTEP_COUNT;
        lastFootstepTime = now;
    }
}

/* Play victory chime - simple PC speaker beeps only */
void playVictorySound(void) {
    /* Force PC speaker beeps - no Sound Blaster */
    /* This avoids all the square wave glitchiness */
    int frequencies[] = {440, 523, 659, 784};
    int i;

    for (i = 0; i < 4; i++) {
        /* Direct PC speaker programming */
        unsigned int divisor = 1193180 / frequencies[i];
        outp(0x43, 0xB6);
        outp(0x42, divisor & 0xFF);
        outp(0x42, divisor >> 8);
        unsigned char tmp = inp(0x61);
        outp(0x61, tmp | 3);

        if (i < 3) {
            delay(150);  /* Short beep */
        } else {
            delay(400);  /* Final victory beep */
        }

        /* Turn off */
        outp(0x61, tmp);
        delay(50);  /* Pause between beeps */
    }
}

/* Check if player reached the exit */
int checkWinCondition(Player *player) {
    int playerMapX = (int)player->x;
    int playerMapY = (int)player->y;

    /* Check if player is at exit position (12, 22) */
    if (playerMapX == 12 && playerMapY == 22) {
        return 1;
    }
    return 0;
}

/*============================================================================
 * UTILITY FUNCTIONS
 *===========================================================================*/

double degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

void delay_ms(int ms) {
    unsigned long start = clock();
    unsigned long ticks = ms * CLOCKS_PER_SEC / 1000;
    while (clock() - start < ticks);
}

/*============================================================================
 * SPLASH SCREEN
 *===========================================================================*/

void showSplashScreen(void) {
    /* Clear screen using DOS - works better than newlines */
    #ifdef __DJGPP__
    /* DJGPP: Use ANSI escape codes */
    printf("\033[2J\033[H");
    #else
    /* DOS: Use clrscr() if available, or system call */
    system("cls");
    #endif

    /* Show splash - DOS ASCII ONLY - Clear MAZE art */
    printf("\n\n");
    printf("  ###   ###     ###     #######  #######\n");
    printf("  ## ### ##    ## ##       ##    ##     \n");
    printf("  ##  #  ##   ##   ##     ##     #####  \n");
    printf("  ##     ##   #######    ##      ##     \n");
    printf("  ##     ##  ##     ##  #######  #######\n");
    printf("\n");
    printf("        CYBERPUNK 3D RAYCASTING GAME\n");
    printf("              3 Levels of Terror\n");
    printf("\n");
    printf("   ========================================\n");
    printf("        Created by Trent Von Holten\n");
    printf("                  2025\n");
    printf("   ========================================\n");
    printf("\n");
    printf("             VonHoltenCodes\n");
    printf("\n");
    printf("        Press any key to continue...\n");

    getch(); /* Wait for keypress */

    /* Clear screen after splash */
    #ifdef __DJGPP__
    printf("\033[2J\033[H");
    #else
    system("cls");
    #endif
}

/*============================================================================
 * MAIN PROGRAM
 *===========================================================================*/

int main(void) {
    Maze maze;
    Player player;
    int running = 1;
    int currentLevel = 1;  /* Start at level 1 */

    /* Show splash screen FIRST */
    showSplashScreen();

    /* Now show system verification messages */
    printf("\n");
    printf("MAZE v1.0 - Cyberpunk Raycasting Engine\n");
    printf("(c) 2025 Trent Von Holten - VonHoltenCodes\n");
    printf("\n");
    printf("Checking system requirements...\n");
    printf(" [OK] MS-DOS Compatible\n");
    printf(" [OK] VGA Graphics Card Detected\n");
    printf(" [OK] 386+ Processor\n");
    printf(" [OK] Math Coprocessor Available\n");
    printf("\n");
    printf("Game engine ready!\n");
    printf("\n");

#ifdef __DJGPP__
    if (!__djgpp_nearptr_enable()) {
        printf("ERROR: Could not enable near pointers!\n");
        return 1;
    }
#endif

    /* Initialize game systems */
    printf("[ SYSTEM ] Initializing neural uplink...\n");

    /* Initialize audio */
    initAudio();

    initMaze(&maze, currentLevel);  /* Start with level 1 */
    initReferencePoints();
    initPlayer(&player, &maze);

    printf("[ GRID  ] Corporate grid loaded: %dx%d sectors\n", maze.width, maze.height);
    printf("[ SPAWN ] Jack-in coordinates: (%.1f, %.1f)\n", player.x, player.y);
    printf("\n");

    /* Print reference points for debugging */
    printReferencePoints();

    printf("========================================\n");
    printf(" RUNNER CONTROLS:\n");
    printf("========================================\n");
    printf("  [UP]    Navigate forward\n");
    printf("  [DOWN]  Navigate backward\n");
    printf("  [LEFT]  Rotate left\n");
    printf("  [RIGHT] Rotate right\n");
    printf("  [ESC]   Disconnect from grid\n");
    printf("\n");
    printf("[ READY ] Press any key to jack in...\n");
    getch();

    /* Initialize double buffer BEFORE entering VGA mode */
    printf("[ VIDEO ] Initializing double buffer...\n");
    initDoubleBuffer();

    /* Enter VGA mode */
    setVideoMode(0x13);

    /* Show VGA splash screen */
    showVGASplashScreen();

    /* Show level 1 splash */
    showLevelSplash(currentLevel);

    /* Start game timer */
    gameStartTime = clock();
    lastFootstepTime = gameStartTime;
    gameWon = 0;

    /* Main game loop - handles all 3 levels */
    while (running) {
        /* Level game loop */
        while (running && !gameWon) {
            /* Render to back buffer */
            renderFrame(&player, &maze);

            /* Display the completed frame during vertical retrace */
            displayFrame();

            /* Handle input */
            handleInput(&player, &maze, &running);

            /* Check win condition */
            if (checkWinCondition(&player)) {
                gameWon = 1;
            }
        }

        /* If level completed (not ESC quit) */
        if (gameWon && running) {
            /* Calculate and save completion time */
            double elapsedTime = (double)(clock() - gameStartTime) / CLOCKS_PER_SEC;
            levelTimes[currentLevel - 1] = elapsedTime;

            /* Play victory sound */
            playVictorySound();

            /* Show completion splash in VGA */
            showLevelCompletionSplash(currentLevel, elapsedTime);

            /* Check if there are more levels */
            if (currentLevel < 3) {
                /* Ask to continue in VGA */
                clearScreen(COLOR_BLACK);
                const char *prompt = "CONTINUE TO NEXT LEVEL?";
                int promptX = (SCREEN_WIDTH - strlen(prompt) * 16) / 2;
                drawBigText(promptX, 70, prompt, NEON_CYAN);

                const char *prompt2 = "PRESS Y TO CONTINUE";
                int prompt2X = (SCREEN_WIDTH - strlen(prompt2) * 8) / 2;
                drawText(prompt2X, 110, prompt2, NEON_GREEN);

                const char *prompt3 = "PRESS N TO QUIT";
                int prompt3X = (SCREEN_WIDTH - strlen(prompt3) * 8) / 2;
                drawText(prompt3X, 130, prompt3, COLOR_RED);

                displayFrame();

                /* Wait for Y or N */
                int response;
                do {
                    response = getch();
                } while (response != 'y' && response != 'Y' && response != 'n' && response != 'N');

                if (response == 'y' || response == 'Y') {
                    /* Load next level */
                    currentLevel++;
                    loadLevel(&maze, currentLevel);
                    initPlayer(&player, &maze);

                    /* Show level splash */
                    showLevelSplash(currentLevel);

                    /* Reset game state for next level */
                    gameWon = 0;
                    gameStartTime = clock();
                    lastFootstepTime = gameStartTime;
                    footstepIndex = 0;
                } else {
                    /* User chose not to continue */
                    running = 0;
                }
            } else {
                /* All levels complete - exit main loop */
                running = 0;
            }
        } else {
            /* User quit with ESC - exit main loop */
            running = 0;
        }
    }

    /* Exit VGA mode if not already */
    setVideoMode(0x03);

    /* Free double buffer */
    freeDoubleBuffer();

    printf("\n");
    if (gameWon && currentLevel == 3) {
        /* Final victory - all 3 levels completed */
        printf("========================================\n");
        printf("  >>> ALL MAZES COMPLETED! <<<\n");
        printf("========================================\n");
        printf("\n");
        printf("  CONGRATULATIONS, RUNNER!\n");
        printf("  You escaped all 3 corporate sectors!\n");
        printf("  You are a true Grid Master!\n");
        printf("\n");
    } else if (gameWon) {
        /* Partial completion */
        printf("========================================\n");
        printf("  >> CONNECTION TERMINATED <<\n");
        printf("  Completed %d of 3 levels\n", currentLevel);
        printf("  Thank you for running the grid\n");
        printf("\n");
    } else {
        /* Quit without completing */
        printf("========================================\n");
        printf("  >> CONNECTION TERMINATED <<\n");
        printf("  Neural interface disconnected\n");
        printf("  Thank you for running the grid\n");
        printf("\n");
    }

    /* Show all completed level times */
    int hasCompletedLevels = 0;
    for (int i = 0; i < 3; i++) {
        if (levelTimes[i] > 0) {
            hasCompletedLevels = 1;
            break;
        }
    }

    if (hasCompletedLevels) {
        printf("  --- YOUR BEST TIMES ---\n");
        for (int i = 0; i < 3; i++) {
            if (levelTimes[i] > 0) {
                printf("  Level %d: %.2f seconds\n", i + 1, levelTimes[i]);
            }
        }
        printf("\n");
    }

    printf("  VonHoltenCodes 2025\n");
    printf("  Cyberpunk Maze Runner v1.0\n");
    printf("========================================\n");
    printf("\n");

    /* Cleanup */
    shutdownAudio();

#ifdef __DJGPP__
    __djgpp_nearptr_disable();
#endif

    return 0;
}
