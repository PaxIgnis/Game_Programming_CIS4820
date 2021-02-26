#include <stdbool.h>

/*
 * Struct to keep track of different levels
 * worldLegend[*][*][*][1] stores actual values from world array
 * worldLegend[*][*][*][0] stores additional info for each location
 */
typedef struct level {
    int worldLegend[WORLDX][WORLDY][WORLDZ][2];
    struct level* up;
    struct level* down;
    int lastLocation[3];
    int lastOrientation[3];
    int worldType;
    //used for dungeon levels
    int startingPoints[9][2]; // Room 0 is bottom left, room 1 is bottom middle...
    int roomSizes[9][2]; // size includes outside wall
} level;


#define WALL 0
#define CORRIDORWALL 1
#define FLOOR 2
#define CORRIDORFLOOR 3
#define DOORWAYPOST 4
#define DUNGEON 5
#define OUTDOOR 6


void animateClouds();
void animateLava();
void animateMesh(level *currentLevel);
void clearWorld();
void createDungeonLevel(level* currentLevel, int direction);
void createOutdoorLevel(level* currentLevel, int direction);
bool cubeInFrustum(float x, float y, float z, float size);
void handleCollision();
void handleGravityCollision();
level* initNewLevel(level* currentPos, int direction);
void meshVisibilityDetection();
void saveLevel(level* currentLevel);
void setColors();
void setUserValues(int var[3], double a, double b, double c);
level* teleport(level* currentLevel);