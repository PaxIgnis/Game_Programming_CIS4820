// Struct to keep track of different levels
typedef struct level {
    int worldLegend[WORLDX][WORLDY][WORLDZ][2];
    struct level *up;
    struct level *down;
    int lastLocation[3];
    int lastOrientation[3];
} level;

// creates dungeon level
void createLevel(level* currentLevel);

// handles collisions
void handleCollision();

// handles collisions while falling
void handleGravityCollision();

// Creates data struct to store new level
level* initNewLevel(level* currentPos, int direction);

// sets custom colors
void setColors();