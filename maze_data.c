/*
 * MAZE_DATA.C - Multi-Level Maze layouts and reference point definitions
 *
 * CYBERPUNK CORPORATE GRID - 3 LEVELS
 * Level 1: "Sector 7-G: Maximum Security Zone" (Easy)
 * Level 2: "Sector 7-H: Encrypted Archives" (Moderate)
 * Level 3: "Sector 7-I: Central Core" (Extreme)
 *
 * By: VonHoltenCodes (2025)
 */

#include "maze.h"
#include <stdio.h>
#include <string.h>

/*
 * Cyberpunk Maze Map Legend:
 * 0  = Empty cyberspace (walkable)
 * 1  = Neon grid panels (cyan)
 * 2  = Circuit boards (green)
 * 3  = Holographic barriers (magenta)
 * 4  = Data streams (purple)
 * 10 = Jack-in point
 * 20 = Data extraction point
 * 30-32 = Security bot spawn points
 * 40-42 = Item pickups (nanobots, energy, keycard)
 * 50-51 = Security terminals
 * 60 = Security firewall
 */

/* LEVEL 1 - Sector 7-G: Maximum Security Zone (EASY) */
static int level1Layout[MAP_HEIGHT][MAP_WIDTH] = {
    /* 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 0 */
    {  1,10, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 1 */
    {  1, 0, 3, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 3, 0, 1 },  /* 2 - HOLOGRAM panels */
    {  1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 },  /* 3 */
    {  1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 },  /* 4 */
    {  1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,40, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 5 */
    {  1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 6 */
    {  1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 7 */
    {  1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },  /* 8 */
    {  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,30, 0, 0, 0, 1 },  /* 9 */
    {  1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 10 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 11 */
    {  1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 12 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0,60, 0, 1, 0,50, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 13 */
    {  1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },  /* 14 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,42, 0, 1 },  /* 15 */
    {  1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1 },  /* 16 */
    {  1, 0, 0, 0, 0, 0, 1, 0, 0, 0,31, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 17 */
    {  1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1 },  /* 18 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,41, 0, 0, 0, 0, 1 },  /* 19 */
    {  1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 20 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 21 */
    {  1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 3,20, 3, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },  /* 22 - HOLOGRAM walls at exit */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }   /* 23 */
};

/* LEVEL 2 - Sector 7-H: Encrypted Archives (MODERATE) - WIDE OPEN CORRIDORS */
static int level2Layout[MAP_HEIGHT][MAP_WIDTH] = {
    /* 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 0 */
    {  1,10, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 1 - HOLOGRAM panels */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 2 */
    {  1, 0, 0, 3, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 3, 0, 0, 1 },  /* 3 - HOLOGRAM panels */
    {  1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 },  /* 4 */
    {  1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0,30, 1 },  /* 5 */
    {  1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 },  /* 6 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 7 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1 },  /* 8 */
    {  1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,31, 0, 1 },  /* 9 */
    {  1, 0, 0, 1,40, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 10 */
    {  1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0,60, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1 },  /* 11 */
    {  1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 12 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 13 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 },  /* 14 */
    {  1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,41, 0, 0, 1 },  /* 15 */
    {  1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },  /* 16 */
    {  1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1 },  /* 17 */
    {  1, 0, 0, 1, 0,32, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 18 */
    {  1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 19 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1 },  /* 20 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,42, 1 },  /* 21 */
    {  1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,20, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 22 - HOLOGRAM walls at exit */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }   /* 23 */
};

/* LEVEL 3 - Sector 7-I: Central Core (EXTREME) - TIGHT NARROW MAZE */
static int level3Layout[MAP_HEIGHT][MAP_WIDTH] = {
    /* 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 0 */
    {  1,10, 0, 3, 0, 0, 0, 1, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1, 0, 3, 0, 0, 0, 1 },  /* 1 - HOLOGRAM panels */
    {  1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1 },  /* 2 */
    {  1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1 },  /* 3 */
    {  1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 },  /* 4 */
    {  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,30, 1 },  /* 5 */
    {  1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1 },  /* 6 */
    {  1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },  /* 7 */
    {  1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1 },  /* 8 */
    {  1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,31, 1 },  /* 9 */
    {  1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 10 */
    {  1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,40, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },  /* 11 */
    {  1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1 },  /* 12 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,60, 1 },  /* 13 */
    {  1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 14 */
    {  1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },  /* 15 */
    {  1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 },  /* 16 */
    {  1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,32, 0, 0, 1 },  /* 17 */
    {  1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1 },  /* 18 */
    {  1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,41, 1 },  /* 19 */
    {  1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 },  /* 20 */
    {  1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,42, 1 },  /* 21 */
    {  1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 3,20, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 22 - HOLOGRAM walls at exit */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }   /* 23 */
};

/* Reference points - shared structure, loaded per level */
static ReferencePoint referencePoints[10];

/* Current level tracker */
static int currentLevel = 1;

/* Load specific level into maze structure */
void loadLevel(Maze *maze, int level) {
    int x, y;
    int (*sourceLayout)[MAP_WIDTH] = NULL;

    currentLevel = level;
    maze->level = level;
    maze->width = MAP_WIDTH;
    maze->height = MAP_HEIGHT;

    /* Select appropriate layout */
    switch (level) {
        case 1:
            sourceLayout = level1Layout;
            break;
        case 2:
            sourceLayout = level2Layout;
            break;
        case 3:
            sourceLayout = level3Layout;
            break;
        default:
            sourceLayout = level1Layout;
            level = 1;
            break;
    }

    /* Copy the maze layout */
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            maze->cells[y][x] = sourceLayout[y][x];
        }
    }

    /* Set up reference points for this level */
    initReferencePoints();
}

/* Initialize maze with specified level */
void initMaze(Maze *maze, int level) {
    loadLevel(maze, level);
}

/* Get cell value at position */
int getMazeCell(Maze *maze, int x, int y) {
    if (x < 0 || x >= maze->width || y < 0 || y >= maze->height) {
        return CELL_WALL_NEON_GRID;
    }
    return maze->cells[y][x];
}

/* Set cell value at position */
void setMazeCell(Maze *maze, int x, int y, int value) {
    if (x >= 0 && x < maze->width && y >= 0 && y < maze->height) {
        maze->cells[y][x] = value;
    }
}

/* Check if a cell value represents a wall */
int isWall(int cellValue) {
    return (cellValue >= CELL_WALL_NEON_GRID && cellValue <= CELL_WALL_DATA);
}

/* Check if a cell value is a reference point */
int isReferencePoint(int cellValue) {
    return (cellValue >= REF_PLAYER_SPAWN);
}

/* Initialize reference points system */
void initReferencePoints(void) {
    /* Common reference points across all levels */
    referencePoints[0] = (ReferencePoint){ REF_PLAYER_SPAWN, 1,  1,  1, "Neural Jack-In Point" };
    referencePoints[1] = (ReferencePoint){ REF_EXIT,         12, 22, 1, "Data Extraction Node" };
    referencePoints[2] = (ReferencePoint){ REF_ENEMY_1,      0,  0,  1, "Security Bot Alpha" };
    referencePoints[3] = (ReferencePoint){ REF_ENEMY_2,      0,  0,  1, "Security Bot Beta" };
    referencePoints[4] = (ReferencePoint){ REF_ENEMY_3,      0,  0,  1, "Security Bot Gamma" };
    referencePoints[5] = (ReferencePoint){ REF_ITEM_HEALTH,  0,  0,  1, "Med-Pack Nanobots" };
    referencePoints[6] = (ReferencePoint){ REF_ITEM_AMMO,    0,  0,  1, "Energy Cell Depot" };
    referencePoints[7] = (ReferencePoint){ REF_ITEM_KEY,     0,  0,  1, "Access Keycard" };
    referencePoints[8] = (ReferencePoint){ REF_TRIGGER_1,    0,  0,  1, "Security Terminal" };
    referencePoints[9] = (ReferencePoint){ 0, 0, 0, 0, "" }; /* Terminator */

    /* Set level-specific positions */
    switch (currentLevel) {
        case 1:
            referencePoints[2].x = 19; referencePoints[2].y = 9;
            referencePoints[3].x = 10; referencePoints[3].y = 17;
            referencePoints[5].x = 11; referencePoints[5].y = 5;
            referencePoints[6].x = 18; referencePoints[6].y = 19;
            referencePoints[7].x = 21; referencePoints[7].y = 15;
            referencePoints[8].x = 13; referencePoints[8].y = 13;
            break;
        case 2:
            referencePoints[2].x = 21; referencePoints[2].y = 5;
            referencePoints[3].x = 10; referencePoints[3].y = 9;
            referencePoints[4].x = 17; referencePoints[4].y = 17;
            referencePoints[5].x = 13; referencePoints[5].y = 5;
            referencePoints[6].x = 22; referencePoints[6].y = 19;
            referencePoints[7].x = 22; referencePoints[7].y = 21;
            referencePoints[8].x = 19; referencePoints[8].y = 13;
            break;
        case 3:
            referencePoints[2].x = 11; referencePoints[2].y = 5;
            referencePoints[3].x = 10; referencePoints[3].y = 9;
            referencePoints[4].x = 15; referencePoints[4].y = 17;
            referencePoints[5].x = 21; referencePoints[5].y = 19;
            referencePoints[6].x = 22; referencePoints[6].y = 21;
            referencePoints[7].x = 22; referencePoints[7].y = 22;
            referencePoints[8].x = 19; referencePoints[8].y = 13;
            break;
    }
}

/* Get reference point at specific grid location */
ReferencePoint* getReferencePoint(int x, int y) {
    int i = 0;

    while (referencePoints[i].id != 0) {
        if (referencePoints[i].x == x && referencePoints[i].y == y) {
            return &referencePoints[i];
        }
        i++;
    }

    return NULL;
}

/* Print all reference points (for debugging) */
void printReferencePoints(void) {
    int i = 0;
    const char* levelNames[] = { "", "7-G", "7-H", "7-I" };

    printf("\n");
    printf("========================================\n");
    printf(" GRID REFERENCE POINTS - SECTOR %s\n", levelNames[currentLevel]);
    printf("========================================\n\n");

    while (referencePoints[i].id != 0) {
        if (referencePoints[i].x > 0 || referencePoints[i].y > 0) {
            printf(" [%2d] <%2d,%2d> %s %s\n",
                referencePoints[i].id,
                referencePoints[i].x,
                referencePoints[i].y,
                referencePoints[i].name,
                referencePoints[i].active ? "[ONLINE]" : "[OFFLINE]"
            );
        }
        i++;
    }

    printf("\n========================================\n\n");
}
