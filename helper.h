#include <stdbool.h>


// Used for world generation
#define WALL 0
#define CORRIDORWALL 1
#define FLOOR 2
#define CORRIDORFLOOR 3
#define DOORWAYPOST 4
#define DUNGEON 5
#define OUTDOOR 6

// Number of meshes in dungeon
#define MESHCOUNT 9

// Mesh Types
#define COW 0
#define FISH 1
#define BAT 2
#define CACTUS 3

// FSA States
#define SEARCHING 0
#define WAITING 0
#define FOLLOWING 1
#define FIGHTING 1
#define INACTIVE 9

// FSA Events
#define NOTADJACENT 0
#define ADJACENT 1
#define VISIBLE 0
#define NOTVISIBLE 1
#define INROOM 0
#define NOTINROOM 1
#define ATTACK 0
#define ATTACKDIAGONAL 1
#define NOACTION 2

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
    // Fog of War
    int visitedWorld[WORLDX][WORLDZ];
    int visitedRooms[9];
    // Used for dungeon levels
    int startingPoints[9][2]; // Room 0 is bottom left, room 1 is bottom middle...
    int roomSizes[9][2]; // size includes outside wall
    // Used to keep track of Meshes AI
    int meshCurrentState[MESHCOUNT];
    // used to keep track of destination for searching meshes
    // index 0 is x coordinate and index 1 is z coordinate
    int meshSearchDest[MESHCOUNT][2];
} level;

/**
 * List of nodes, where x and y represent a position in the 2d
 * array representing the dungeon level
 * 
 */
typedef struct node {
    int x;
    int y;
    struct node *next;
    struct node *prev;
} Node;

/**
 * Used for the return type for the pathfinding bfs.
 * returns a boolean representing the success of a path,
 * and x,y coordinates for the next cube in the path if one
 * was found.
 * 
 */ 
typedef struct path {
    int x;
    int y;
    bool pathFound;
} Path;


/**
 * Queue used for the bfs
 *  
 */
typedef struct queue {
    Node *head;
    Node *tail;
    int length;
} Queue;


void animateClouds();
void animateLava();
void animateMesh(level* currentLevel);
void attackMesh(level* currentLevel, int meshId);
void attackPlayer(int meshId);
bool checkIfAdjacentUser(int meshId);
bool checkIfEmpty(level* currentLevel, int x, int y, int z);
void clearWorld();
void countUserTurn(level* currentLevel);
void createDungeonLevel(level* currentLevel, int direction);
void createOutdoorLevel(level* currentLevel, int direction);
bool cubeInFrustum(float x, float y, float z, float size);
void drawMap(level* currentLevel);
void drawFogMap(level *currentLevel);
void findNearestCorridor(int x, int z, int newx, int newz, int inputDirection, int distance, int maxMeasuredDist, level *currentLevel);
float getDistanceBetween(float x1, float y1, float x2, float y2);
void handleCollision(level* currentLevel);
void handleGravityCollision();
level* initNewLevel(level* currentPos, int direction);
void meshVisibilityDetection(level* currentLevel);
void runMeshTurn(level* currentLevel, int action, int meshId);
void runPlantFSA(level* currentLevel, int meshId);
void runRandomSearchFSA(level* currentLevel, int meshID);
void runResponsiveFSA(level* currentLevel, int meshID);
void countUserTurn(level* currentLevel);
void saveLevel(level* currentLevel);
void setColors();
void setUserValues(int var[3], double a, double b, double c);
level* teleport(level* currentLevel);
void updateFog(level* currentLevel);

/* 2D drawing functions */
extern void draw2Dline(int, int, int, int, int);
extern void draw2Dbox(int, int, int, int);
extern void draw2Dtriangle(int, int, int, int, int, int);
extern void set2Dcolour(float[]);
extern void draw2Dquad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);


bool queueIsEmpty(Queue * queue);
void queuePop(Queue* queue);
void queuePush(Queue* queue, Node* node);
Queue* newQueue(Queue* queue);
Node* newNode(int x, int y);
void freeQueue(Queue* queue);
bool nodesEqual(Node* first, Node* second);
Path bfs(int startX, int startY, int endX, int endY, level* currentLevel, Path nextStep);