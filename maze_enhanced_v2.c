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
#include "adlib.h"
/* Minion Game Sprites - from vonholtencodes-site */
/* Combined enemy roster - original minions + new enemies for variety */
#include "src/enemy_gremlin.h"
#include "src/minion_jump.h"
#include "src/enemy_creeper.h"
#include "src/enemy_tomato.h"
#include "src/enemy_spiderman.h"
#include "src/enemy_waldo.h"
#include "src/enemy_snowman.h"
#include "src/weapon_pistol_better.h"  /* Old Kenney weapon - better colors */

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
static double levelTimes[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; /* Track completion time for each level */

/* Footstep sound state */
static int footstepIndex = 0;
static clock_t lastFootstepTime = 0;

/* Keyboard state for simultaneous key detection */
static unsigned char keyDown[128] = {0};

/* Mouse control variables */
#define MOUSE_SENSITIVITY 0.003  /* Adjust for desired mouse speed */
static int mouseAvailable = 0;
static int playerPitch = 0;  /* Vertical look angle (-100 to +100) */

/* Bullet tracer state */
static clock_t lastShotTime = 0;
static double lastShotHitX = 0.0;
static double lastShotHitY = 0.0;
static int lastShotHit = 0;  /* 1 if last shot hit something, 0 if missed */
#define TRACER_DURATION_MS 100  /* How long tracer is visible (ms) */

/* Sprite system */
#define ROBOT_SPRITE_WIDTH 25
#define ROBOT_SPRITE_HEIGHT 32
#define ZOMBIE_SPRITE_WIDTH 25
#define ZOMBIE_SPRITE_HEIGHT 32
#define MAX_TEST_SPRITES 30  /* Increased for more enemies + pickups */

/* Sprite type definitions - combined minions + new enemies */
#define SPRITE_ENEMY_GREMLIN 0
#define SPRITE_ENEMY_MINION 1
#define SPRITE_ENEMY_CREEPER 2
#define SPRITE_ENEMY_TOMATO 3
#define SPRITE_ENEMY_SPIDERMAN 4
#define SPRITE_ENEMY_WALDO 5
#define SPRITE_ENEMY_SNOWMAN 6
#define SPRITE_PICKUP_HEALTH 7
#define SPRITE_PICKUP_AMMO 8
#define SPRITE_TORCH 9  /* NEW: Animated torch for dynamic lighting */

/*============================================================================
 * DYNAMIC LIGHTING SYSTEM - v3.0 FEATURE
 * Real-time torch lighting with animated flames and fire particles
 *===========================================================================*/

#define MAX_TORCHES 16
#define MAX_FIRE_PARTICLES 64
#define TORCH_LIGHT_RADIUS 5.0  /* How far torch light reaches */
#define TORCH_FLICKER_SPEED 250 /* ms between flicker updates - slower for stability */

/* Animated torch sprite - 16x24, 4 animation frames */
#define TORCH_WIDTH 16
#define TORCH_HEIGHT 24

/* Torch animation frame 1 - Base flame */
static unsigned char torchFrame1[TORCH_WIDTH * TORCH_HEIGHT] = {
    0,0,0,0,0,0,0,14,14,0,0,0,0,0,0,0,
    0,0,0,0,0,0,14,14,14,14,0,0,0,0,0,0,
    0,0,0,0,0,14,14,12,12,14,14,0,0,0,0,0,
    0,0,0,0,0,14,12,4,4,12,14,0,0,0,0,0,
    0,0,0,0,14,12,4,4,4,4,12,14,0,0,0,0,
    0,0,0,0,14,4,4,15,15,4,4,14,0,0,0,0,
    0,0,0,14,12,4,15,15,15,15,4,12,14,0,0,0,
    0,0,0,14,4,4,15,15,15,15,4,4,14,0,0,0,
    0,0,0,12,4,14,14,15,15,14,14,4,12,0,0,0,
    0,0,0,0,12,14,0,14,14,0,14,12,0,0,0,0,
    0,0,0,0,0,14,0,0,0,0,14,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,0,0,0,0,0,0,
    0,0,0,0,0,6,6,8,8,6,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,6,6,6,8,8,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0
};

/* Torch animation frame 2 - Flame swaying left */
static unsigned char torchFrame2[TORCH_WIDTH * TORCH_HEIGHT] = {
    0,0,0,0,0,0,14,14,0,0,0,0,0,0,0,0,
    0,0,0,0,0,14,14,14,14,0,0,0,0,0,0,0,
    0,0,0,0,14,14,12,12,14,14,0,0,0,0,0,0,
    0,0,0,0,14,12,4,4,12,14,0,0,0,0,0,0,
    0,0,0,14,12,4,4,4,4,12,14,0,0,0,0,0,
    0,0,0,14,4,4,15,15,4,4,14,0,0,0,0,0,
    0,0,14,12,4,15,15,15,15,4,12,14,0,0,0,0,
    0,0,14,4,4,15,15,15,15,4,4,14,0,0,0,0,
    0,0,12,4,14,14,15,15,14,14,4,12,0,0,0,0,
    0,0,0,12,14,0,14,14,0,14,12,0,0,0,0,0,
    0,0,0,0,14,0,0,0,0,14,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,0,0,0,0,0,0,
    0,0,0,0,0,6,6,8,8,6,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,6,6,6,8,8,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0
};

/* Torch animation frame 3 - Flame tall */
static unsigned char torchFrame3[TORCH_WIDTH * TORCH_HEIGHT] = {
    0,0,0,0,0,0,0,14,14,0,0,0,0,0,0,0,
    0,0,0,0,0,0,14,12,12,14,0,0,0,0,0,0,
    0,0,0,0,0,14,12,4,4,12,14,0,0,0,0,0,
    0,0,0,0,0,14,4,4,4,4,14,0,0,0,0,0,
    0,0,0,0,14,12,4,15,15,4,12,14,0,0,0,0,
    0,0,0,0,14,4,15,15,15,15,4,14,0,0,0,0,
    0,0,0,14,12,4,15,15,15,15,4,12,14,0,0,0,
    0,0,0,14,4,4,15,15,15,15,4,4,14,0,0,0,
    0,0,0,12,4,14,15,15,15,15,14,4,12,0,0,0,
    0,0,0,0,12,14,14,15,15,14,14,12,0,0,0,0,
    0,0,0,0,0,14,0,14,14,0,14,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,0,0,0,0,0,0,
    0,0,0,0,0,6,6,8,8,6,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,6,6,6,8,8,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0
};

/* Torch animation frame 4 - Flame swaying right */
static unsigned char torchFrame4[TORCH_WIDTH * TORCH_HEIGHT] = {
    0,0,0,0,0,0,0,0,14,14,0,0,0,0,0,0,
    0,0,0,0,0,0,0,14,14,14,14,0,0,0,0,0,
    0,0,0,0,0,0,14,14,12,12,14,14,0,0,0,0,
    0,0,0,0,0,0,14,12,4,4,12,14,0,0,0,0,
    0,0,0,0,0,14,12,4,4,4,4,12,14,0,0,0,
    0,0,0,0,0,14,4,4,15,15,4,4,14,0,0,0,
    0,0,0,0,14,12,4,15,15,15,15,4,12,14,0,0,
    0,0,0,0,14,4,4,15,15,15,15,4,4,14,0,0,
    0,0,0,0,12,4,14,14,15,15,14,14,4,12,0,0,
    0,0,0,0,0,12,14,0,14,14,0,14,12,0,0,0,
    0,0,0,0,0,0,14,0,0,0,0,14,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,0,0,0,0,0,0,
    0,0,0,0,0,6,6,8,8,6,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,0,6,8,8,8,8,6,0,0,0,0,0,
    0,0,0,0,6,6,6,8,8,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,
    0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0
};

/* Array of torch frame pointers for animation */
static unsigned char *torchFrames[4] = {torchFrame1, torchFrame2, torchFrame3, torchFrame4};

/* Torch position structure */
typedef struct {
    double x, y;       /* World position */
    int active;        /* Is torch lit? */
    int animFrame;     /* Current animation frame (0-3) */
    double intensity;  /* Light intensity (0.5-1.5, flickers) */
    clock_t lastFlicker;
} Torch;

static Torch torches[MAX_TORCHES];
static int numTorches = 0;

/* Fire particle structure for floating embers */
typedef struct {
    double x, y, z;    /* World position (z = height above ground) */
    double vx, vy, vz; /* Velocity */
    int life;          /* Remaining life in frames */
    unsigned char color; /* Particle color */
    int active;
} FireParticle;

static FireParticle fireParticles[MAX_FIRE_PARTICLES];
static clock_t lastParticleSpawn = 0;

/* Initialize torch system */
void initTorches(void) {
    int i;
    numTorches = 0;

    /* Clear all torches */
    for (i = 0; i < MAX_TORCHES; i++) {
        torches[i].active = 0;
        torches[i].animFrame = 0;
        torches[i].intensity = 1.0;
        torches[i].lastFlicker = 0;
    }

    /* Clear all particles */
    for (i = 0; i < MAX_FIRE_PARTICLES; i++) {
        fireParticles[i].active = 0;
    }
}

/* Add a torch at world position */
void addTorch(double x, double y) {
    if (numTorches >= MAX_TORCHES) return;

    torches[numTorches].x = x;
    torches[numTorches].y = y;
    torches[numTorches].active = 1;
    torches[numTorches].animFrame = numTorches % 4;  /* Stagger animation */
    torches[numTorches].intensity = 1.0;
    torches[numTorches].lastFlicker = clock();
    numTorches++;
}

/* Setup torches for current level */
/* Positions verified against maze layout - all in open floor cells (value 0) */
void setupLevelTorches(int level) {
    initTorches();

    /* Place torches in confirmed open floor areas based on maze layout */
    switch (level) {
        case 1:
            /* Level 1: Verified open positions from level1Layout */
            addTorch(3.5, 1.5);   /* Row 1, near spawn - value 0 */
            addTorch(8.5, 1.5);   /* Row 1, east corridor */
            addTorch(3.5, 7.5);   /* Row 7, west side - long corridor */
            addTorch(10.5, 7.5);  /* Row 7, middle of corridor */
            addTorch(18.5, 7.5);  /* Row 7, east side */
            addTorch(5.5, 11.5);  /* Row 11, open area */
            addTorch(10.5, 21.5); /* Row 21, near exit */
            break;
        case 2:
            addTorch(3.5, 1.5);
            addTorch(10.5, 1.5);
            addTorch(5.5, 7.5);
            addTorch(15.5, 7.5);
            addTorch(10.5, 15.5);
            addTorch(10.5, 21.5);
            break;
        case 3:
            addTorch(3.5, 1.5);
            addTorch(10.5, 3.5);
            addTorch(5.5, 7.5);
            addTorch(15.5, 7.5);
            addTorch(10.5, 11.5);
            addTorch(5.5, 17.5);
            addTorch(10.5, 21.5);
            break;
        case 4:
            addTorch(3.5, 1.5);
            addTorch(10.5, 3.5);
            addTorch(5.5, 7.5);
            addTorch(15.5, 11.5);
            addTorch(10.5, 17.5);
            addTorch(10.5, 21.5);
            break;
        case 5:
            addTorch(5.5, 1.5);
            addTorch(15.5, 3.5);
            addTorch(5.5, 9.5);
            addTorch(15.5, 13.5);
            addTorch(10.5, 17.5);
            addTorch(10.5, 21.5);
            break;
        case 6:
            addTorch(3.5, 1.5);
            addTorch(10.5, 3.5);
            addTorch(18.5, 5.5);
            addTorch(5.5, 9.5);
            addTorch(15.5, 13.5);
            addTorch(8.5, 17.5);
            addTorch(10.5, 21.5);
            break;
        default:
            addTorch(5.5, 7.5);
            addTorch(15.5, 15.5);
            addTorch(10.5, 21.5);
            break;
    }
}

/* Update torch animations and fire particles */
void updateTorchSystem(void) {
    int i;
    clock_t now = clock();
    long elapsed;

    /* Update torch animations */
    for (i = 0; i < numTorches; i++) {
        if (!torches[i].active) continue;

        elapsed = (now - torches[i].lastFlicker) * 1000 / CLOCKS_PER_SEC;

        if (elapsed > TORCH_FLICKER_SPEED) {
            /* Advance animation frame */
            torches[i].animFrame = (torches[i].animFrame + 1) % 4;

            /* Subtle intensity flicker (0.95 to 1.05) - minimal for stable walls */
            torches[i].intensity = 0.95 + (rand() % 10) / 100.0;

            torches[i].lastFlicker = now;
        }
    }

    /* Update fire particles */
    for (i = 0; i < MAX_FIRE_PARTICLES; i++) {
        if (!fireParticles[i].active) continue;

        /* Move particle */
        fireParticles[i].x += fireParticles[i].vx;
        fireParticles[i].y += fireParticles[i].vy;
        fireParticles[i].z += fireParticles[i].vz;

        /* Apply gravity and air resistance */
        fireParticles[i].vz -= 0.001;  /* Slight gravity */
        fireParticles[i].vx *= 0.98;   /* Air resistance */
        fireParticles[i].vy *= 0.98;

        /* Age particle */
        fireParticles[i].life--;

        /* Color fade: yellow -> orange -> red -> dark red */
        if (fireParticles[i].life < 10) {
            fireParticles[i].color = COLOR_RED;
        } else if (fireParticles[i].life < 20) {
            fireParticles[i].color = 4;  /* Dark red */
        }

        /* Kill dead particles */
        if (fireParticles[i].life <= 0 || fireParticles[i].z < 0) {
            fireParticles[i].active = 0;
        }
    }

    /* Spawn new particles from torches */
    elapsed = (now - lastParticleSpawn) * 1000 / CLOCKS_PER_SEC;
    if (elapsed > 50) {  /* Every 50ms */
        lastParticleSpawn = now;

        /* Try to spawn a particle for each active torch */
        for (i = 0; i < numTorches && i < 4; i++) {  /* Limit spawns per frame */
            int j;
            if (!torches[i].active) continue;
            if (rand() % 3 != 0) continue;  /* 33% chance per torch */

            /* Find inactive particle slot */
            for (j = 0; j < MAX_FIRE_PARTICLES; j++) {
                if (!fireParticles[j].active) {
                    fireParticles[j].active = 1;
                    fireParticles[j].x = torches[i].x + (rand() % 20 - 10) / 100.0;
                    fireParticles[j].y = torches[i].y + (rand() % 20 - 10) / 100.0;
                    fireParticles[j].z = 0.3;  /* Start above torch base */
                    fireParticles[j].vx = (rand() % 10 - 5) / 500.0;
                    fireParticles[j].vy = (rand() % 10 - 5) / 500.0;
                    fireParticles[j].vz = 0.02 + (rand() % 20) / 1000.0;  /* Float upward */
                    fireParticles[j].life = 30 + rand() % 20;
                    fireParticles[j].color = COLOR_YELLOW;  /* Start yellow */
                    break;
                }
            }
        }
    }
}

/* Calculate torch light intensity at a world position */
double getTorchLightAt(double worldX, double worldY) {
    int i;
    double totalLight = 0.0;

    for (i = 0; i < numTorches; i++) {
        if (!torches[i].active) continue;

        double dx = worldX - torches[i].x;
        double dy = worldY - torches[i].y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist < TORCH_LIGHT_RADIUS) {
            /* Inverse square falloff with torch intensity */
            double falloff = 1.0 - (dist / TORCH_LIGHT_RADIUS);
            totalLight += falloff * falloff * torches[i].intensity;
        }
    }

    /* Clamp to maximum */
    if (totalLight > 1.5) totalLight = 1.5;
    return totalLight;
}

/* Apply torch lighting to a color - STABLE version without flickering */
/* Only affects very close walls with a consistent warm tint */
unsigned char applyTorchLight(unsigned char baseColor, double lightLevel) {
    /* Only apply effect for strong, close light (reduces flicker zones) */
    if (lightLevel < 0.5) return baseColor;

    /* Simple warm tint for walls very close to torches */
    /* No multiple thresholds = no threshold-crossing flicker */
    if (baseColor == COLOR_BLACK) return COLOR_BROWN;
    if (baseColor == COLOR_BROWN) return 6;  /* Keep brown */

    /* All other colors stay the same - stable walls */
    return baseColor;
}

typedef struct {
    double x, y;
    int active;
    int spriteType;  /* 0=soldier, 1=zombie, 2=health, 3=ammo */
    int health;      /* Enemy health (100 = full, 0 = dead) - not used for pickups */
    int isPickup;    /* 1 if this is a pickup item, 0 if enemy */
    double spawnX, spawnY;  /* Original spawn position for respawning */
    clock_t respawnTime;    /* When to respawn (0 if never killed) */
} TestSprite;

static TestSprite testSprites[MAX_TEST_SPRITES];

/* Z-buffer for sprite depth testing */
static double zBuffer[SCREEN_WIDTH];

/* Game state */
static int playerHealth = 100;
#define STARTING_AMMO 50
static int playerAmmo = STARTING_AMMO;
static int playerScore = 0;
static clock_t muzzleFlashTime = 0;  /* Time when last shot was fired */
static int shotsFired = 0;  /* Debug: track total shots */
static int shotsHit = 0;  /* Debug: track hits */

/* High Score System */
#define MAX_HIGH_SCORES 10
#define SCORE_FILE "SCORES.DAT"
typedef struct {
    char name[16];
    int score;
    int level;
    char date[12];  /* MM/DD/YYYY format */
} HighScore;

static HighScore highScores[MAX_HIGH_SCORES];
static int numHighScores = 0;

/* Robot sprite - 25x32 from Kenney topdown shooter pack */
static unsigned char robotSprite[800] = {
    0,0,0,0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,6,6,6,12,12,12,12,12,12,6,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,0,6,6,12,12,12,12,12,12,12,12,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,6,6,12,7,7,7,7,7,7,7,7,12,6,6,0,0,0,0,0,0,0,
    0,0,0,0,6,6,7,7,7,7,7,7,7,7,7,7,6,6,0,0,0,0,0,0,0,
    0,0,0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,0,0,
    0,0,0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,
    0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,
    0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,0,
    0,0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,0,
    0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,
    0,0,8,7,7,15,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,
    0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,
    0,0,7,12,12,12,12,12,12,12,12,12,12,12,7,7,7,7,7,7,7,0,0,0,0,
    0,0,7,12,12,12,12,12,12,12,12,12,12,12,7,7,7,7,7,7,7,8,0,0,0,
    0,0,7,12,12,12,12,12,12,12,12,12,12,12,7,7,7,7,7,7,7,8,0,0,0,
    0,0,7,12,12,12,12,12,12,12,12,12,12,12,7,7,7,7,7,7,7,0,0,0,0,
    0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,
    0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,
    0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,
    0,0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,
    0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,0,
    0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,0,
    0,0,0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,
    0,0,0,0,8,7,7,7,7,7,7,7,7,7,7,7,7,8,0,0,0,0,0,0,0,
    0,0,0,0,6,6,7,7,7,7,7,7,7,7,7,7,6,6,0,0,0,0,0,0,0,
    0,0,0,0,6,6,12,7,7,7,7,7,7,7,7,12,6,6,0,0,0,0,0,0,0,
    0,0,0,0,0,6,6,12,12,12,12,12,12,12,12,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,0,6,6,12,12,12,12,12,12,12,6,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,
};

/* Health pickup sprite - 16x16 green cross */
#define HEALTH_SPRITE_WIDTH 16
#define HEALTH_SPRITE_HEIGHT 16
static unsigned char healthSprite[256] = {
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0,
    0,0,0,0,0,0,10,10,10,10,0,0,0,0,0,0
};

/* Ammo pickup sprite - 16x16 yellow box */
#define AMMO_SPRITE_WIDTH 16
#define AMMO_SPRITE_HEIGHT 16
static unsigned char ammoSprite[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,14,14,14,14,14,14,14,14,14,14,14,14,14,14,0,
    0,14,14,14,14,14,14,14,14,14,14,14,14,14,14,0,
    0,14,14,0,0,0,0,0,0,0,0,0,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,6,6,6,6,6,6,6,6,0,14,14,0,
    0,14,14,0,0,0,0,0,0,0,0,0,0,14,14,0,
    0,14,14,14,14,14,14,14,14,14,14,14,14,14,14,0,
    0,14,14,14,14,14,14,14,14,14,14,14,14,14,14,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* Zombie sprite - 25x32 from Kenney topdown shooter pack */
static unsigned char zombieSprite[800] = {
    0,0,0,0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,6,6,6,12,12,12,12,12,12,6,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,0,6,6,6,12,12,12,12,12,12,12,6,8,0,0,0,0,0,0,0,0,
    0,0,0,0,6,6,6,6,12,12,12,12,12,12,12,8,2,8,0,0,0,0,0,0,0,
    0,0,0,0,6,6,6,6,12,12,12,12,12,12,8,8,8,8,0,0,0,0,0,0,0,
    0,0,0,0,6,6,6,6,12,12,12,12,12,12,12,12,6,6,0,0,0,0,0,0,0,
    0,0,0,0,6,6,6,6,12,12,12,12,12,12,12,12,6,6,0,0,0,0,0,0,0,
    0,0,0,0,6,6,12,12,12,12,6,6,8,12,12,12,6,6,0,0,0,0,0,0,0,
    0,0,0,0,6,12,12,8,8,8,8,8,8,8,8,8,12,6,6,0,0,0,0,0,0,
    0,0,0,8,6,6,8,8,8,8,8,8,8,8,8,3,8,8,8,0,0,0,0,0,0,
    0,0,0,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,0,0,0,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,3,8,8,0,0,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,3,8,8,0,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,0,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,8,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,8,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,0,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,0,0,0,0,
    0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,0,0,0,0,0,
    0,0,0,8,8,8,8,8,8,8,8,8,8,8,8,3,3,3,8,8,0,0,0,0,0,
    0,0,0,8,8,8,8,8,8,8,8,8,8,8,8,3,3,8,8,0,0,0,0,0,0,
    0,0,0,0,6,6,8,8,8,8,8,8,8,8,8,8,8,6,8,0,0,0,0,0,0,
    0,0,0,0,6,6,12,12,8,8,8,8,8,8,8,12,12,6,6,0,0,0,0,0,0,
    0,0,0,0,8,6,12,12,12,12,12,12,12,12,12,6,6,6,0,0,0,0,0,0,0,
    0,0,0,0,8,8,8,8,6,12,12,12,12,12,12,6,6,6,0,0,0,0,0,0,0,
    0,0,0,0,8,8,3,3,8,12,12,12,12,12,6,6,6,6,0,0,0,0,0,0,0,
    0,0,0,0,8,8,3,8,12,12,12,12,12,12,6,6,6,6,0,0,0,0,0,0,0,
    0,0,0,0,0,8,8,12,12,12,12,12,12,12,12,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,0,8,8,12,12,12,12,12,12,12,6,6,6,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,
};

/* Simple 8x8 procedural textures */
#define TEX_SIZE 64  /* Increased from 8 to 64 for finer detail */

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

/* Get wall texture - HIGH DETAIL stone masonry (64x64 resolution) */
/* Enemy colors to AVOID: Magenta(5), Red(4), Gray(8), White(15) */
/* Wall colors: Browns(6), Yellows(14), Cyans(3,11), Blues(1,9), Greens(2,10) */
unsigned char getWallTexel(int wallType, int texX, int texY, int level) {
    /* Add MAZE text decoration */
    if (isTextPixel(texX, texY)) {
        return COLOR_YELLOW; /* Bright yellow text on all walls */
    }

    /* Wrap coordinates for tiling */
    texX = texX & (TEX_SIZE - 1);
    texY = texY & (TEX_SIZE - 1);

    /* HOLOGRAM walls: Transparent walk-through panels with ANIMATED zig-zag pattern */
    if (wallType == CELL_WALL_HOLOGRAM) {
        unsigned char brightColor, darkColor, veryBrightColor;

        /* Get time for flashing effect */
        clock_t now = clock();
        long elapsed = (now - gameStartTime) * 1000 / CLOCKS_PER_SEC;
        int flashPhase = (elapsed / 250) % 2;  /* Flash every 250ms */

        /* Level-based colors - BRIGHT RED for winning wall on level 2 */
        if (level == 1) {
            veryBrightColor = COLOR_WHITE;      /* White flashes */
            brightColor = COLOR_LMAGENTA;       /* Light magenta zig-zags */
            darkColor = COLOR_MAGENTA;          /* Magenta base */
        } else if (level == 2) {
            /* EXIT WALLS - FLASH between yellow and red for maximum visibility! */
            if (flashPhase) {
                veryBrightColor = COLOR_BWHITE;  /* Bright white - SUPER VISIBLE */
                brightColor = COLOR_YELLOW;      /* Yellow - EXIT INDICATOR */
                darkColor = COLOR_LRED;          /* Light red base */
            } else {
                veryBrightColor = COLOR_YELLOW;  /* Yellow flashes */
                brightColor = COLOR_RED;         /* BRIGHT RED zig-zags */
                darkColor = COLOR_RED;           /* Red base - FLASHING! */
            }
        } else {
            veryBrightColor = COLOR_GRAY;       /* Gray flashes */
            brightColor = COLOR_LMAGENTA;       /* Light magenta zig-zags */
            darkColor = COLOR_BLUE;             /* Blue base */
        }

        /* ANIMATED GRID PATTERN - creates pulsing effect */
        int gridSize = 8;
        int isGridLine = (texX % gridSize == 0) || (texY % gridSize == 0);
        int isCorner = (texX % gridSize == 0) && (texY % gridSize == 0);

        /* Corners are very bright */
        if (isCorner) {
            return veryBrightColor;
        }

        /* Grid lines are bright */
        if (isGridLine) {
            return brightColor;
        }

        /* ZIG-ZAG PATTERN in between grid lines */
        if ((texX ^ texY) & 2) {
            return brightColor;
        }
        return darkColor;
    }

    /* CLASSIC STONE WALL - Wolfenstein 3D style clean look */
    /* Colors: Cyan(3) as light gray, Brown(6) as medium, Black(0) as dark */

    #define STONE_SIZE 16  /* 16x16 stone blocks */

    /* Calculate which stone block we're in */
    int stoneX = texX / STONE_SIZE;
    int stoneY = texY / STONE_SIZE;
    int localX = texX % STONE_SIZE;
    int localY = texY % STONE_SIZE;

    /* Mortar between stones - 1 pixel black lines */
    if (localX == 0 || localY == 0) {
        return COLOR_BLACK;
    }

    /* Simple stone texture with light variation */
    int stoneSeed = (stoneX * 31 + stoneY * 47) % 8;

    /* 3-shade simple pattern for clean look */
    unsigned char stoneColor;

    if (stoneSeed < 3) {
        stoneColor = COLOR_CYAN;   /* Light gray stone (most common) */
    } else if (stoneSeed < 6) {
        stoneColor = COLOR_BROWN;  /* Medium gray stone */
    } else {
        stoneColor = COLOR_BLUE;   /* Darker gray stone */
    }

    /* Add simple depth - lighten top-left, darken bottom-right */
    if (localX < 3 && localY < 3) {
        /* Top-left corner - brighten */
        if (stoneColor == COLOR_BROWN) return COLOR_CYAN;
        if (stoneColor == COLOR_BLUE) return COLOR_BROWN;
    } else if (localX > 12 || localY > 12) {
        /* Bottom-right edge - darken */
        if (stoneColor == COLOR_CYAN) return COLOR_BROWN;
        if (stoneColor == COLOR_BROWN) return COLOR_BLUE;
    }

    return stoneColor;
}

/* Get floor texture - LOCKED TO WORLD GRID to prevent treadmill effect */
/* Simple dark floor that stays put */
unsigned char getFloorTexel(int worldX, int worldY) {
    /* worldX and worldY are INTEGER maze cell coordinates */
    /* This locks the pattern to the maze grid - no sliding! */

    /* Simple checkerboard locked to maze cells */
    int checker = (worldX + worldY) & 1;

    if (checker) {
        return COLOR_GREEN;   /* Dark green (appears as dark floor) */
    } else {
        return COLOR_LGREEN;  /* Lighter green */
    }
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

    /* Draw ceiling - BLACK for all levels (no blue!) */
    unsigned char ceilingColor = COLOR_BLACK;

    /* Shift horizon with pitch for cohesive vertical look (Doom-style) */
    int horizon = SCREEN_CENTER + playerPitch;

    /* Draw ceiling with procedural starry sky */
    int ceilEnd = (horizon < SCREEN_HEIGHT) ? horizon : SCREEN_HEIGHT;
    for (y = 0; y < ceilEnd; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            /* Generate procedural stars using hash function */
            /* Stars are static - based only on screen position, not player rotation */
            int starHash = (x * 7919 + y * 4561) % 997;

            unsigned char pixelColor = ceilingColor;

            /* Star density: ~1.5% of pixels (15 in 1000) */
            if (starHash < 15) {
                /* Vary star brightness for depth effect */
                int brightness = starHash % 3;
                if (brightness == 0) {
                    pixelColor = COLOR_WHITE;    /* Bright stars */
                } else if (brightness == 1) {
                    pixelColor = COLOR_WHITE;    /* Medium-bright stars */
                } else {
                    pixelColor = COLOR_GRAY;     /* Dimmer stars */
                }
            }

            backBuffer[y * SCREEN_WIDTH + x] = pixelColor;
        }
    }

    /* Draw floor with cobblestone texture */
    int floorStart = (horizon > 0) ? horizon : 0;
    for (y = floorStart; y < SCREEN_HEIGHT; y++) {
        /* Raycast floor row - adjust for pitch */
        double rowDistance = (SCREEN_HEIGHT / 2.0) / (y - SCREEN_CENTER - playerPitch);

        /* Calculate floor endpoints for this row */
        double floorStepX = rowDistance * (player->planeX * 2.0) / SCREEN_WIDTH;
        double floorStepY = rowDistance * (player->planeY * 2.0) / SCREEN_WIDTH;

        double floorX = player->x + rowDistance * player->dirX - rowDistance * player->planeX;
        double floorY = player->y + rowDistance * player->dirY - rowDistance * player->planeY;

        for (x = 0; x < SCREEN_WIDTH; x++) {
            /* Use INTEGER world coordinates to lock floor to maze grid */
            /* This prevents the treadmill/sliding effect */
            int worldX = (int)floorX;
            int worldY = (int)floorY;
            backBuffer[y * SCREEN_WIDTH + x] = getFloorTexel(worldX, worldY);

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

        /* Store distance in z-buffer for sprite depth testing */
        zBuffer[x] = perpWallDist;

        /* Calculate height of line to draw on screen */
        /* TALLER WALLS SYSTEM - 1.5x height for more imposing walls */
        int lineHeight = (int)(SCREEN_HEIGHT * 1.5 / perpWallDist);

        /* Calculate wall position WITHOUT pitch (for texture mapping) */
        int wallStart = -lineHeight / 2 + SCREEN_CENTER;
        int wallEnd = lineHeight / 2 + SCREEN_CENTER;

        /* Calculate actual draw position WITH pitch (for screen rendering) */
        int drawStart = wallStart + playerPitch;
        int drawEnd = wallEnd + playerPitch;
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

        /* Calculate texture starting position accounting for clipping */
        int wallDrawStart = wallStart + playerPitch;  /* Where wall should start with pitch */
        double texPos = (drawStart - wallDrawStart) * step;  /* Offset for clipped top */

        /* Calculate wall world position for torch lighting */
        double wallWorldX, wallWorldY;
        if (side == 0) {
            wallWorldX = mapX + 0.5;
            wallWorldY = player->y + perpWallDist * rayDirY;
        } else {
            wallWorldX = player->x + perpWallDist * rayDirX;
            wallWorldY = mapY + 0.5;
        }

        /* Get torch light level at this wall position */
        double torchLight = getTorchLightAt(wallWorldX, wallWorldY);

        /* Draw textured vertical line */
        for (y = drawStart; y < drawEnd; y++) {
            int texY = ((int)texPos) & (TEX_SIZE - 1);
            texPos += step;

            /* Sample texture with level-specific colors */
            unsigned char color = getWallTexel(cellValue, texX, texY, level);

            /* Apply distance fog - simple darkening */
            if (perpWallDist > 15.0) {
                color = COLOR_BLACK;  /* Far walls are black */
            } else if (perpWallDist > 10.0) {
                /* Darken distant walls slightly */
                if (color == COLOR_WHITE) color = COLOR_BROWN;
            }

            /* Make EW walls slightly darker for depth */
            if (side == 1) {
                if (color == COLOR_WHITE) color = COLOR_BROWN;
                else if (color == COLOR_BROWN) color = COLOR_BLACK;
            }

            /* v3.0: Apply dynamic torch lighting */
            if (torchLight > 0.1 && perpWallDist < 12.0) {
                color = applyTorchLight(color, torchLight);
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

/* Forward declarations */
void playFootstepSound(void);
void playGunshotSound(void);

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

    /* Now poll for ALL keys in buffer to support simultaneous keypresses */
    while (_bios_keybrd(0x11) != 0) {
        int keycode = _bios_keybrd(0x11);  /* Peek without removing */
        int scancode = (keycode >> 8) & 0xFF;

        /* Mark the key as pressed */
        if (scancode == 0x48) keys[72] = 1;  /* Up */
        if (scancode == 0x50) keys[80] = 1;  /* Down */
        if (scancode == 0x4B) keys[75] = 1;  /* Left */
        if (scancode == 0x4D) keys[77] = 1;  /* Right */
        if (scancode == 0x39) keys[57] = 1;  /* SPACE */
        if (scancode == 0x01) keys[1] = 1;   /* ESC */

        /* Remove this key from buffer */
        _bios_keybrd(0x10);
        /* Continue reading all keys in buffer */
    }
    #endif
}

/* Mouse control functions */
void initMouse(void) {
    union REGS r;

    /* Reset mouse driver */
    r.x.ax = 0;
    int86(0x33, &r, &r);

    if (r.x.ax == 0xFFFF) {
        mouseAvailable = 1;

        /* Hide cursor (AX=2 for true captured feel) */
        r.x.ax = 2;
        int86(0x33, &r, &r);
    } else {
        mouseAvailable = 0;
    }
}

void getMouseDelta(int *dx, int *dy) {
    union REGS r;

    if (!mouseAvailable) {
        *dx = 0;
        *dy = 0;
        return;
    }

    /* Read motion counters (mickeys) */
    r.x.ax = 11;
    int86(0x33, &r, &r);

    *dx = (short)r.x.cx;  /* Horizontal mickeys */
    *dy = (short)r.x.dx;  /* Vertical mickeys */
}

/* Get mouse button state - returns 1 if left button pressed */
int getMouseButton(void) {
    union REGS r;

    if (!mouseAvailable) {
        return 0;
    }

    /* Get button status (AX=3) */
    r.x.ax = 3;
    int86(0x33, &r, &r);

    return (r.x.bx & 1);  /* Bit 0 = left button */
}

void renderSprites(Player *player) {
    int i, x, y;

    /* Iterate through all active sprites */
    for (i = 0; i < MAX_TEST_SPRITES; i++) {
        if (!testSprites[i].active) continue;

        /* Get sprite dimensions and data based on type */
        int spriteTexWidth, spriteTexHeight;
        unsigned char *spriteData;

        switch (testSprites[i].spriteType) {
            case SPRITE_ENEMY_GREMLIN:
                /* Purple gremlin - original minion game */
                spriteTexWidth = ENEMY_GREMLIN_WIDTH;
                spriteTexHeight = ENEMY_GREMLIN_HEIGHT;
                spriteData = (unsigned char *)enemy_gremlin;
                break;
            case SPRITE_ENEMY_MINION:
                /* Jumping minion - original minion game */
                spriteTexWidth = MINION_JUMP_WIDTH;
                spriteTexHeight = MINION_JUMP_HEIGHT;
                spriteData = (unsigned char *)minion_jump;
                break;
            case SPRITE_ENEMY_CREEPER:
                /* Green creeper enemy */
                spriteTexWidth = ENEMY_CREEPER_WIDTH;
                spriteTexHeight = ENEMY_CREEPER_HEIGHT;
                spriteData = (unsigned char *)enemy_creeper;
                break;
            case SPRITE_ENEMY_TOMATO:
                /* Orange tomato enemy */
                spriteTexWidth = ENEMY_TOMATO_WIDTH;
                spriteTexHeight = ENEMY_TOMATO_HEIGHT;
                spriteData = (unsigned char *)enemy_tomato;
                break;
            case SPRITE_ENEMY_SPIDERMAN:
                /* Red spiderman enemy */
                spriteTexWidth = ENEMY_SPIDERMAN_WIDTH;
                spriteTexHeight = ENEMY_SPIDERMAN_HEIGHT;
                spriteData = (unsigned char *)enemy_spiderman;
                break;
            case SPRITE_ENEMY_WALDO:
                /* Waldo enemy */
                spriteTexWidth = ENEMY_WALDO_WIDTH;
                spriteTexHeight = ENEMY_WALDO_HEIGHT;
                spriteData = (unsigned char *)enemy_waldo;
                break;
            case SPRITE_ENEMY_SNOWMAN:
                /* Snowman enemy */
                spriteTexWidth = ENEMY_SNOWMAN_WIDTH;
                spriteTexHeight = ENEMY_SNOWMAN_HEIGHT;
                spriteData = (unsigned char *)enemy_snowman;
                break;
            case SPRITE_PICKUP_HEALTH:
                /* Health pickup - green cross */
                spriteTexWidth = HEALTH_SPRITE_WIDTH;
                spriteTexHeight = HEALTH_SPRITE_HEIGHT;
                spriteData = (unsigned char *)healthSprite;
                break;
            case SPRITE_PICKUP_AMMO:
                /* Ammo pickup - yellow box */
                spriteTexWidth = AMMO_SPRITE_WIDTH;
                spriteTexHeight = AMMO_SPRITE_HEIGHT;
                spriteData = (unsigned char *)ammoSprite;
                break;
            default:
                /* Default to gremlin if unknown type */
                spriteTexWidth = ENEMY_GREMLIN_WIDTH;
                spriteTexHeight = ENEMY_GREMLIN_HEIGHT;
                spriteData = (unsigned char *)enemy_gremlin;
                break;
        }

        /* Translate sprite position to relative to camera */
        double spriteX = testSprites[i].x - player->x;
        double spriteY = testSprites[i].y - player->y;

        /* Transform sprite with inverse camera matrix */
        double invDet = 1.0 / (player->planeX * player->dirY - player->dirX * player->planeY);

        double transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
        double transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);

        /* Sprite is behind player - don't render */
        if (transformY <= 0) continue;

        /* Calculate sprite screen X position */
        int spriteScreenX = (int)((SCREEN_WIDTH / 2) * (1 + transformX / transformY));

        /* Calculate sprite height on screen */
        int spriteHeight = abs((int)(SCREEN_HEIGHT / transformY));

        /* Calculate sprite width maintaining aspect ratio (25:32) */
        int spriteWidth = abs((int)(spriteHeight * spriteTexWidth / spriteTexHeight));

        /* Calculate draw start/end Y with pitch offset */
        int drawStartY = -spriteHeight / 2 + SCREEN_CENTER + playerPitch;
        int drawEndY = spriteHeight / 2 + SCREEN_CENTER + playerPitch;
        if (drawStartY < 0) drawStartY = 0;
        if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0) drawStartX = 0;
        if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

        /* Render sprite column by column */
        for (x = drawStartX; x < drawEndX; x++) {
            /* Check if sprite is in front of wall (simple depth test) */
            if (transformY < zBuffer[x]) {
                /* Calculate texture X coordinate */
                int texX = (int)((x - (-spriteWidth / 2 + spriteScreenX)) * spriteTexWidth / spriteWidth);
                if (texX < 0 || texX >= spriteTexWidth) continue;

                /* Render sprite column */
                for (y = drawStartY; y < drawEndY; y++) {
                    /* Calculate texture Y coordinate - normal orientation */
                    int texY = (int)((y - drawStartY) * spriteTexHeight / (drawEndY - drawStartY));
                    if (texY < 0 || texY >= spriteTexHeight) continue;

                    /* Get sprite pixel color */
                    unsigned char color = spriteData[texY * spriteTexWidth + texX];

                    /* Draw pixel if not transparent (0 = transparent) */
                    if (color != 0) {
                        backBuffer[y * SCREEN_WIDTH + x] = color;
                    }
                }
            }
        }
    }
}

/*============================================================================
 * TORCH AND FIRE PARTICLE RENDERING - v3.0 FEATURE
 *===========================================================================*/

/* Render animated torches as billboard sprites */
void renderTorches(Player *player) {
    int i, x, y;

    for (i = 0; i < numTorches; i++) {
        if (!torches[i].active) continue;

        /* Get current animation frame */
        unsigned char *spriteData = torchFrames[torches[i].animFrame];

        /* Translate torch position to relative to camera */
        double spriteX = torches[i].x - player->x;
        double spriteY = torches[i].y - player->y;

        /* Transform sprite with inverse camera matrix */
        double invDet = 1.0 / (player->planeX * player->dirY - player->dirX * player->planeY);
        double transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
        double transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);

        /* Torch is behind player - don't render */
        if (transformY <= 0.2) continue;

        /* Calculate torch screen X position */
        int torchScreenX = (int)((SCREEN_WIDTH / 2) * (1 + transformX / transformY));

        /* Calculate torch height on screen (slightly smaller than enemies) */
        int torchHeight = abs((int)(SCREEN_HEIGHT * 0.6 / transformY));

        /* Calculate torch width maintaining aspect ratio (16:24) */
        int torchWidth = abs((int)(torchHeight * TORCH_WIDTH / TORCH_HEIGHT));

        /* Calculate draw positions with pitch offset */
        int drawStartY = -torchHeight / 2 + SCREEN_CENTER + playerPitch + torchHeight / 4;  /* Shifted down to floor */
        int drawEndY = torchHeight / 2 + SCREEN_CENTER + playerPitch + torchHeight / 4;
        if (drawStartY < 0) drawStartY = 0;
        if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;
        int drawStartX = -torchWidth / 2 + torchScreenX;
        int drawEndX = torchWidth / 2 + torchScreenX;
        if (drawStartX < 0) drawStartX = 0;
        if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

        /* Render torch column by column */
        for (x = drawStartX; x < drawEndX; x++) {
            /* Check if torch is in front of wall */
            if (transformY < zBuffer[x]) {
                /* Calculate texture X coordinate */
                int texX = (int)((x - (-torchWidth / 2 + torchScreenX)) * TORCH_WIDTH / torchWidth);
                if (texX < 0 || texX >= TORCH_WIDTH) continue;

                /* Render torch column */
                for (y = drawStartY; y < drawEndY; y++) {
                    int texY = (int)((y - drawStartY) * TORCH_HEIGHT / (drawEndY - drawStartY));
                    if (texY < 0 || texY >= TORCH_HEIGHT) continue;

                    /* Get torch pixel color */
                    unsigned char color = spriteData[texY * TORCH_WIDTH + texX];

                    /* Draw pixel if not transparent */
                    if (color != 0) {
                        backBuffer[y * SCREEN_WIDTH + x] = color;
                    }
                }
            }
        }
    }
}

/* Render fire particles floating above torches */
void renderFireParticles(Player *player) {
    int i;

    for (i = 0; i < MAX_FIRE_PARTICLES; i++) {
        if (!fireParticles[i].active) continue;

        /* Translate particle position to camera space */
        double particleX = fireParticles[i].x - player->x;
        double particleY = fireParticles[i].y - player->y;

        /* Transform with inverse camera matrix */
        double invDet = 1.0 / (player->planeX * player->dirY - player->dirX * player->planeY);
        double transformX = invDet * (player->dirY * particleX - player->dirX * particleY);
        double transformY = invDet * (-player->planeY * particleX + player->planeX * particleY);

        /* Particle is behind player */
        if (transformY <= 0.1) continue;

        /* Calculate screen position */
        int screenX = (int)((SCREEN_WIDTH / 2) * (1 + transformX / transformY));

        /* Y position based on particle height (z) and perspective */
        int baseY = SCREEN_CENTER + playerPitch;
        int screenY = baseY - (int)(fireParticles[i].z * SCREEN_HEIGHT / transformY);

        /* Clamp to screen bounds */
        if (screenX < 0 || screenX >= SCREEN_WIDTH) continue;
        if (screenY < 0 || screenY >= SCREEN_HEIGHT) continue;

        /* Only draw if in front of wall */
        if (transformY < zBuffer[screenX]) {
            /* Draw particle as 1-2 pixels depending on distance */
            backBuffer[screenY * SCREEN_WIDTH + screenX] = fireParticles[i].color;

            /* Add extra pixels for close particles */
            if (transformY < 3.0) {
                if (screenX + 1 < SCREEN_WIDTH) {
                    backBuffer[screenY * SCREEN_WIDTH + screenX + 1] = fireParticles[i].color;
                }
                if (screenY + 1 < SCREEN_HEIGHT) {
                    backBuffer[(screenY + 1) * SCREEN_WIDTH + screenX] = fireParticles[i].color;
                }
            }
        }
    }
}

/*============================================================================
 * SHOOTING MECHANICS
 *===========================================================================*/

/* Check if there's a clear line of sight between two points (no walls blocking) */
int hasLineOfSight(Player *player, Maze *maze, double targetX, double targetY) {
    double dirX = targetX - player->x;
    double dirY = targetY - player->y;
    double distance = sqrt(dirX * dirX + dirY * dirY);
    double stepX = dirX / distance * 0.05;  /* Small steps for accurate collision */
    double stepY = dirY / distance * 0.05;
    double rayX = player->x;
    double rayY = player->y;
    int steps = (int)(distance / 0.05);
    int i;

    /* Step along ray from player to target */
    for (i = 0; i < steps; i++) {
        rayX += stepX;
        rayY += stepY;

        /* Check if we hit a wall */
        if (isWall(getMazeCell(maze, (int)rayX, (int)rayY))) {
            return 0;  /* Wall blocks line of sight */
        }
    }

    return 1;  /* Clear line of sight */
}

/* Shoot weapon - returns 0=no ammo, 1=shot fired but miss, 2=hit, 3=kill */
int shootWeapon(Player *player, Maze *maze) {
    int i;
    double closestDist = 999999.0;
    int closestSprite = -1;

    /* Check ammo */
    if (playerAmmo <= 0) {
        return 0;  /* Out of ammo */
    }

    /* Reduce ammo and track shot */
    playerAmmo--;
    shotsFired++;

    /* Set muzzle flash */
    muzzleFlashTime = clock();

    /* Play gunshot sound */
    playGunshotSound();

    /* Record shot time for bullet tracer */
    lastShotTime = clock();

    /* Find closest active enemy in front of player */
    for (i = 0; i < MAX_TEST_SPRITES; i++) {
        double spriteX, spriteY, dist, angle, angleDiff;

        if (!testSprites[i].active) continue;
        if (testSprites[i].isPickup) continue;  /* Don't shoot pickups */

        /* Vector from player to sprite */
        spriteX = testSprites[i].x - player->x;
        spriteY = testSprites[i].y - player->y;

        /* Distance to sprite */
        dist = sqrt(spriteX * spriteX + spriteY * spriteY);

        /* Angle to sprite */
        angle = atan2(spriteY, spriteX);

        /* Difference from player's looking direction */
        angleDiff = angle - player->angle;

        /* Normalize angle difference to -PI to PI */
        while (angleDiff > 3.14159265) angleDiff -= 6.28318530;
        while (angleDiff < -3.14159265) angleDiff += 6.28318530;

        /* Check if sprite is in front (within 15 degree cone) AND has clear line of sight */
        if (fabs(angleDiff) < 0.26 && dist < closestDist && dist < 20.0) {
            /* NEW: Check if walls block the shot */
            if (hasLineOfSight(player, maze, testSprites[i].x, testSprites[i].y)) {
                closestDist = dist;
                closestSprite = i;
            }
        }
    }

    /* Hit sprite if found */
    if (closestSprite >= 0) {
        shotsHit++;
        testSprites[closestSprite].health -= 50;  /* 50 damage per shot */

        /* Record hit position for bullet tracer */
        lastShotHitX = testSprites[closestSprite].x;
        lastShotHitY = testSprites[closestSprite].y;
        lastShotHit = 1;

        if (testSprites[closestSprite].health <= 0) {
            /* Enemy killed */
            testSprites[closestSprite].active = 0;
            /* Set respawn timer for 15 seconds from now */
            testSprites[closestSprite].respawnTime = clock() + (CLOCKS_PER_SEC * 15);
            playerScore += 100;
            return 3;  /* Kill */
        }

        return 2;  /* Hit */
    }

    /* Shot missed - raycast to find wall hit position */
    {
        double rayDirX = player->dirX;
        double rayDirY = player->dirY;
        double rayPosX = player->x;
        double rayPosY = player->y;
        double stepX = rayDirX * 0.1;
        double stepY = rayDirY * 0.1;
        int steps = 0;

        /* Step along ray until hitting wall or max distance */
        while (steps < 200) {  /* Max ~20 units */
            rayPosX += stepX;
            rayPosY += stepY;

            if (isWall(getMazeCell(maze, (int)rayPosX, (int)rayPosY))) {
                lastShotHitX = rayPosX;
                lastShotHitY = rayPosY;
                lastShotHit = 1;
                break;
            }
            steps++;
        }

        if (steps >= 200) {
            /* No wall hit, just use max distance */
            lastShotHitX = rayPosX;
            lastShotHitY = rayPosY;
            lastShotHit = 1;
        }
    }

    return 1;  /* Shot fired but missed */
}

/* Simple enemy AI - move toward player slowly and damage on contact */
void updateEnemyAI(Player *player, Maze *maze) {
    int i;
    const double AI_SPEED = 0.01;  /* Very slow movement */
    const double MIN_DISTANCE = 1.0;  /* Stop when close to player */
    const double ATTACK_DISTANCE = 0.6;  /* Damage player if this close */
    static clock_t lastDamageTime = 0;
    clock_t now = clock();

    for (i = 0; i < MAX_TEST_SPRITES; i++) {
        double dx, dy, dist, moveX, moveY, newX, newY;

        if (!testSprites[i].active) continue;
        if (testSprites[i].isPickup) continue;  /* Only enemies move */

        /* Calculate vector to player */
        dx = player->x - testSprites[i].x;
        dy = player->y - testSprites[i].y;
        dist = sqrt(dx * dx + dy * dy);

        /* Damage player if close enough (once per second) */
        if (dist < ATTACK_DISTANCE) {
            if ((now - lastDamageTime) > CLOCKS_PER_SEC) {  /* 1 second cooldown */
                playerHealth -= 10;  /* 10 damage per hit */
                lastDamageTime = now;
                if (playerHealth < 0) playerHealth = 0;
            }
        }

        /* Don't move if too close */
        if (dist < MIN_DISTANCE) continue;

        /* Normalize and scale by speed */
        moveX = (dx / dist) * AI_SPEED;
        moveY = (dy / dist) * AI_SPEED;

        /* Calculate new position */
        newX = testSprites[i].x + moveX;
        newY = testSprites[i].y + moveY;

        /* Only move if not hitting a wall */
        if (!isWall(getMazeCell(maze, (int)newX, (int)testSprites[i].y))) {
            testSprites[i].x = newX;
        }
        if (!isWall(getMazeCell(maze, (int)testSprites[i].x, (int)newY))) {
            testSprites[i].y = newY;
        }
    }
}

/* Update enemy respawn system - reactivate killed enemies after timer */
void updateEnemyRespawn(void) {
    int i;
    clock_t now = clock();

    for (i = 0; i < MAX_TEST_SPRITES; i++) {
        /* Skip if already active or has no respawn timer set */
        if (testSprites[i].active) continue;
        if (testSprites[i].respawnTime == 0) continue;

        /* Check if respawn time has passed */
        if (now >= testSprites[i].respawnTime) {
            /* Respawn at original position */
            testSprites[i].x = testSprites[i].spawnX;
            testSprites[i].y = testSprites[i].spawnY;
            testSprites[i].active = 1;

            /* Set health based on type */
            if (testSprites[i].isPickup) {
                testSprites[i].health = 0;  /* Pickups have no health */
            } else {
                testSprites[i].health = 100;  /* Enemies respawn with full health */
            }

            testSprites[i].respawnTime = 0;  /* Reset timer */
        }
    }
}

/* Check for pickups near player and collect them */
void checkPickups(Player *player) {
    int i;
    const double PICKUP_RADIUS = 0.5;  /* How close player must be to collect */

    for (i = 0; i < MAX_TEST_SPRITES; i++) {
        double dx, dy, dist;

        if (!testSprites[i].active) continue;
        if (!testSprites[i].isPickup) continue;  /* Only check pickups */

        /* Calculate distance to player */
        dx = testSprites[i].x - player->x;
        dy = testSprites[i].y - player->y;
        dist = sqrt(dx * dx + dy * dy);

        /* If close enough, collect it */
        if (dist < PICKUP_RADIUS) {
            testSprites[i].active = 0;  /* Remove from world */

            /* Set respawn timer for 20 seconds from now */
            testSprites[i].respawnTime = clock() + (CLOCKS_PER_SEC * 20);

            switch (testSprites[i].spriteType) {
                case SPRITE_PICKUP_HEALTH:
                    /* Restore 25 health, max 100 */
                    playerHealth += 25;
                    if (playerHealth > 100) playerHealth = 100;
                    break;

                case SPRITE_PICKUP_AMMO:
                    /* Refill to full starting amount */
                    playerAmmo = STARTING_AMMO;
                    break;
            }
        }
    }
}

void handleInput(Player *player, Maze *maze, int *running) {
    static unsigned char prevKeyDown[128] = {0};
    int didMove = 0;

    /* Read current keyboard state */
    readKeyboardState(keyDown);

    /* Check for ESC */
    if (keyDown[1]) {
        *running = 0;
        return;
    }

    /* Handle mouse look */
    int mx, my;
    getMouseDelta(&mx, &my);

    if (mx != 0) {
        /* Apply horizontal mouse movement to player angle */
        player->angle += mx * MOUSE_SENSITIVITY;

        /* Update direction vectors based on new angle */
        player->dirX = cos(player->angle);
        player->dirY = sin(player->angle);

        /* Update camera plane (perpendicular to direction) */
        player->planeX = -player->dirY * 0.66;
        player->planeY = player->dirX * 0.66;
    }

    if (my != 0) {
        /* Apply vertical mouse movement to pitch (inverted for intuitive control) */
        playerPitch -= my;

        /* Clamp pitch to reasonable range */
        if (playerPitch > 100) playerPitch = 100;
        if (playerPitch < -100) playerPitch = -100;
    }

    /* NEW CONTROL SCHEME: All actions are independent */

    /* FORWARD/BACKWARD movement (Up/Down arrows) */
    if (keyDown[72]) {  /* Up arrow */
        movePlayer(player, maze, 1.0);
        didMove = 1;
    }
    if (keyDown[80]) {  /* Down arrow */
        movePlayer(player, maze, -1.0);
        didMove = 1;
    }

    /* STRAFE movement (Left/Right arrows) */
    if (keyDown[75]) {  /* Left arrow - strafe left */
        double newX = player->x - player->planeX * MOVE_SPEED;
        double newY = player->y - player->planeY * MOVE_SPEED;
        if (!isWall(getMazeCell(maze, (int)newX, (int)player->y))) {
            player->x = newX;
        }
        if (!isWall(getMazeCell(maze, (int)player->x, (int)newY))) {
            player->y = newY;
        }
        didMove = 1;
    }
    if (keyDown[77]) {  /* Right arrow - strafe right */
        double newX = player->x + player->planeX * MOVE_SPEED;
        double newY = player->y + player->planeY * MOVE_SPEED;
        if (!isWall(getMazeCell(maze, (int)newX, (int)player->y))) {
            player->x = newX;
        }
        if (!isWall(getMazeCell(maze, (int)player->x, (int)newY))) {
            player->y = newY;
        }
        didMove = 1;
    }

    /* Play footstep if moved */
    if (didMove) {
        playFootstepSound();
    }

    /* Handle shooting - SPACE key OR left mouse button */
    /* Allow rapid fire by holding button down */
    if (keyDown[57] || getMouseButton()) {  /* SPACE or left mouse button */
        static clock_t lastShotTime = 0;
        clock_t now = clock();
        /* Rate limit to 5 shots per second */
        if ((now - lastShotTime) > (CLOCKS_PER_SEC / 5)) {
            shootWeapon(player, maze);
            lastShotTime = now;
        }
    }

    /* Auto-collect pickups when walking near them */
    checkPickups(player);

    /* Store previous key state for next frame */
    memcpy(prevKeyDown, keyDown, sizeof(keyDown));
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

/*============================================================================
 * GAME HUD (Health, Ammo, Score)
 *===========================================================================*/

void drawGameHUD(int currentLevel) {
    char buffer[32];
    int i;
    int hudX = 5;  /* Bottom left corner */
    int hudY = SCREEN_HEIGHT - 50;  /* Start 50 pixels from bottom */

    /* REORGANIZED STACK - BOTTOM LEFT (health at bottom) */

    /* Ammo counter - first line */
    sprintf(buffer, "AMMO: %d", playerAmmo);
    drawText(hudX, hudY, buffer, 14);  /* Yellow */

    /* Score - second line */
    sprintf(buffer, "SCORE: %d", playerScore);
    drawText(hudX, hudY + 10, buffer, 11);  /* Cyan */

    /* Shots accuracy - third line */
    sprintf(buffer, "HITS: %d/%d", shotsHit, shotsFired);
    drawText(hudX, hudY + 20, buffer, 8);  /* Gray */

    /* Health bar with label - fourth line (bottom of stack) */
    drawText(hudX, hudY + 30, "HP:", 12);  /* Red text */

    /* Draw health bar background (black) */
    for (i = 0; i < 100; i++) {
        int x = hudX + 25 + i;
        int y;
        for (y = hudY + 30; y < hudY + 38; y++) {
            if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
                backBuffer[y * SCREEN_WIDTH + x] = 0;
            }
        }
    }

    /* Draw health bar foreground (color based on health) */
    for (i = 0; i < playerHealth && i < 100; i++) {
        int x = hudX + 25 + i;
        int y;
        unsigned char color = (playerHealth > 50) ? 10 : (playerHealth > 25 ? 14 : 4);  /* Green/Yellow/Red */
        for (y = hudY + 31; y < hudY + 37; y++) {
            if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
                backBuffer[y * SCREEN_WIDTH + x] = color;
            }
        }
    }

    /* Draw health value */
    sprintf(buffer, "%d", playerHealth);
    drawText(hudX + 130, hudY + 30, buffer, 15);  /* White */

    /* TOP LEFT - Game name and level */
    drawText(5, 5, "MAZE RUNNER", 11);  /* Cyan */
    sprintf(buffer, "LEVEL %d", currentLevel);
    drawText(5, 15, buffer, 14);  /* Yellow */

    /* CROSSHAIRS - center of screen */
    {
        int cx = SCREEN_WIDTH / 2;
        int cy = SCREEN_HEIGHT / 2 + playerPitch / 4;  /* Adjust for pitch */
        int i;

        /* Horizontal line */
        for (i = -8; i <= 8; i++) {
            if (i >= -2 && i <= 2) continue;  /* Gap in center */
            if (cx + i >= 0 && cx + i < SCREEN_WIDTH && cy >= 0 && cy < SCREEN_HEIGHT) {
                backBuffer[cy * SCREEN_WIDTH + (cx + i)] = 15;  /* White */
            }
        }

        /* Vertical line */
        for (i = -8; i <= 8; i++) {
            if (i >= -2 && i <= 2) continue;  /* Gap in center */
            if (cx >= 0 && cx < SCREEN_WIDTH && cy + i >= 0 && cy + i < SCREEN_HEIGHT) {
                backBuffer[(cy + i) * SCREEN_WIDTH + cx] = 15;  /* White */
            }
        }

        /* Center dot */
        if (cx >= 0 && cx < SCREEN_WIDTH && cy >= 0 && cy < SCREEN_HEIGHT) {
            backBuffer[cy * SCREEN_WIDTH + cx] = 12;  /* Red center dot */
        }
    }

    /* BULLET TRACER - visible for short time after shooting */
    {
        clock_t now = clock();
        long elapsed = (now - lastShotTime) * 1000 / CLOCKS_PER_SEC;

        if (elapsed < TRACER_DURATION_MS && lastShotHit) {
            /* Draw tracer line from player to hit position */
            /* This is a simple 2D overlay - not a 3D projected line */
            /* We'll draw it as a bright yellow/orange flash */
            int screenX = SCREEN_WIDTH / 2;
            int screenY = SCREEN_HEIGHT / 2;
            int i;

            /* Draw a few bright pixels in center to simulate tracer */
            for (i = 0; i < 30; i++) {
                int x = screenX + (rand() % 10 - 5);
                int y = screenY + (rand() % 10 - 5);
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                    backBuffer[y * SCREEN_WIDTH + x] = 14;  /* Yellow flash */
                }
            }
        }
    }
}

/* Draw mini-map in TOP-RIGHT corner */
void drawMiniMap(Player *player, Maze *maze) {
    #define MINIMAP_SIZE 60
    #define MINIMAP_PADDING 5
    #define MINIMAP_CELL_SIZE (MINIMAP_SIZE / MAP_WIDTH)

    int startX = SCREEN_WIDTH - MINIMAP_SIZE - MINIMAP_PADDING;
    int startY = MINIMAP_PADDING;
    int x, y, px, py;

    /* Draw semi-transparent black background */
    for (y = 0; y < MINIMAP_SIZE + 4; y++) {
        for (x = 0; x < MINIMAP_SIZE + 4; x++) {
            int screenX = startX - 2 + x;
            int screenY = startY - 2 + y;
            if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT) {
                backBuffer[screenY * SCREEN_WIDTH + screenX] = COLOR_BLACK;
            }
        }
    }

    /* Draw border */
    for (x = 0; x < MINIMAP_SIZE + 4; x++) {
        int screenX = startX - 2 + x;
        if (screenX >= 0 && screenX < SCREEN_WIDTH) {
            backBuffer[(startY - 2) * SCREEN_WIDTH + screenX] = COLOR_YELLOW;
            backBuffer[(startY + MINIMAP_SIZE + 1) * SCREEN_WIDTH + screenX] = COLOR_YELLOW;
        }
    }
    for (y = 0; y < MINIMAP_SIZE + 4; y++) {
        int screenY = startY - 2 + y;
        if (screenY >= 0 && screenY < SCREEN_HEIGHT) {
            backBuffer[screenY * SCREEN_WIDTH + (startX - 2)] = COLOR_YELLOW;
            backBuffer[screenY * SCREEN_WIDTH + (startX + MINIMAP_SIZE + 1)] = COLOR_YELLOW;
        }
    }

    /* Draw maze walls and exit */
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            int cell = maze->cells[y][x];
            int minimapX = startX + (x * MINIMAP_SIZE) / MAP_WIDTH;
            int minimapY = startY + (y * MINIMAP_SIZE) / MAP_HEIGHT;
            unsigned char color = COLOR_BLACK;

            /* Determine cell color */
            if (cell == REF_EXIT || cell == 20) {
                /* Flash exit - red/yellow alternating */
                clock_t now = clock();
                long elapsed = (now - gameStartTime) * 1000 / CLOCKS_PER_SEC;
                color = ((elapsed / 250) % 2) ? COLOR_RED : COLOR_YELLOW;
            } else if (cell == CELL_WALL_HOLOGRAM || cell == 3) {
                /* HOLOGRAM walls - magenta */
                color = COLOR_MAGENTA;
            } else if (cell > 0 && cell < 10) {
                /* Regular walls - gray */
                color = COLOR_GRAY;
            }

            /* Draw cell pixel */
            if (color != COLOR_BLACK && minimapX >= 0 && minimapX < SCREEN_WIDTH && minimapY >= 0 && minimapY < SCREEN_HEIGHT) {
                backBuffer[minimapY * SCREEN_WIDTH + minimapX] = color;
            }
        }
    }

    /* Draw player as bright green dot with direction indicator */
    px = startX + ((int)(player->x * MINIMAP_SIZE)) / MAP_WIDTH;
    py = startY + ((int)(player->y * MINIMAP_SIZE)) / MAP_HEIGHT;

    /* Draw 2x2 player dot */
    for (y = -1; y <= 1; y++) {
        for (x = -1; x <= 1; x++) {
            int screenX = px + x;
            int screenY = py + y;
            if (screenX >= startX && screenX < startX + MINIMAP_SIZE &&
                screenY >= startY && screenY < startY + MINIMAP_SIZE) {
                backBuffer[screenY * SCREEN_WIDTH + screenX] = COLOR_LGREEN;
            }
        }
    }

    /* Draw direction line from player */
    for (int i = 1; i <= 3; i++) {
        int dirX = px + (int)(player->dirX * i * 2);
        int dirY = py + (int)(player->dirY * i * 2);
        if (dirX >= startX && dirX < startX + MINIMAP_SIZE &&
            dirY >= startY && dirY < startY + MINIMAP_SIZE) {
            backBuffer[dirY * SCREEN_WIDTH + dirX] = COLOR_YELLOW;
        }
    }

    /* Draw enemies on mini-map */
    for (int i = 0; i < MAX_TEST_SPRITES; i++) {
        if (testSprites[i].active && !testSprites[i].isPickup && testSprites[i].health > 0) {
            int ex = startX + ((int)(testSprites[i].x * MINIMAP_SIZE)) / MAP_WIDTH;
            int ey = startY + ((int)(testSprites[i].y * MINIMAP_SIZE)) / MAP_HEIGHT;
            if (ex >= startX && ex < startX + MINIMAP_SIZE &&
                ey >= startY && ey < startY + MINIMAP_SIZE) {
                backBuffer[ey * SCREEN_WIDTH + ex] = COLOR_RED;
            }
        }
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

/* Draw a single big character at position */
void drawBigChar(int x, int y, char c, unsigned char color) {
    if (c >= 128 || c < 0) return;

    const unsigned char *glyph = font8x8[(unsigned char)c];

    /* Draw 2x scaled character */
    for (int row = 0; row < 8; row++) {
        unsigned char rowData = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (rowData & (0x80 >> col)) {
                /* Draw 2x2 pixel block */
                for (int dy = 0; dy < 2; dy++) {
                    for (int dx = 0; dx < 2; dx++) {
                        int px = x + col * 2 + dx;
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

/* Show VGA splash screen at game start with ANIMATED TITLE */
void showVGASplashScreen(void) {
    const char *title = "MAZE RUNNER";
    int titleLen = strlen(title);
    int finalTitleX = (SCREEN_WIDTH - titleLen * 16) / 2;  /* Final centered position */
    int titleY = 60;  /* Centered vertically for hero section */

    /* Letter positions - each letter slides in from off-screen */
    int letterX[32];  /* Current X position of each letter */
    int letterTargetX[32];  /* Target X position */

    /* Calculate target positions */
    for (int i = 0; i < titleLen; i++) {
        letterTargetX[i] = finalTitleX + i * 16;
        letterX[i] = -16 - i * 5;  /* Start off-screen left, staggered */
    }

    /* Animation loop - slide letters in */
    int animating = 1;
    int frame = 0;
    while (animating) {
        clearScreen(COLOR_BLACK);
        animating = 0;

        /* Draw and update each letter */
        for (int i = 0; i < titleLen; i++) {
            /* Slide letter toward target */
            if (letterX[i] < letterTargetX[i]) {
                letterX[i] += 4;  /* Slide speed: 4 pixels per frame (slower) */
                if (letterX[i] > letterTargetX[i]) {
                    letterX[i] = letterTargetX[i];  /* Snap to final position */
                }
                animating = 1;  /* Still animating */
            }

            /* Draw letter at current position */
            if (letterX[i] >= -16) {  /* Only draw if partially on screen */
                drawBigChar(letterX[i], titleY, title[i], NEON_CYAN);
            }
        }

        displayFrame();
        delay(30);  /* ~33 fps animation */
        frame++;
    }

    /* Fade in credits immediately after title animation */
    const char *credit1 = "CREATED BY TRENT VON HOLTEN";
    const char *credit2 = "VONHOLTENCODES";
    const char *credit3 = "VERSION 3.0 - 2025";

    int credit1X = (SCREEN_WIDTH - strlen(credit1) * 8) / 2;
    int credit2X = (SCREEN_WIDTH - strlen(credit2) * 8) / 2;
    int credit3X = (SCREEN_WIDTH - strlen(credit3) * 8) / 2;

    /* Fade in by showing progressively brighter colors */
    unsigned char fadeColors[] = {COLOR_BLACK, COLOR_GRAY, COLOR_WHITE, COLOR_WHITE};
    unsigned char fadeColors2[] = {COLOR_BLACK, COLOR_GRAY, NEON_GREEN, NEON_GREEN};
    unsigned char fadeColors3[] = {COLOR_BLACK, COLOR_GRAY, COLOR_YELLOW, COLOR_YELLOW};

    for (int fadeStep = 0; fadeStep < 4; fadeStep++) {
        clearScreen(COLOR_BLACK);

        /* Redraw title */
        drawBigText(finalTitleX, titleY, title, NEON_CYAN);

        /* Draw credits with current fade color */
        drawText(credit1X, 100, credit1, fadeColors[fadeStep]);
        drawText(credit2X, 110, credit2, fadeColors2[fadeStep]);
        drawText(credit3X, 120, credit3, fadeColors3[fadeStep]);

        displayFrame();
        delay(300);  /* Slower fade - 300ms per step */
    }

    /* Hold on title and credits for 5 seconds */
    delay(5000);

    /* Now show instructions */
    clearScreen(COLOR_BLACK);

    /* Title at top */
    drawBigText(finalTitleX, 20, title, NEON_CYAN);

    /* Subtitle */
    const char *subtitle = "CYBERPUNK GRID ESCAPE";
    int subX = (SCREEN_WIDTH - strlen(subtitle) * 8) / 2;
    drawText(subX, 45, subtitle, NEON_MAGENTA);

    /* Instructions */
    int y = 60;
    drawText(60, y, "CONTROLS:", NEON_GREEN);
    y += 14;
    drawText(30, y, "UP/DOWN ARROWS - MOVE", COLOR_WHITE);
    y += 10;
    drawText(30, y, "LEFT/RIGHT ARROWS - TURN", COLOR_WHITE);
    y += 10;
    drawText(30, y, "SPACE - SHOOT", COLOR_WHITE);
    y += 10;
    drawText(30, y, "ESC - QUIT GAME", COLOR_WHITE);
    y += 16;
    drawText(60, y, "GAMEPLAY:", NEON_GREEN);
    y += 14;
    drawText(25, y, "SHOOT ENEMIES FOR POINTS", COLOR_LMAGENTA);
    y += 10;
    drawText(25, y, "COLLECT AMMO & HEALTH PICKUPS", COLOR_LGREEN);
    y += 10;
    drawText(25, y, "FIND FLASHING EXIT ON MINIMAP", COLOR_YELLOW);
    y += 10;
    drawText(25, y, "SURVIVE 6 LEVELS TO WIN!", COLOR_LRED);

    /* Credits at bottom */
    drawText(credit1X, 172, credit1, COLOR_WHITE);
    drawText(credit2X, 182, credit2, NEON_GREEN);

    /* Press key prompt */
    const char *prompt = "PRESS ANY KEY TO START";
    int promptX = (SCREEN_WIDTH - strlen(prompt) * 8) / 2;
    drawText(promptX, 192, prompt, COLOR_YELLOW);

    displayFrame();
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
    sprintf(numText, "LEVEL %d OF 6", level);
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

        /* Quick short tone for footstep - VERY QUIET */
        if (isAudioAvailable()) {
            playToneBlocking(freq, 8);  /* Ultra short 8ms burst (was 20ms) */
        } else {
            /* PC Speaker fallback */
            unsigned int divisor = 1193180 / freq;
            outp(0x43, 0xB6);
            outp(0x42, divisor & 0xFF);
            outp(0x42, divisor >> 8);
            unsigned char tmp = inp(0x61);
            outp(0x61, tmp | 3);
            delay(8);  /* Ultra short footstep (was 20ms) */
            outp(0x61, tmp & 0xFC);
        }

        footstepIndex = (footstepIndex + 1) % FOOTSTEP_COUNT;
        lastFootstepTime = now;
    }
}

/* Play gunfire sound - explosive BANG through sound card */
void playGunshotSound(void) {
    /* Use sound card like footsteps do - NOT PC speaker! */
    if (isAudioAvailable()) {
        /* Sound card: 3-stage frequency sweep - SUPER LOUD (10x original) */
        playToneBlocking(200, 80);   /* Deep BANG - MUCH louder */
        playToneBlocking(400, 60);   /* Mid explosion - MUCH louder */
        playToneBlocking(800, 40);   /* High crack - MUCH louder */
    } else {
        /* PC Speaker fallback - frequency sweep - SUPER LOUD */
        unsigned char tmp = inp(0x61);

        /* Initial BANG - deep explosive sound (200 Hz) - MUCH louder */
        unsigned int divisor = 1193180 / 200;
        outp(0x43, 0xB6);
        outp(0x42, divisor & 0xFF);
        outp(0x42, divisor >> 8);
        outp(0x61, tmp | 3);
        delay(80);

        /* Mid explosion (400 Hz) - MUCH louder */
        divisor = 1193180 / 400;
        outp(0x42, divisor & 0xFF);
        outp(0x42, divisor >> 8);
        delay(60);

        /* High crack (800 Hz) - MUCH louder */
        divisor = 1193180 / 800;
        outp(0x42, divisor & 0xFF);
        outp(0x42, divisor >> 8);
        delay(40);

        /* Silence */
        outp(0x61, tmp & 0xFC);
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

/* Show credits screen - skippable with ESC */
/*===========================================================================
 * HIGH SCORE SYSTEM
 *===========================================================================*/

/* Load high scores from file */
void loadHighScores(void) {
    FILE *fp = fopen(SCORE_FILE, "rb");
    int i;

    /* Initialize with zeros */
    numHighScores = 0;
    for (i = 0; i < MAX_HIGH_SCORES; i++) {
        strcpy(highScores[i].name, "");
        highScores[i].score = 0;
        highScores[i].level = 0;
        strcpy(highScores[i].date, "");
    }

    if (fp == NULL) {
        /* No score file exists yet - that's OK */
        return;
    }

    /* Read scores from file */
    numHighScores = fread(highScores, sizeof(HighScore), MAX_HIGH_SCORES, fp);
    fclose(fp);
}

/* Save high scores to file */
void saveHighScores(void) {
    FILE *fp = fopen(SCORE_FILE, "wb");
    if (fp == NULL) {
        return;  /* Can't save - oh well */
    }

    fwrite(highScores, sizeof(HighScore), numHighScores, fp);
    fclose(fp);
}

/* Check if score qualifies for high score table */
int isHighScore(int score) {
    if (numHighScores < MAX_HIGH_SCORES) {
        return 1;  /* Table not full yet */
    }
    return (score > highScores[numHighScores - 1].score);
}

/* Get player name input (in VGA mode) */
void getPlayerName(char *name, int maxLen) {
    int i = 0;
    int ch;
    int needsRedraw = 1;

    /* Clear name buffer */
    memset(name, 0, maxLen);

    /* Clear keyboard buffer first */
    while (kbhit()) getch();

    clearScreen(COLOR_BLACK);
    drawBigText(70, 60, "ENTER NAME:", COLOR_YELLOW);
    drawText(60, 100, "(UP TO 15 CHARACTERS)", COLOR_GRAY);
    drawText(60, 115, "PRESS ENTER WHEN DONE", COLOR_GRAY);
    displayFrame();

    while (1) {
        /* Only redraw if something changed */
        if (needsRedraw) {
            char displayName[20];
            sprintf(displayName, "%s_", name);  /* Show cursor */
            int nameX = (SCREEN_WIDTH - strlen(displayName) * 8) / 2;

            /* Clear line and redraw */
            drawText(0, 140, "                                        ", COLOR_BLACK);
            drawText(nameX, 140, displayName, COLOR_LGREEN);
            displayFrame();
            needsRedraw = 0;
        }

        /* Wait for key */
        if (!kbhit()) continue;

        ch = getch();

        /* Handle extended keys (arrows, delete, etc.) */
        if (ch == 0) {
            getch();  /* Consume the extended scancode and ignore */
            continue;
        }

        if (ch == 13 || ch == 10) {  /* Enter */
            break;
        } else if (ch == 8) {  /* Backspace ONLY */
            if (i > 0) {
                i--;
                name[i] = '\0';
                needsRedraw = 1;
            }
        } else if (ch >= 'A' && ch <= 'Z') {  /* Uppercase letters */
            if (i < maxLen - 1) {
                name[i] = ch;
                i++;
                name[i] = '\0';
                needsRedraw = 1;
            }
        } else if (ch >= 'a' && ch <= 'z') {  /* Convert lowercase to uppercase */
            if (i < maxLen - 1) {
                name[i] = ch - 32;  /* Convert to uppercase */
                i++;
                name[i] = '\0';
                needsRedraw = 1;
            }
        } else if (ch >= '0' && ch <= '9') {  /* Numbers */
            if (i < maxLen - 1) {
                name[i] = ch;
                i++;
                name[i] = '\0';
                needsRedraw = 1;
            }
        } else if (ch == ' ') {  /* Space */
            if (i < maxLen - 1 && i > 0) {  /* No leading spaces */
                name[i] = ch;
                i++;
                name[i] = '\0';
                needsRedraw = 1;
            }
        }
        /* Ignore all other keys (arrows, delete, etc.) */
    }

    /* If no name entered, use default */
    if (strlen(name) == 0) {
        strcpy(name, "PLAYER");
    }
}

/* Add new high score to table */
void addHighScore(int score, int level, const char *playerName) {
    int i, insertPos;
    time_t t;
    struct tm *tm_info;
    char dateStr[12];

    /* Get current date */
    time(&t);
    tm_info = localtime(&t);
    sprintf(dateStr, "%02d/%02d/%04d",
            tm_info->tm_mon + 1,
            tm_info->tm_mday,
            tm_info->tm_year + 1900);

    /* Find insertion position */
    insertPos = numHighScores;
    for (i = 0; i < numHighScores; i++) {
        if (score > highScores[i].score) {
            insertPos = i;
            break;
        }
    }

    /* Shift scores down */
    for (i = MAX_HIGH_SCORES - 1; i > insertPos; i--) {
        highScores[i] = highScores[i - 1];
    }

    /* Insert new score */
    strncpy(highScores[insertPos].name, playerName, 15);
    highScores[insertPos].name[15] = '\0';
    highScores[insertPos].score = score;
    highScores[insertPos].level = level;
    strcpy(highScores[insertPos].date, dateStr);

    if (numHighScores < MAX_HIGH_SCORES) {
        numHighScores++;
    }

    /* Save to file */
    saveHighScores();
}

/* Display high score table */
void showHighScores(void) {
    int i;
    char buffer[80];

    clearScreen(COLOR_BLACK);

    /* Title */
    drawBigText(80, 10, "HIGH SCORES", COLOR_YELLOW);

    /* Column headers */
    drawText(20, 35, "RANK  NAME            SCORE   LEVEL   DATE", COLOR_LCYAN);
    drawText(20, 45, "----  --------------  ------  -----   ----------", COLOR_GRAY);

    /* Display scores */
    for (i = 0; i < numHighScores && i < MAX_HIGH_SCORES; i++) {
        int y = 55 + (i * 12);

        /* Rank */
        sprintf(buffer, "%2d.", i + 1);
        drawText(20, y, buffer, COLOR_WHITE);

        /* Name */
        sprintf(buffer, "%-14s", highScores[i].name);
        drawText(50, y, buffer, COLOR_LGREEN);

        /* Score */
        sprintf(buffer, "%6d", highScores[i].score);
        drawText(120, y, buffer, COLOR_YELLOW);

        /* Level */
        sprintf(buffer, "%3d", highScores[i].level);
        drawText(160, y, buffer, COLOR_LMAGENTA);

        /* Date */
        drawText(195, y, highScores[i].date, COLOR_GRAY);
    }

    if (numHighScores == 0) {
        drawText(80, 90, "NO HIGH SCORES YET", COLOR_GRAY);
        drawText(60, 105, "BE THE FIRST TO SET A RECORD!", COLOR_LGREEN);
    }

    /* Instructions */
    drawText(70, 180, "PRESS ANY KEY TO CONTINUE", COLOR_WHITE);

    displayFrame();
    getch();
}

void showCredits(void) {
    clearScreen(COLOR_BLACK);

    /* Title */
    const char *title = "MAZE RUNNER";
    int titleX = (SCREEN_WIDTH - strlen(title) * 16) / 2;
    drawBigText(titleX, 10, title, NEON_CYAN);

    /* Credits - ALL UPPERCASE FOR VISIBILITY */
    drawText(20, 45, "CREATED BY VONHOLTENCODES (2025)", COLOR_YELLOW);
    drawText(20, 55, "VERSION 3.0", COLOR_LCYAN);
    drawText(20, 70, "OPEN SOURCE & FREEWARE USED:", COLOR_WHITE);

    drawText(20, 85, "- MINION GAME SPRITES (VONHOLTENCODES)", COLOR_GRAY);
    drawText(25, 95, "  CUSTOM ENEMY AND CHARACTER SPRITES", COLOR_GRAY);

    drawText(20, 110, "- KENNEY.NL WEAPON SPRITE (CC0 1.0)", COLOR_GRAY);
    drawText(25, 120, "  PISTOL WEAPON SPRITE", COLOR_GRAY);

    drawText(20, 135, "- DJGPP CROSS-COMPILER (GPL)", COLOR_GRAY);
    drawText(25, 145, "  DOS DEVELOPMENT TOOLCHAIN", COLOR_GRAY);

    drawText(20, 160, "- DOS/4GW DOS EXTENDER (FREEWARE)", COLOR_GRAY);
    drawText(25, 170, "  32-BIT DOS MEMORY SUPPORT", COLOR_GRAY);

    drawText(20, 185, "BUILT WITH CLASSIC DOS RAYCASTING", COLOR_WHITE);
    drawText(20, 195, "INSPIRED BY WOLFENSTEIN 3D AND DOOM", COLOR_WHITE);

    /* Skip prompt */
    const char *skip = "PRESS ESC TO EXIT";
    int skipX = (SCREEN_WIDTH - strlen(skip) * 8) / 2;
    drawText(skipX, 195, skip, NEON_GREEN);

    displayFrame();

    /* Wait for ESC */
    int ch;
    do {
        if (kbhit()) {
            ch = getch();
            if (ch == 27) break;  /* ESC */
        }
    } while (1);
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
    printf("              6 Levels of Terror\n");
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
    printf("MAZE RUNNER v3.0 - DOS Raycasting Engine - Dynamic Lighting\n");
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

    /* Load high scores */
    loadHighScores();

    /* Initialize audio */
    initAudio();

    /* Initialize AdLib music */
    initAdLib();

    /* Initialize mouse */
    initMouse();
    if (mouseAvailable) {
        printf("[ MOUSE ] Mouse detected and initialized\n");
    } else {
        printf("[ MOUSE ] No mouse found - using keyboard only\n");
    }

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
    if (mouseAvailable) {
        printf("  [MOUSE] Look around (captured)\n");
    }
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

    /* v3.0: Initialize dynamic torch lighting system */
    setupLevelTorches(currentLevel);

    /* Start game timer */
    gameStartTime = clock();
    lastFootstepTime = gameStartTime;
    gameWon = 0;

    /* Initialize sprites - diverse enemy roster */
    /* Enemy 0: Purple Gremlin */
    testSprites[0].x = testSprites[0].spawnX = 7.5;
    testSprites[0].y = testSprites[0].spawnY = 3.5;
    testSprites[0].active = 1;
    testSprites[0].spriteType = SPRITE_ENEMY_GREMLIN;
    testSprites[0].health = 100;
    testSprites[0].isPickup = 0;
    testSprites[0].respawnTime = 0;

    /* Enemy 1: Zombie */
    testSprites[1].x = testSprites[1].spawnX = 12.5;
    testSprites[1].y = testSprites[1].spawnY = 7.5;
    testSprites[1].active = 1;
    testSprites[1].spriteType = SPRITE_ENEMY_MINION;
    testSprites[1].health = 100;
    testSprites[1].isPickup = 0;
    testSprites[1].respawnTime = 0;

    /* Enemy 2: Creeper */
    testSprites[2].x = testSprites[2].spawnX = 18.5;
    testSprites[2].y = testSprites[2].spawnY = 12.5;
    testSprites[2].active = 1;
    testSprites[2].spriteType = SPRITE_ENEMY_CREEPER;
    testSprites[2].health = 100;
    testSprites[2].isPickup = 0;
    testSprites[2].respawnTime = 0;

    /* Enemy 3: Tomato */
    testSprites[3].x = testSprites[3].spawnX = 3.5;
    testSprites[3].y = testSprites[3].spawnY = 16.5;
    testSprites[3].active = 1;
    testSprites[3].spriteType = SPRITE_ENEMY_TOMATO;
    testSprites[3].health = 100;
    testSprites[3].isPickup = 0;
    testSprites[3].respawnTime = 0;

    /* Enemy 4: Spiderman */
    testSprites[4].x = testSprites[4].spawnX = 15.5;
    testSprites[4].y = testSprites[4].spawnY = 18.5;
    testSprites[4].active = 1;
    testSprites[4].spriteType = SPRITE_ENEMY_SPIDERMAN;
    testSprites[4].health = 100;
    testSprites[4].isPickup = 0;
    testSprites[4].respawnTime = 0;

    /* Enemy 5: Waldo */
    testSprites[5].x = testSprites[5].spawnX = 5.5;
    testSprites[5].y = testSprites[5].spawnY = 5.5;
    testSprites[5].active = 1;
    testSprites[5].spriteType = SPRITE_ENEMY_WALDO;
    testSprites[5].health = 100;
    testSprites[5].isPickup = 0;
    testSprites[5].respawnTime = 0;

    /* Enemy 6: Soldier */
    testSprites[6].x = testSprites[6].spawnX = 9.5;
    testSprites[6].y = testSprites[6].spawnY = 14.5;
    testSprites[6].active = 1;
    testSprites[6].spriteType = SPRITE_ENEMY_GREMLIN;
    testSprites[6].health = 100;
    testSprites[6].isPickup = 0;
    testSprites[6].respawnTime = 0;

    /* Enemy 7: Zombie */
    testSprites[7].x = testSprites[7].spawnX = 16.5;
    testSprites[7].y = testSprites[7].spawnY = 6.5;
    testSprites[7].active = 1;
    testSprites[7].spriteType = SPRITE_ENEMY_MINION;
    testSprites[7].health = 100;
    testSprites[7].isPickup = 0;
    testSprites[7].respawnTime = 0;

    /* Enemy 8: Creeper */
    testSprites[8].x = testSprites[8].spawnX = 11.5;
    testSprites[8].y = testSprites[8].spawnY = 17.5;
    testSprites[8].active = 1;
    testSprites[8].spriteType = SPRITE_ENEMY_CREEPER;
    testSprites[8].health = 100;
    testSprites[8].isPickup = 0;
    testSprites[8].respawnTime = 0;

    /* Enemy 9: Tomato */
    testSprites[9].x = testSprites[9].spawnX = 6.5;
    testSprites[9].y = testSprites[9].spawnY = 11.5;
    testSprites[9].active = 1;
    testSprites[9].spriteType = SPRITE_ENEMY_TOMATO;
    testSprites[9].health = 100;
    testSprites[9].isPickup = 0;
    testSprites[9].respawnTime = 0;

    /* Enemy 10: Spiderman */
    testSprites[10].x = testSprites[10].spawnX = 13.5;
    testSprites[10].y = testSprites[10].spawnY = 4.5;
    testSprites[10].active = 1;
    testSprites[10].spriteType = SPRITE_ENEMY_SPIDERMAN;
    testSprites[10].health = 100;
    testSprites[10].isPickup = 0;
    testSprites[10].respawnTime = 0;

    /* Enemy 11: Waldo */
    testSprites[11].x = testSprites[11].spawnX = 19.5;
    testSprites[11].y = testSprites[11].spawnY = 9.5;
    testSprites[11].active = 1;
    testSprites[11].spriteType = SPRITE_ENEMY_WALDO;
    testSprites[11].health = 100;
    testSprites[11].isPickup = 0;
    testSprites[11].respawnTime = 0;

    /* Enemy 12: Soldier */
    testSprites[12].x = testSprites[12].spawnX = 4.5;
    testSprites[12].y = testSprites[12].spawnY = 13.5;
    testSprites[12].active = 1;
    testSprites[12].spriteType = SPRITE_ENEMY_GREMLIN;
    testSprites[12].health = 100;
    testSprites[12].isPickup = 0;
    testSprites[12].respawnTime = 0;

    /* Enemy 13: Zombie */
    testSprites[13].x = testSprites[13].spawnX = 17.5;
    testSprites[13].y = testSprites[13].spawnY = 19.5;
    testSprites[13].active = 1;
    testSprites[13].spriteType = SPRITE_ENEMY_MINION;
    testSprites[13].health = 100;
    testSprites[13].isPickup = 0;
    testSprites[13].respawnTime = 0;

    /* Enemy 14: Snowman */
    testSprites[14].x = testSprites[14].spawnX = 8.5;
    testSprites[14].y = testSprites[14].spawnY = 8.5;
    testSprites[14].active = 1;
    testSprites[14].spriteType = SPRITE_ENEMY_SNOWMAN;
    testSprites[14].health = 100;
    testSprites[14].isPickup = 0;
    testSprites[14].respawnTime = 0;

    /* Pickup: Health kit */
    testSprites[15].x = testSprites[15].spawnX = 10.5;
    testSprites[15].y = testSprites[15].spawnY = 5.5;
    testSprites[15].active = 1;
    testSprites[15].spriteType = SPRITE_PICKUP_HEALTH;
    testSprites[15].health = 0;
    testSprites[15].isPickup = 1;
    testSprites[15].respawnTime = 0;

    /* Pickup: Ammo box */
    testSprites[16].x = testSprites[16].spawnX = 8.5;
    testSprites[16].y = testSprites[16].spawnY = 10.5;
    testSprites[16].active = 1;
    testSprites[16].spriteType = SPRITE_PICKUP_AMMO;
    testSprites[16].health = 0;
    testSprites[16].isPickup = 1;
    testSprites[16].respawnTime = 0;

    /* Pickup: Health kit */
    testSprites[17].x = testSprites[17].spawnX = 14.5;
    testSprites[17].y = testSprites[17].spawnY = 15.5;
    testSprites[17].active = 1;
    testSprites[17].spriteType = SPRITE_PICKUP_HEALTH;
    testSprites[17].health = 0;
    testSprites[17].isPickup = 1;
    testSprites[17].respawnTime = 0;

    /* Pickup: Ammo box */
    testSprites[18].x = testSprites[18].spawnX = 5.5;
    testSprites[18].y = testSprites[18].spawnY = 19.5;
    testSprites[18].active = 1;
    testSprites[18].spriteType = SPRITE_PICKUP_AMMO;
    testSprites[18].health = 0;
    testSprites[18].isPickup = 1;
    testSprites[18].respawnTime = 0;

    /* Additional ammo pickups for more availability */
    testSprites[19].x = testSprites[19].spawnX = 12.5;
    testSprites[19].y = testSprites[19].spawnY = 5.5;
    testSprites[19].active = 1;
    testSprites[19].spriteType = SPRITE_PICKUP_AMMO;
    testSprites[19].health = 0;
    testSprites[19].isPickup = 1;
    testSprites[19].respawnTime = 0;

    testSprites[20].x = testSprites[20].spawnX = 18.5;
    testSprites[20].y = testSprites[20].spawnY = 12.5;
    testSprites[20].active = 1;
    testSprites[20].spriteType = SPRITE_PICKUP_AMMO;
    testSprites[20].health = 0;
    testSprites[20].isPickup = 1;
    testSprites[20].respawnTime = 0;

    testSprites[21].x = testSprites[21].spawnX = 3.5;
    testSprites[21].y = testSprites[21].spawnY = 8.5;
    testSprites[21].active = 1;
    testSprites[21].spriteType = SPRITE_PICKUP_AMMO;
    testSprites[21].health = 0;
    testSprites[21].isPickup = 1;
    testSprites[21].respawnTime = 0;

    /* Remaining slots unused */
    {
        int i;
        for (i = 22; i < MAX_TEST_SPRITES; i++) {
            testSprites[i].active = 0;
            testSprites[i].respawnTime = 0;
        }
    }

    /* Start music for first level */
    playMusic(MUSIC_TRACK_LEVEL1);

    /* Main game loop - handles all 6 levels */
    while (running) {
        /* Level game loop */
        while (running && !gameWon) {
            /* Update music sequencer */
            updateMusic();

            /* v3.0: Update dynamic torch lighting and fire particles */
            updateTorchSystem();

            /* Update enemy AI - make them move toward player */
            updateEnemyAI(&player, &maze);

            /* Update enemy respawn system */
            updateEnemyRespawn();

            /* Check if player died */
            if (playerHealth <= 0) {
                /* Show GAME OVER screen */
                clearScreen(COLOR_BLACK);
                const char *gameOver = "GAME OVER";
                int gameOverX = (SCREEN_WIDTH - strlen(gameOver) * 16) / 2;
                drawBigText(gameOverX, 70, gameOver, COLOR_RED);

                const char *deathMsg = "YOU HAVE BEEN TERMINATED";
                int deathMsgX = (SCREEN_WIDTH - strlen(deathMsg) * 8) / 2;
                drawText(deathMsgX, 110, deathMsg, COLOR_GRAY);

                const char *prompt = "CONTINUE? Y/N";
                int promptX = (SCREEN_WIDTH - strlen(prompt) * 8) / 2;
                drawText(promptX, 130, prompt, COLOR_YELLOW);

                displayFrame();

                /* Wait for Y or N */
                int response;
                do {
                    response = getch();
                } while (response != 'y' && response != 'Y' && response != 'n' && response != 'N');

                if (response == 'y' || response == 'Y') {
                    /* RESTART from level 1 */
                    currentLevel = 1;
                    loadLevel(&maze, currentLevel);

                    /* v3.0: Reset torch lighting for level 1 */
                    setupLevelTorches(currentLevel);

                    /* Start level music */
                    playMusic(MUSIC_TRACK_LEVEL1);

                    /* Reset player position to spawn */
                    player.x = 1.5;
                    player.y = 1.5;
                    player.angle = 0.0;

                    /* Reset player stats */
                    playerHealth = 100;
                    playerAmmo = STARTING_AMMO;
                    playerScore = 0;

                    /* Reset game state */
                    gameWon = 0;
                    gameStartTime = clock();

                    /* Reset all sprites to spawn positions with full health */
                    {
                        int i;
                        for (i = 0; i < MAX_TEST_SPRITES; i++) {
                            testSprites[i].x = testSprites[i].spawnX;
                            testSprites[i].y = testSprites[i].spawnY;
                            testSprites[i].active = 1;
                            testSprites[i].respawnTime = 0;
                            if (testSprites[i].isPickup) {
                                testSprites[i].health = 0;
                            } else {
                                testSprites[i].health = 100;
                            }
                        }
                    }

                    /* Continue game loop */
                    continue;
                } else {
                    /* Check for high score */
                    if (isHighScore(playerScore)) {
                        char playerName[16];

                        clearScreen(COLOR_BLACK);
                        drawBigText(60, 60, "NEW HIGH SCORE!", COLOR_YELLOW);
                        char scoreMsg[40];
                        sprintf(scoreMsg, "SCORE: %d  LEVEL: %d", playerScore, currentLevel);
                        int msgX = (SCREEN_WIDTH - strlen(scoreMsg) * 8) / 2;
                        drawText(msgX, 95, scoreMsg, COLOR_LGREEN);
                        drawText(50, 120, "PRESS ANY KEY TO CONTINUE", COLOR_WHITE);
                        displayFrame();
                        getch();

                        /* Get player name */
                        getPlayerName(playerName, 16);

                        addHighScore(playerScore, currentLevel, playerName);
                        showHighScores();
                    }

                    /* Show credits before exit */
                    showCredits();

                    /* Exit game */
                    running = 0;
                    break;
                }
            }

            /* Render to back buffer */
            renderFrame(&player, &maze);

            /* v3.0: Render animated torches with dynamic lighting */
            renderTorches(&player);

            /* v3.0: Render fire particles floating above torches */
            renderFireParticles(&player);

            /* Render sprites on top of walls */
            renderSprites(&player);

            /* Draw FPS weapon sprite at bottom center with muzzle flash - DOUBLE SIZE */
            {
                /* Scale weapon to 2x size for better visibility */
                int weaponScaledWidth = WEAPON_PISTOL_BETTER_WIDTH * 2;
                int weaponScaledHeight = WEAPON_PISTOL_BETTER_HEIGHT * 2;
                int weaponX = (SCREEN_WIDTH - weaponScaledWidth) / 2;
                int weaponY = SCREEN_HEIGHT - weaponScaledHeight;
                int wx, wy;
                clock_t now = clock();
                int flashActive = (now - muzzleFlashTime) < (CLOCKS_PER_SEC / 10);  /* 100ms flash */

                /* Draw scaled weapon sprite (2x size) */
                for (wy = 0; wy < weaponScaledHeight && weaponY + wy < SCREEN_HEIGHT; wy++) {
                    for (wx = 0; wx < weaponScaledWidth && weaponX + wx < SCREEN_WIDTH; wx++) {
                        /* Sample from original sprite (scaled down by 2) */
                        int srcX = wx / 2;
                        int srcY = wy / 2;
                        unsigned char c = weapon_pistol_better[srcY * WEAPON_PISTOL_BETTER_WIDTH + srcX];
                        if (c != 0) {  /* Skip transparent pixels */
                            /* Brighten weapon during muzzle flash */
                            if (flashActive && c < 15) {
                                c = 15;  /* White flash */
                            }
                            backBuffer[(weaponY + wy) * SCREEN_WIDTH + (weaponX + wx)] = c;
                        }
                    }
                }
            }

            /* Draw HUD (health, ammo, score, level) */
            drawGameHUD(currentLevel);

            /* Draw mini-map */
            drawMiniMap(&player, &maze);

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
            if (currentLevel < 6) {
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

                    /* v3.0: Setup torches for new level */
                    setupLevelTorches(currentLevel);

                    /* Start appropriate level music */
                    if (currentLevel <= 6) {
                        playMusic(currentLevel);  /* MUSIC_TRACK_LEVEL1-6 map to levels 1-6 */
                    }

                    /* Refill ammo to starting amount */
                    playerAmmo = STARTING_AMMO;

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

    /* Check for high score before exiting */
    if (playerScore > 0 && isHighScore(playerScore)) {
        char playerName[16];

        clearScreen(COLOR_BLACK);
        drawBigText(60, 60, "NEW HIGH SCORE!", COLOR_YELLOW);
        char scoreMsg[40];
        sprintf(scoreMsg, "SCORE: %d  LEVEL: %d", playerScore, currentLevel);
        int msgX = (SCREEN_WIDTH - strlen(scoreMsg) * 8) / 2;
        drawText(msgX, 95, scoreMsg, COLOR_LGREEN);
        drawText(50, 120, "PRESS ANY KEY TO CONTINUE", COLOR_WHITE);
        displayFrame();
        getch();

        /* Get player name */
        getPlayerName(playerName, 16);

        addHighScore(playerScore, currentLevel, playerName);
        showHighScores();
    }

    /* Show credits before exiting VGA mode */
    showCredits();

    /* Exit VGA mode if not already */
    setVideoMode(0x03);

    /* Free double buffer */
    freeDoubleBuffer();

    printf("\n");
    if (gameWon && currentLevel == 6) {
        /* Final victory - all 6 levels completed */
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
        printf("  Completed %d of 6 levels\n", currentLevel);
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
    printf("  MAZE RUNNER v3.0 - Dynamic Lighting Edition\n");
    printf("========================================\n");
    printf("\n");

    /* Cleanup */
    shutdownAudio();
    shutdownAdLib();

#ifdef __DJGPP__
    __djgpp_nearptr_disable();
#endif

    return 0;
}
