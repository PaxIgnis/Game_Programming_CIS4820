/*
 * Struct to keep track of different levels
 * worldLegend[*][*][*][1] stores actual values from world array
 * worldLegend[*][*][*][0] stores additional info for each location
 */
typedef struct level {
    int worldLegend[WORLDX][WORLDY][WORLDZ][2];
    struct level *up;
    struct level *down;
    int lastLocation[3];
    int lastOrientation[3];
    int worldType;
} level;


#define WALL 0
#define CORRIDORWALL 1
#define FLOOR 2
#define CORRIDORFLOOR 3
#define DOORWAYPOST 4
#define DUNGEON 5
#define OUTDOOR 6

// moves clouds across sky
void animateClouds();

// clears world values
void clearWorld();

// creates dungeon level
void createDungeonLevel(level* currentLevel, int direction);

// creates outdoor level
void createOutdoorLevel(level* currentLevel, int direction);

// handles collisions
void handleCollision();

// handles collisions while falling
void handleGravityCollision();

// Creates data struct to store new level
level* initNewLevel(level* currentPos, int direction);

// saves level details to dat struct
void saveLevel(level* currentLevel);

// sets custom colors
void setColors();

// shortcut to update user arrays
void setUserValues(int var[3], double a, double b, double c);

// teleports player to new level
level* teleport(level* currentLevel);