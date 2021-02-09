
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graphics.h"
#include "helper.h"

#define WALL 0
#define CORRIDORWALL 1
#define FLOOR 2
#define CORRIDORFLOOR 3

extern GLubyte world[WORLDX][WORLDY][WORLDZ];
extern void setViewPosition(float, float, float);
extern void getViewPosition(float*, float*, float*);
extern void getOldViewPosition(float*, float*, float*);
extern void setOldViewPosition(float, float, float);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);


/* 
 * Shortcut to update array
 */
void setUserValues(int var[3], double a, double b, double c) {
    var[0] = a;
    var[1] = b;
    var[2] = c;
}

/* Creates a new level structure to store the level,
 * adds it to list of levels, uses direction to store
 * the new levels in the up or down direction.
 */
level* initNewLevel(level* currentPos, int direction) {
    level* l = (level*)malloc(sizeof(level));
    if (currentPos == NULL) {
        l->up = NULL;
        l->down = NULL;
        
    } else if (direction > 0) {
        currentPos->up = l;
        l->up = NULL;
        l->down = currentPos;
    } else {
        currentPos->down = l;
        l->down = NULL;
        l->up = currentPos;
    }
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDY; j++) {
            for (int k = 0; k < WORLDZ; k++) {
                l->worldLegend[i][j][k][0] = 0;
                l->worldLegend[i][j][k][1] = 0;
            }
        }
    }

    return l;
}

/*
 * Creates a level and sets player in one of the rooms.
 * 
 */
void createLevel(level* currentLevel) {
    // array to store 2D world
    int worldLegend[WORLDX][1][WORLDZ];
    int startingPoints[9][2]; // Room 0 is bottom left, room 1 is bottom middle...
    int roomSizes[9][2];
    int i, j, k, x, z;

    // generate 9 rooms
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {

            // generate random room size that fits each of the quadrants
            x = (rand() % (30 - 3 + 1)) + 3;
            z = (rand() % (30 - 3 + 1)) + 3;
            roomSizes[(3 * j) + i][0] = x;
            roomSizes[(3 * j) + i][1] = z;

            // select random bottom left corner for each room
            x = (rand() % ((30 + (34 * j) - roomSizes[(3 * j) + i][0]) - (34 * j) + 1)) + 34 * j;
            z = (rand() % ((30 + (34 * i) - roomSizes[(3 * j) + i][1]) - (34 * i) + 1)) + 34 * i;
            startingPoints[(3 * j) + i][0] = x;
            startingPoints[(3 * j) + i][1] = z;
        }
    }

    // initialize 2d world array to be empty
    for (i = 0; i < WORLDX; i++) {
        for (j = 0; j < WORLDZ; j++) {
            worldLegend[i][0][j] = -1;
            for (int k = 0; k < WORLDY; k++) {
                world[i][k][j] = 0;
            }
        }
    }

    /* generate walls */
    for (int i = 0; i < 9; i++) {
        // build walls along x axis (including corners)
        for (int j = startingPoints[i][0]; j < (startingPoints[i][0] + roomSizes[i][0] + 2); j++) {
            for (int k = 0; k < 5; k++) {
                world[j][25 + k][startingPoints[i][1]] = (rand() % (15 + 1 - 11)) + 11;
                world[j][25 + k][startingPoints[i][1] + roomSizes[i][1] + 1] = (rand() % (15 + 1 - 11)) + 11;
            }
            if (j > startingPoints[i][0] && j < (startingPoints[i][0] + roomSizes[i][0] + 1)) {
                for (int k = startingPoints[i][1] + 1; k < (startingPoints[i][1] + roomSizes[i][1] + 1); k++) {
                    worldLegend[j][0][k] = FLOOR;
                }
            }
        }
        // build walls along z axis
        for (int j = startingPoints[i][1] + 1; j < (startingPoints[i][1] + roomSizes[i][1] + 1); j++) {
            for (int k = 0; k < 5; k++) {
                world[startingPoints[i][0]][25 + k][j] = (rand() % (15 + 1 - 11)) + 11;
                world[startingPoints[i][0] + roomSizes[i][0] + 1][25 + k][j] = (rand() % (15 + 1 - 11)) + 11;
            }
        }
    }

    /* generate corridors that connect the doorways along z axis*/
    for (int i = 1; i < 7; i++) {
        // generate corridors along z axis
        int firstRoom = ((double)(0.25 * (pow(-1, i))) * ((6 * (pow(-1, i)) * i - 7 * (pow(-1, i)) - 1))); // corridor on right side (pattern: 0,1,3,4,6,7)
        int secondRoom = ((double)(0.25 * (pow(-1, i))) * ((6 * (pow(-1, i)) * i - 7 * (pow(-1, i)) - 1))) + 1; // corridor on left side (pattern: 1,2,4,5,7,8)

        // select right facing corridor openings
        int rand1 = rand() % (roomSizes[firstRoom][0] - 1);
        for (int k = 0; k < 5; k++) {
            world[rand1 + startingPoints[firstRoom][0] + 1][25 + k][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = 0;
            world[rand1 + startingPoints[firstRoom][0] + 2][25 + k][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = 0;
        }
        worldLegend[rand1 + startingPoints[firstRoom][0] + 1][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = CORRIDORFLOOR;
        worldLegend[rand1 + startingPoints[firstRoom][0] + 2][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = CORRIDORFLOOR;

        // select left facing corridor openings
        int rand2 = rand() % (roomSizes[secondRoom][0] - 1);
        for (int k = 0; k < 5; k++) {
            world[rand2 + startingPoints[secondRoom][0] + 1][25 + k][startingPoints[secondRoom][1]] = 0;
            world[rand2 + startingPoints[secondRoom][0] + 2][25 + k][startingPoints[secondRoom][1]] = 0;
        }
        worldLegend[rand2 + startingPoints[secondRoom][0] + 1][0][startingPoints[secondRoom][1]] = CORRIDORFLOOR;
        worldLegend[rand2 + startingPoints[secondRoom][0] + 2][0][startingPoints[secondRoom][1]] = CORRIDORFLOOR;

        // select random location between both doorways to insert connecting corridor
        int yDistBetween = (startingPoints[secondRoom][1]) - (startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1) - 1;

        // choose where the connecting corridor starts
        int randDist = rand() % (yDistBetween - 1);

        // build corridor until join
        for (int i = 0; i < randDist; i++) {
            int x = rand1 + startingPoints[firstRoom][0];
            int z = startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + i;
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
            worldLegend[x + 1][0][z] = CORRIDORFLOOR;
            worldLegend[x + 2][0][z] = CORRIDORFLOOR;
            worldLegend[x + 3][0][z] = worldLegend[x + 3][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
        }
        // build join
        if ((rand1 + startingPoints[firstRoom][0]) < (rand2 + startingPoints[secondRoom][0])) { // if left door lower than right door
            int xDistBetween = (rand2 + startingPoints[secondRoom][0]) - (rand1 + startingPoints[firstRoom][0]);
            // build top/bottom of join
            for (int j = 0; j < 3; j++) {
                if ((randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j) < startingPoints[secondRoom][1]) {
                    if (worldLegend[rand1 + startingPoints[firstRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j] != CORRIDORFLOOR) {
                        worldLegend[rand1 + startingPoints[firstRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    if (worldLegend[rand2 + startingPoints[secondRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j] != CORRIDORFLOOR) {
                        worldLegend[rand2 + startingPoints[secondRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j] = CORRIDORWALL;
                    }
                }
            }
            // build sides of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // left side
                if (i > 2) {
                    if (worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist] != CORRIDORFLOOR) {
                        worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist] = CORRIDORWALL;
                    }
                }
                // right side
                if (i < xDistBetween + 1) {
                    if (worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 4 + randDist] != CORRIDORFLOOR) {
                        worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 4 + randDist] = CORRIDORWALL;
                    }
                }
                // FLOOR
                if (i > 0 && i < xDistBetween + 3) {
                    worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + randDist] = CORRIDORFLOOR;
                    worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 3 + randDist] = CORRIDORFLOOR;
                }
            }
        } else { // if right door is lower or equal to left door
            int xDistBetween = (rand1 + startingPoints[firstRoom][0]) - (rand2 + startingPoints[secondRoom][0]);
            // build top/bottom of join
            for (int j = 0; j < 3; j++) {
                if ((randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j) < startingPoints[secondRoom][1]) {
                    if (worldLegend[rand1 + startingPoints[firstRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j] != CORRIDORFLOOR) {
                        worldLegend[rand1 + startingPoints[firstRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    if (worldLegend[rand2 + startingPoints[secondRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j] != CORRIDORFLOOR) {
                        worldLegend[rand2 + startingPoints[secondRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j] = CORRIDORWALL;
                    }
                }
            }
            // build sides of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // left side
                if (i < xDistBetween + 1) {
                    if (worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist] != CORRIDORFLOOR) {
                        worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist] = CORRIDORWALL;
                    }
                }
                // right side
                if (i > 2) {
                    if (worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 4 + randDist] != CORRIDORFLOOR) {
                        worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 4 + randDist] = CORRIDORWALL;
                    }
                }
                // FLOOR 
                if (i > 0 && i < xDistBetween + 3) {
                    worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + randDist] = CORRIDORFLOOR;
                    worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 3 + randDist] = CORRIDORFLOOR;
                }
            }
        }
        // build corridor after join
        for (int i = 1; i < yDistBetween - randDist - 1; i++) {
            int x = rand2 + startingPoints[secondRoom][0];
            int z = startingPoints[secondRoom][1] - i;
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
            worldLegend[x + 1][0][z] = CORRIDORFLOOR;
            worldLegend[x + 2][0][z] = CORRIDORFLOOR;
            worldLegend[x + 3][0][z] = worldLegend[x + 3][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
        }
    }

    /* generate corridors that connect the doorways along x axis */
    for (int i = 0; i < 6; i++) {
        // generate corridors along x axis
        int firstRoom = i; // corridor on bottom 
        int secondRoom = i + 3; // corridor on top

        // select up facing corridor openings
        int rand1 = rand() % (roomSizes[firstRoom][1] - 1);
        for (int k = 0; k < 5; k++) {
            world[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][25 + k][rand1 + startingPoints[firstRoom][1] + 1] = 0;
            world[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][25 + k][rand1 + startingPoints[firstRoom][1] + 2] = 0;
        }
        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 1] = CORRIDORFLOOR;
        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 2] = CORRIDORFLOOR;
        // select down facing corridor openings
        int rand2 = rand() % (roomSizes[secondRoom][1] - 1);
        for (int k = 0; k < 5; k++) {
            world[startingPoints[secondRoom][0]][25 + k][rand2 + startingPoints[secondRoom][1] + 1] = 0;
            world[startingPoints[secondRoom][0]][25 + k][rand2 + startingPoints[secondRoom][1] + 2] = 0;
        }
        worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 1] = CORRIDORFLOOR;
        worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 2] = CORRIDORFLOOR;

        // select random location between both doorways to insert connecting corridor
        int yDistBetween = (startingPoints[secondRoom][0]) - (startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1) - 1;

        int randDist = rand() % (yDistBetween - 1); // choose where the connecting corridor starts
        // build corridor until join
        for (int i = 0; i < randDist; i++) {
            int x = startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + i;
            int z = rand1 + startingPoints[firstRoom][1];
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
            worldLegend[x][0][z + 1] = CORRIDORFLOOR;
            worldLegend[x][0][z + 2] = CORRIDORFLOOR;
            worldLegend[x][0][z + 3] = worldLegend[x][0][z + 3] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
        }

        // build join
        if ((rand1 + startingPoints[firstRoom][1]) < (rand2 + startingPoints[secondRoom][1])) { // if bottom door lower (z val) than top door
            int xDistBetween = (rand2 + startingPoints[secondRoom][1]) - (rand1 + startingPoints[firstRoom][1]);
            // build right/left of join
            for (int j = 0; j < 3; j++) {
                if ((randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j) < startingPoints[secondRoom][0]) {
                    if (worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1]] != CORRIDORFLOOR) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1]] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    if (worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1] + 3] != CORRIDORFLOOR) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1] + 3] = CORRIDORWALL;
                    }
                }
            }
            // build sides of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // bottom side
                if (i > 2) {
                    if (worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] != CORRIDORFLOOR) {
                        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] = CORRIDORWALL;
                    }
                }
                // top side
                if (i < xDistBetween + 1) {
                    if (worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 4 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] != CORRIDORFLOOR) {
                        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 4 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] = CORRIDORWALL;
                    }
                }
                // FLOOR
                if (i > 0 && i < xDistBetween + 3) {
                    worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] = CORRIDORFLOOR;
                    worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 3 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] = CORRIDORFLOOR;
                }
            }
        } else { // if top door is lower (z val) or equal to bottom door
            int xDistBetween = (rand1 + startingPoints[firstRoom][1]) - (rand2 + startingPoints[secondRoom][1]);
            // build left/right of join
            for (int j = 0; j < 3; j++) {
                if ((randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j) < startingPoints[secondRoom][0]) {
                    if (worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1] + 3] != CORRIDORFLOOR) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1] + 3] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    if (worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1]] != CORRIDORFLOOR) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1]] = CORRIDORWALL;
                    }
                }
            }
            // build side of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // bottom side
                if (i < xDistBetween + 1) {
                    if (worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] != CORRIDORFLOOR) {
                        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] = CORRIDORWALL;
                    }
                }
                // top side
                if (i > 2) {
                    if (worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 4 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] != CORRIDORFLOOR) {
                        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 4 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] = CORRIDORWALL;
                    }
                }
                // FLOOR
                if (i < xDistBetween + 3 && i > 0) {
                    worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] = CORRIDORFLOOR;
                    worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 3 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] = CORRIDORFLOOR;
                }
            }
        }
        // build corridor after join
        for (int i = 1; i < yDistBetween - randDist - 1; i++) {
            int x = startingPoints[secondRoom][0] - i;
            int z = rand2 + startingPoints[secondRoom][1];
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
            worldLegend[x][0][z + 1] = CORRIDORFLOOR;
            worldLegend[x][0][z + 2] = CORRIDORFLOOR;
            worldLegend[x][0][z + 3] = worldLegend[x][0][z + 3] == CORRIDORFLOOR ? CORRIDORFLOOR : CORRIDORWALL;
        }
    }

    // build items from legend
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDZ; j++) {
            if (worldLegend[i][0][j] == CORRIDORWALL || worldLegend[i][0][j] == WALL) {
                for (k = 0; k < 5; k++) {
                    world[i][25 + k][j] = (rand() % (15 + 1 - 11)) + 11;
                }
            }
            if (worldLegend[i][0][j] == CORRIDORFLOOR) {
                world[i][25][j] = (rand() % (5 + 1 - 4)) + 4;
                world[i][29][j] = (rand() % (15 + 1 - 11)) + 11;
            }
            if (worldLegend[i][0][j] == FLOOR) {
                // sets random blocks
                if (((rand() % (65 + 1 - 1)) + 1) == 1) {
                    world[i][26][j] = 10;
                }
                world[i][25][j] = (rand() % (20 + 1 - 16)) + 16;
                world[i][29][j] = (rand() % (15 + 1 - 11)) + 11;
            }

            currentLevel->worldLegend[i][0][j][0] = worldLegend[i][0][j];
        }
    }
    // updates world array actual vals
    for (i = 0; i < WORLDX; i++) {
        for (j = 0; j < WORLDY; j++) {
            for (int k = 0; k < WORLDZ; k++) {
                currentLevel->worldLegend[i][j][k][1] = world[i][j][k];
            }
        }
    }

    // place player in random room
    int room = rand() % 9;
    setViewPosition(-(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
    setOldViewPosition(-(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
    setViewOrientation(0, 135, 0);

    // updates user position and orientation in the level struct
    setUserValues(currentLevel->lastLocation, -(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
    setUserValues(currentLevel->lastOrientation, 0, 135, 0);
}

/*
 * Generates all colors needed.
 *
 *
 */
void setColors() {
    /* Set colors */
    // changing colors
    setUserColour(10, 1.0, 0.60, 0.20, 1.0, 1.0, 0.60, 0.20, 1.0);
    // vanadyl blue
    setUserColour(11, 0, 151.0 / 255.0, 230.0 / 255.0, 1.0, 0, 151.0 / 255.0, 230.0 / 255.0, 1.0);
    // matt purple
    setUserColour(12, 140.0 / 255.0, 122.0 / 255.0, 230.0 / 255.0, 1.0, 140.0 / 255.0, 122.0 / 255.0, 230.0 / 255.0, 1.0);
    // nanohanacha gold
    setUserColour(13, 225.0 / 255.0, 177.0 / 255.0, 44.0 / 255.0, 1.0, 225.0 / 255.0, 177.0 / 255.0, 44.0 / 255.0, 1.0);
    //skirret green
    setUserColour(14, 68.0 / 255.0, 189.0 / 255.0, 50.0 / 255.0, 1.0, 68.0 / 255.0, 189.0 / 255.0, 50.0 / 255.0, 1.0);
    // naval
    setUserColour(15, 64.0 / 255.0, 115.0 / 255.0, 158.0 / 255.0, 1.0, 64.0 / 255.0, 115.0 / 255.0, 158.0 / 255.0, 1.0);

    // hd orange
    setUserColour(16, 194.0 / 255.0, 54.0 / 255.0, 22.0 / 255.0, 1.0, 194.0 / 255.0, 54.0 / 255.0, 22.0 / 255.0, 1.0);
    // hint of pensive
    setUserColour(17, 220.0 / 255.0, 221.0 / 255.0, 22.0 / 255.0, 1.0, 220.0 / 255.0, 221.0 / 255.0, 22.0 / 255.0, 1.0);
    // chain gang grey
    setUserColour(18, 113.0 / 255.0, 128.0 / 255.0, 147.0 / 255.0, 1.0, 113.0 / 255.0, 128.0 / 255.0, 147.0 / 255.0, 1.0);
    // pico void
    setUserColour(19, 25.0 / 255.0, 42.0 / 255.0, 86.0 / 255.0, 1.0, 25.0 / 255.0, 42.0 / 255.0, 86.0 / 255.0, 1.0);
    // electromagnetic
    setUserColour(20, 47.0 / 255.0, 54.0 / 255.0, 64.0 / 255.0, 1.0, 47.0 / 255.0, 54.0 / 255.0, 64.0 / 255.0, 1.0);

    //grey 
    setUserColour(20, 70.0 / 255.0, 70.0 / 255.0, 70.0 / 255.0, 1.0, 70.0 / 255.0, 70.0 / 255.0, 70.0 / 255.0, 1.0);
}

/*
 * Handle collision when falling
 */
void handleGravityCollision() {
    float x,y,z;
    getViewPosition(&x,&y,&z);
    x = -x;
    y = -y;
    z = -z;
    if (world[(int)floor(x-0.3)][(int)y][(int)z] != 0) {
        x = (ceil((x-0.3))+0.31);
        printf("a\n");
    }
    if (world[(int)x][(int)y][(int)floor((z-0.3))] != 0) {
        z = (ceil(z-0.3)+0.31);
        printf("b\n");
    }
    if (world[(int)floor((x+0.3))][(int)y][(int)z] != 0) {
        x = (floor((x+0.3))-0.31);
        printf("c\n");
    }
    if (world[(int)x][(int)y][(int)floor((z+0.3))] != 0) {
        z = (floor((z+0.3))-0.31);
        printf("d\n");
    }
    setViewPosition(-x,-y,-z);
}

/*
 * Handles collision. Detects blocks, and allows user to 'slide' along surfaces.
 *
 */
void handleCollision() {
    float x, y, z, nextx, nexty, nextz;
    float xx, yy, zz;
    float alp1, alp2, alp3, a2, a3, u, v, RHS1, RHS2, newx, newz;

    getViewPosition(&x, &y, &z);
    getOldViewPosition(&xx, &yy, &zz);
    x = -x;
    nextx = x;
    z = -z;
    nextz = z;
    xx = -xx;
    yy = -yy;
    zz = -zz;
    float z2Array[4] = { z,z,zz,zz };
    float z1Array[4] = { zz,zz,z,z };
    float x1Array[4] = { xx,xx,x,x };
    float x2Array[4] = { x,x,xx,xx };
    float alp1Array[4] = { 45.0,22.5,78.75,67.5 };
    float alp2Array[4] = { 67.5,78.75,22.5,45.0 };
    // check 5 directions if obstruction is ahead (45 to 135 degree in direction of movement, every 22.5 degrees)
    for (int i = 0; i < 5; i++) {

        int deg = (int)(450.0 + atan2f(z - (zz), x - (xx)) * (180.0 / 3.14159265)) % 360;
        if (i == 4) { // if direction straight ahead
            newx = x;
            newz = z;
        } else { // if direction at an angle
            float x1 = x1Array[i];
            float x2 = x2Array[i];
            float z1 = z1Array[i];
            float z2 = z2Array[i];
            float alp1Angle = alp1Array[i];
            float alp2Angle = alp2Array[i];
            alp1 = (3.141592 / 180.0) * alp1Angle;
            alp2 = (3.141592 / 180.0) * alp2Angle;
            u = x2 - x1;
            v = z2 - z1;
            a3 = sqrt(pow(u, 2) + pow(v, 2));
            alp3 = 3.141592 - alp1 - alp2;
            a2 = a3 * sin(alp2) / sin(alp3);
            RHS1 = x1 * u + z1 * v + a2 * a3 * cos(alp1);
            RHS2 = z2 * u - x2 * v - a2 * a3 * sin(alp1);
            newx = (1 / pow(a3, 2)) * (u * RHS1 - v * RHS2);
            newz = (1 / pow(a3, 2)) * (v * RHS1 + u * RHS2);
        }
        // distance of next move
        float dist = fabs(sqrt(pow(xx - ((-newx + xx) * 2) - xx, 2) + pow(zz - ((-newz + zz) * 2) - zz, 2)));
        // check if new location is inside a block
        if (world[(int)floor(xx - ((-newx + xx) * 2))][(int)floor(yy)][(int)floor(zz - ((-newz + zz) * 2))] != 0 && world[(int)floor(xx - ((-newx + xx) * 2))][(int)floor(yy + 1)][(int)floor(zz - ((-newz + zz) * 2))] == 0) {
            // move on top of single block
            setViewPosition(-(xx - ((-newx + xx) * 2)), -(yy + 1), -(zz - ((-newz + zz) * 2)));
        } else if (world[(int)floor(xx - ((-newx + xx) * 2))][(int)floor(yy)][(int)floor(zz - ((-newz + zz) * 2))] != 0) {
            // prevent 'sticking' to walls
            // split 360 degrees into 8 octants and check each one
            x = xx;
            y = yy;
            z = zz;
            // first octant
            if (deg >= 0 && deg < 45) {
                //check left
                if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                    // if there is a block in up direction (+x)
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        x = floor(x + dist) - (dist / 4);
                        // if next x is free and block above is free
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                        z = ceil((z - (2 * dist))) + dist;
                    } else {
                        z = z - (dist / 4);
                    }
                    //check up
                } else if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        z = ceil(z - dist) + (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = floor((x + (2 * dist))) - dist;
                    } else {
                        x = x + (dist / 4);
                    }
                }
                // second octant
            } else if (deg >= 45 && deg < 90) {
                //check up
                if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        z = ceil(z - dist) + (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = floor((x + (2 * dist))) - dist;
                    } else {
                        x = x + (dist / 4);
                    }
                    //check left
                } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        x = floor(x + dist) - (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                        z = ceil((z - (2 * dist))) + dist;
                    } else {
                        z = z - (dist / 4);
                    }
                }
                // third octant
            } else if (deg >= 90 && deg < 135) {
                //check up
                if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        z = floor(z + dist) - (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = floor((x + (2 * dist))) - dist;
                    } else {
                        x = x + (dist / 4);
                    }
                    //check right
                } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        x = floor(x + dist) - (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                        z = floor((z + (2 * dist))) - dist;
                    } else {
                        z = z + (dist / 4);
                    }
                }
                // fourth octant
            } else if (deg >= 135 && deg < 180) {
                // check right
                if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        x = floor(x + dist) - (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                        z = floor((z + (2 * dist))) - dist;
                    } else {
                        z = z + (dist / 4);
                    }
                    // check up
                } else if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        z = floor(z + dist) - (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = floor((x + (2 * dist))) - dist;
                    } else {
                        x = x + (dist / 4);
                    }
                }
                // fifth octant
            } else if (deg >= 180 && deg < 225) {
                // check right
                if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        x = ceil(x - dist) + (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                        z = floor((z + (2 * dist))) - dist;
                    } else {
                        z = z + (dist / 4);
                    }
                    // check down
                } else if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        z = floor(z + dist) - (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = ceil((x - (2 * dist))) + dist;
                    } else {
                        x = x - (dist / 4);
                    }
                }
                // sixth octant
            } else if (deg >= 225 && deg < 270) {
                // check down
                if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        z = floor(z + dist) - (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = ceil((x - (2 * dist))) + dist;
                    } else {
                        x = x - (dist / 4);
                    }
                    // check right
                } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                        x = ceil(x - dist) + (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                        z = floor((z + (2 * dist))) - dist;
                    } else {
                        z = z + (dist / 4);
                    }
                }
                // seventh octant
            } else if (deg >= 270 && deg < 315) {
                // check down
                if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        z = ceil(z - dist) + (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = ceil((x - (2 * dist))) + dist;
                    } else {
                        x = x - (dist / 4);
                    }
                    // check left
                } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        x = ceil(x - dist) + (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                        z = ceil((z - (2 * dist))) + dist;
                    } else {
                        z = z - (dist / 4);
                    }
                }
                // eight octant
            } else if (deg >= 315 && deg <= 360) {
                // check left
                if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        x = ceil(x - dist) + (dist / 4);
                    } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
                        x = nextx;
                    }
                    if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                        z = ceil((z - (2 * dist))) + dist;
                    } else {
                        z = z - (dist / 4);
                    }
                    // check down
                } else if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
                    if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                        z = ceil(z - dist) + (dist / 4);
                    } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
                        z = nextz;
                    }
                    if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                        x = ceil((x - (2 * dist))) + dist;
                    } else {
                        x = x - (dist / 4);
                    }
                }
            }

            if (world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                setViewPosition(-x, -y, -z);
            } else {
                setViewPosition(-xx, -yy, -zz);
            }
            break;
        }
    }
}