
/*
 * Owner: Hendrik van der Meijden
 * Date: Feb 2021
 *
 * Defines functions used for CIS4820 W21
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h> 

#include "graphics.h"
#include "helper.h"
#include "perlin.h"

extern GLubyte world[WORLDX][WORLDY][WORLDZ];
extern void setViewPosition(float, float, float);
extern void getViewPosition(float*, float*, float*);
extern void getOldViewPosition(float*, float*, float*);
extern void setOldViewPosition(float, float, float);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float*, float*, float*);
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);
/* texture functions */
extern int setAssignedTexture(int, int);
extern void unsetAssignedTexture(int);
extern int getAssignedTexture(int);
extern void setTextureOffset(int, float, float);
/* mesh creation, translatio, rotation functions */
extern void setMeshID(int, int, float, float, float);
extern void unsetMeshID(int);
extern void setTranslateMesh(int, float, float, float);
extern void setRotateMesh(int, float, float, float);
extern void setScaleMesh(int, float);
extern void drawMesh(int);
extern void hideMesh(int);
extern int screenWidth;
extern int screenHeight;
extern int skySize;
extern float mvx;
extern float mvy;


/*
 * Function: animateLava
 * -------------------
 *
 * Called to move lava texture by an offset
 *
 */
void animateLava() {
    static struct timeval t, t1;
    static float textureOffset = 0.0;
    static int initialized;

    if (initialized == 0) {
        initialized = 1;
        gettimeofday(&t, NULL);
    }
    gettimeofday(&t1, NULL);

    double elapsedTime = (t1.tv_sec - t.tv_sec) * 1000.0; // sec to ms
    elapsedTime += (t1.tv_usec - t.tv_usec) / 1000.0; // us to ms

    // update clouds every 50 ms
    if (elapsedTime > 50) {
        textureOffset -= 0.01;
        setTextureOffset(48, 0.0, textureOffset);
        setTextureOffset(49, 0.0, textureOffset);
        gettimeofday(&t, NULL);
    }
}

/*
 * Function: animateClouds
 * -------------------
 *
 * Called to move clouds across the sky
 *
 */
void animateClouds() {
    static struct timeval t, t1;
    static int i;

    static int initialized;
    if (initialized == 0) {
        initialized = 1;
        gettimeofday(&t, NULL);
    }
    gettimeofday(&t1, NULL);

    double elapsedTime = (t1.tv_sec - t.tv_sec) * 1000.0; // sec to ms
    elapsedTime += (t1.tv_usec - t.tv_usec) / 1000.0; // us to ms

    // update clouds every 150 ms
    if (elapsedTime > 150) {
        for (int x = 0; x < WORLDX; x++) {
            for (int z = 0; z < WORLDZ; z++) {
                float height = perlin2d(x + i, z, 0.1, 1);

                if ((int)((float)height * 24.0) > 18) {
                    world[x][40][z] = 5;
                } else {
                    world[x][40][z] = 0;
                }
            }
        }
        gettimeofday(&t, NULL);
        i++;
    }
}


/*
 * Function: animateMesh
 * -------------------
 *
 * Called to move Meshes around in their respective rooms
 *
 */
void animateMesh(level* currentLevel) {
    float x, y, z, xrot, yrot, zrot;
    static struct timeval t, t1;
    static int i;
    double speed = .05;

    static int initialized;
    if (initialized == 0) {
        initialized = 1;
        gettimeofday(&t, NULL);
    }
    gettimeofday(&t1, NULL);

    double elapsedTime = (t1.tv_sec - t.tv_sec) * 1000.0; // sec to ms
    elapsedTime += (t1.tv_usec - t.tv_usec) / 1000.0; // us to ms
    // update meshes every 50 ms
    if (elapsedTime > 50) {
        if (elapsedTime > 90) {
            speed = ((int)elapsedTime / 50) * .05;
        } else {
            speed = .05;
        }
        for (int i = 0; i < 9; i++) {
            int meshType = getMeshNumber(i);
            if (isMeshVisible(i) == 1) {
                getMeshLocation(i, &x, &y, &z);
                getMeshOrientation(i, &xrot, &yrot, &zrot);
                // cow (0) faces right and all else left on 0 degrees y rot
                if (meshType == 0) {
                    yrot = yrot + 270;
                } else {
                    yrot = yrot + 90;
                }
                if ((int)yrot >= 360) {
                    yrot = yrot - 360;
                } else if ((int)yrot < 0) {
                    yrot = yrot + 360;
                }
                // moving in left direction
                if ((int)yrot == 0) {
                    if ((int)floor(x) > currentLevel->startingPoints[i][0] + 2 &&
                        world[(int)floor(x)][26][(int)floor(z)] == 0 &&
                        world[(int)floor(x) - 1][26][(int)floor(z)] == 0 &&
                        world[(int)floor(x)][26][(int)floor(z) - 1] == 0 &&
                        world[(int)floor(x) - 1][26][(int)floor(z) - 1] == 0 &&
                        world[(int)floor(x)][26][(int)floor(z) + 1] == 0 &&
                        world[(int)floor(x) - 1][26][(int)floor(z) + 1] == 0) {
                        x = x - speed;
                    } else {
                        int direction = rand() % 3;
                        yrot = direction == 1 ? 90 : (direction == 2 ? 180 : 270);
                    }
                } else if ((int)yrot == 90) { // moving up direction
                    if ((int)floor(z) < (currentLevel->startingPoints[i][1] + currentLevel->roomSizes[i][1] - 2) &&
                        world[(int)floor(x)][26][(int)ceil(z)] == 0 &&
                        world[(int)floor(x)][26][(int)ceil(z) + 1] == 0 &&
                        world[(int)floor(x) - 1][26][(int)ceil(z)] == 0 &&
                        world[(int)floor(x) - 1][26][(int)ceil(z) + 1] == 0 &&
                        world[(int)floor(x) + 1][26][(int)ceil(z)] == 0 &&
                        world[(int)floor(x) + 1][26][(int)ceil(z) + 1] == 0) {
                        z = z + speed;
                    } else {
                        int direction = rand() % 3;
                        yrot = direction == 1 ? 0 : (direction == 2 ? 180 : 270);
                    }
                } else if ((int)yrot == 180) { // moving right direction
                    if ((int)ceil(x) < (currentLevel->startingPoints[i][0] + currentLevel->roomSizes[i][0] - 2) &&
                        world[(int)ceil(x)][(int)floor(26)][(int)floor(z)] == 0 &&
                        world[(int)ceil(x) + 1][(int)floor(26)][(int)floor(z)] == 0 &&
                        world[(int)ceil(x)][(int)floor(26)][(int)floor(z) + 1] == 0 &&
                        world[(int)ceil(x) + 1][(int)floor(26)][(int)floor(z) + 1] == 0 &&
                        world[(int)ceil(x)][(int)floor(26)][(int)floor(z) - 1] == 0 &&
                        world[(int)ceil(x) + 1][(int)floor(26)][(int)floor(z) - 1] == 0) {
                        x = x + speed;
                    } else {
                        int direction = rand() % 3;
                        yrot = direction == 1 ? 0 : (direction == 2 ? 90 : 270);
                    }
                } else if ((int)yrot == 270) { // moving down direction
                    if ((int)floor(z) > (currentLevel->startingPoints[i][1] + 2) &&
                        world[(int)floor(x)][26][(int)floor(z)] == 0 &&
                        world[(int)floor(x)][26][(int)floor(z) - 1] == 0 &&
                        world[(int)floor(x) + 1][26][(int)floor(z)] == 0 &&
                        world[(int)floor(x) + 1][26][(int)floor(z) - 1] == 0 &&
                        world[(int)floor(x) - 1][26][(int)floor(z)] == 0 &&
                        world[(int)floor(x) - 1][26][(int)floor(z) - 1] == 0) {
                        z = z - speed;
                    } else {
                        int direction = rand() % 3;
                        yrot = direction == 1 ? 0 : (direction == 2 ? 90 : 180);
                    }
                }
                if (meshType == 0) {
                    yrot = yrot - 270;
                } else {
                    yrot = yrot - 90;
                }
                if ((int)yrot >= 360) {
                    yrot = yrot - 360;
                } else if ((int)yrot < 0) {
                    yrot = yrot + 360;
                }
                setTranslateMesh(i, x, y, z);
                setRotateMesh(i, xrot, yrot, zrot);
            }
        }
        gettimeofday(&t, NULL);
    }
}


/*
 * Function: loadLevel
 * -------------------
 *
 * Loads level from stored level struct into the world
 * Assumes that world array has already been cleared
 *
 * newLevel: the pointer pointing to the struct that holds level data
 *
 */
void loadLevel(level* newLevel) {
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDY; j++) {
            for (int k = 0; k < WORLDZ; k++) {
                world[i][j][k] = newLevel->worldLegend[i][j][k][1];
            }
        }
    }
    setViewPosition(newLevel->lastLocation[0] + 0.5, newLevel->lastLocation[1] - 1, newLevel->lastLocation[2] + 0.5);
    setViewOrientation(newLevel->lastOrientation[0], newLevel->lastOrientation[1], newLevel->lastOrientation[2]);
}


/*
 * Function: clearWorld
 *  -------------------
 *
 * Cleans world, setting all values to 0
 *
 */
void clearWorld() {
    // clears world array
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDY; j++) {
            for (int k = 0; k < WORLDZ; k++) {
                world[i][j][k] = 0;
            }
        }
    }
}


/*
 * Function: createOutdoorLevel
 * -------------------
 *
 * Creates an outdoor level using perlin. Places user spawn and
 * teleportation cubes.
 *
 * currentLevel: pointer to struct to store new level
 * direction: int that tracks which direction the teleport blocks
 *            should go to. If direction is larger than 0 then there will
 *            be a single teleport block to go up to the next leve. If direction
 *            is less than 0 then there will be a single teleport cube to go down.
 *            If direction == 0 then there will be 2 teleport cubes, one going up and
 *            the other going down.
 *
 */
void createOutdoorLevel(level* currentLevel, int direction) {
    // build terain using perlin
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDZ; j++) {
            float height = perlin2d(i, j, 0.03, 4);
            int adjustedHeight = (int)14 + ((float)height * 24.0);
            // create a checkerboard pattern
            if (((adjustedHeight % 2 == 0) && ((i + j) % 2 != 0)) || ((adjustedHeight % 2 != 0) && ((i + j) % 2 == 0))) {
                if (adjustedHeight <= 24) {
                    world[i][adjustedHeight][j] = 49;
                    world[i][adjustedHeight - 1][j] = 49;
                    world[i][adjustedHeight - 2][j] = 49;
                } else if (adjustedHeight > 30) {
                    world[i][adjustedHeight][j] = 51;
                    world[i][adjustedHeight - 1][j] = 51;
                    world[i][adjustedHeight - 2][j] = 51;
                } else {
                    world[i][adjustedHeight][j] = 42;
                    world[i][adjustedHeight - 1][j] = 42;
                    world[i][adjustedHeight - 2][j] = 42;
                }
            } else {
                if (adjustedHeight <= 24) {
                    world[i][adjustedHeight][j] = 48;
                    world[i][adjustedHeight - 1][j] = 48;
                    world[i][adjustedHeight - 2][j] = 48;
                } else if (adjustedHeight > 30) {
                    world[i][adjustedHeight][j] = 52;
                    world[i][adjustedHeight - 1][j] = 52;
                    world[i][adjustedHeight - 2][j] = 52;
                } else {
                    world[i][adjustedHeight][j] = 50;
                    world[i][adjustedHeight - 1][j] = 50;
                    world[i][adjustedHeight - 2][j] = 50;
                }
            }
        }
    }

    // sets teleport block(s)
    int xCoord = (rand() % (WORLDX - 10)) + 5;
    int zCoord = (rand() % (WORLDZ - 10)) + 5;
    if (direction > 0) {
        for (int i = WORLDY - 1; i > 0; i--) {
            if (world[xCoord][i][zCoord] != 0) {
                world[xCoord][i + 1][zCoord] = 5;
                break;
            }
        }
    } else if (direction < 0) {
        for (int i = WORLDY - 1; i > 0; i--) {
            if (world[xCoord][i][zCoord] != 0) {
                world[xCoord][i + 1][zCoord] = 21;
                break;
            }
        }
    } else {
        for (int i = WORLDY - 1; i > 0; i--) {
            if (world[xCoord][i][zCoord] != 0) {
                world[xCoord][i + 1][zCoord] = 5;
                break;
            }
        }
        for (int i = WORLDY - 1; i > 0; i--) {
            if (world[xCoord][i][zCoord - 3] != 0) {
                world[xCoord][i + 1][zCoord - 3] = 21;
            }
        }
    }
    // places user spawn near teleport cubes
    for (int i = WORLDY - 1; i > 0; i--) {
        if (world[xCoord - 3][i][zCoord] != 0) {
            setViewPosition(-(xCoord - 3), -(i + 5), -(zCoord));
            break;
        }
    }
    setViewOrientation(0, 135, 0);
    currentLevel->worldType = OUTDOOR;
    saveLevel(currentLevel);
}

void drawMap(level* currentLevel) {
    GLfloat green[] = { 0.0, 0.5, 0.0, .98 };
    GLfloat red[] = { 0.5, 0.0, 0.0, .98 };
    GLfloat blue[] = { 0.0, 0.0, 1, .98 };
    GLfloat black[] = { 0.1, 0.1, 0.1, .98 };
    GLfloat white[] = { 1, 1, 1, .98 };
    GLfloat grey[] = { 0.3, 0.3, 0.3, .98 };
    GLfloat lightGreen[] = { 0.0, 0.7, 0.0, .2 };
    GLfloat yellow[] = { 0.8, 0.8, 0.1, .98 };
    GLfloat orange[] = { 0.95, 0.35, 0.01, .98 };
    GLfloat snow[] = { 0.9, 0.9, 0.9, .98 };
    GLfloat brown[] = { 0.25, 0.08, 0.1, .98 };

    float x, y, z;

    if (currentLevel->worldType == DUNGEON) {
        glClear(GL_DEPTH_BUFFER_BIT);
        set2Dcolour(black);
        // draw rooms
        for (int i = 0; i < 9; i++) {
            draw2Dbox((int)(((currentLevel->startingPoints[i][1] + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                (int)(((currentLevel->startingPoints[i][0] + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                (int)(((currentLevel->startingPoints[i][1] + currentLevel->roomSizes[i][1] + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                (int)(((currentLevel->startingPoints[i][0] + currentLevel->roomSizes[i][0] + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
        }
        glClear(GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < WORLDX; i++) {
            for (int j = 0; j < WORLDZ; j++) {
                // draw corridors
                if (currentLevel->worldLegend[i][0][j][0] == CORRIDORFLOOR) {
                    set2Dcolour(red);
                    draw2Dbox((int)((j * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)((i * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                        (int)(((j + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)(((i + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
                }
                // draw random boxes (to jump over)
                if (world[i][26][j] == 47) {
                    set2Dcolour(blue);
                    draw2Dbox((int)((j * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)((i * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                        (int)(((j + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)(((i + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
                }
                // draw teleportation blocks (stairs)
                if (world[i][26][j] == 5) {
                    set2Dcolour(white);
                    draw2Dbox((int)((j * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)((i * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                        (int)(((j + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)(((i + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
                } else if (world[i][26][j] == 21) {
                    set2Dcolour(grey);
                    draw2Dbox((int)((j * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)((i * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                        (int)(((j + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                        (int)(((i + 1) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
                }
            }
        }
        // draw meshes
        for (int i = 0; i < 9; i++) {
            glClear(GL_DEPTH_BUFFER_BIT);
            // don't draw mesh if it is hidden
            if (isMeshVisible(i) == 1) {
                getMeshLocation(i, &x, &y, &z);
                set2Dcolour(yellow);
                draw2Dbox((int)((((z)-.5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                    (int)((((x)-.5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                    (int)(((z + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                    (int)(((x + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
            }
        }
    } else if (currentLevel->worldType == OUTDOOR) {
        // glClear(GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < WORLDX; i++) {
            for (int j = 0; j < WORLDY; j++) {
                for (int k = 0; k < WORLDZ; k++) {
                    // draw land mass (3 levels)
                    if (world[i][j + 1][k] == 0 && (world[i][j][k] == 48 || world[i][j][k] == 49 || world[i][j][k] == 50 ||
                        world[i][j][k] == 51 || world[i][j][k] == 52 || world[i][j][k] == 42)) {
                        if (j <= 24) {
                            set2Dcolour(orange);
                        } else if (j > 30) {
                            set2Dcolour(snow);
                        } else {
                            set2Dcolour(brown);
                        }
                        draw2Dbox((int)((((k)-.5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                            (int)((((i)-.5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                            (int)(((k + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                            (int)(((i + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
                    }
                    // draw teleportation blocks (stairs)
                    if (world[i][j][k] == 21) {
                        set2Dcolour(grey);
                        draw2Dbox((int)((((k)-.5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                            (int)((((i)-.5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
                            (int)(((k + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
                            (int)(((i + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
                    }
                }
            }
        }
    }

    // draw user
    getViewPosition(&x, &y, &z);
    glClear(GL_DEPTH_BUFFER_BIT);
    set2Dcolour(green);
    draw2Dbox((int)((((-z) - .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
        (int)((((-x) - .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
        (int)(((-z + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
        (int)(((-x + .5) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
    glClear(GL_DEPTH_BUFFER_BIT);
    // draw user viewing direction
    float rotx = (mvx / 180.0 * 3.141592);
    float roty = (mvy / 180.0 * 3.141592);
    // forward
    float vpx = x - sin(roty) * 15;
    float vpz = z + cos(roty) * 15;
    // left
    float lvpx = vpx + cos(roty) * 15;
    float lvpz = vpz + sin(roty) * 15;
    // right
    float rvpx = vpx - cos(roty) * 15;
    float rvpz = vpz - sin(roty) * 15;
    set2Dcolour(lightGreen);
    draw2Dtriangle((int)((((-z)) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
        (int)((((-x)) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
        (int)(((-lvpz) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
        (int)(((-lvpx) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)),
        (int)(((-rvpz) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)) + (screenWidth - screenHeight) / 2,
        (int)(((-rvpx) * (double)(screenHeight * 0.009)) + (double)(screenHeight * 0.05)));
}


/*
 * Function: getDistanceBetween(float x1, float y1, float x2, float y2)
 * -------------------
 *
 * Returns the distance between 2 points in 2d
 *
 */
float getDistanceBetween(float x1, float y1, float x2, float y2) {
    return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}


/*
 * Function: meshVisibilityDetection
 * -------------------
 *
 * Checks if mesh should be drawn or hidden by
 * checking if is falls within the users view
 *
 *
 */
void meshVisibilityDetection() {
    float x, y, z, curx, cury, curz, maxDist;

    maxDist = 35;
    void extractFrustum();
    getViewPosition(&curx, &cury, &curz);

    // loop through meshes
    for (int i = 0; i < 9; i++) {
        getMeshLocation(i, &x, &y, &z);
        if (cubeInFrustum(x + 1, y, z, 1.5) && getDistanceBetween(x, z, -curx, -curz) < maxDist) {
            if (isMeshVisible(i) == 0) {
                switch (getMeshNumber(i)) {
                case 0:
                    printf("Cow mesh #%d is visible.\n", i + 1);
                    break;
                case 1:
                    printf("Fish mesh #%d is visible.\n", i + 1);
                    break;
                case 2:
                    printf("Bat mesh #%d is visible.\n", i + 1);
                    break;
                case 3:
                    printf("Cactus mesh #%d is visible.\n", i + 1);
                    break;
                }
                drawMesh(i);
            }
        } else {
            if (isMeshVisible(i) == 1) {
                switch (getMeshNumber(i)) {
                case 0:
                    printf("Cow mesh #%d is not visible.\n", i + 1);
                    break;
                case 1:
                    printf("Fish mesh #%d is not visible.\n", i + 1);
                    break;
                case 2:
                    printf("Bat mesh #%d is not visible.\n", i + 1);
                    break;
                case 3:
                    printf("Cactus mesh #%d is not visible.\n", i + 1);
                    break;
                }
                hideMesh(i);
            }
        }
    }
}


/*
 * Function: currentLevel
 * -------------------
 *
 * Checks if player is on teleport cube
 * If they are it creates/loads new level and places user in it
 *
 * currentLevel: pointer to struct of current level
 *
 * return: pointer to struct containing new level
 *
 */
level* teleport(level* currentLevel) {
    float x, y, z;
    level* newLevel;
    getViewPosition(&x, &y, &z);
    x = -x;
    y = -y;
    z = -z;

    // if standing on teleport cube
    // #5 is white going up cube
    // #21 is grey going down cube
    if (world[(int)floor(x)][(int)floor(y - 1)][(int)floor(z)] == 5) {
        // going up
        saveLevel(currentLevel);
        clearWorld();
        if (currentLevel->up == NULL) {
            // create new level
            newLevel = initNewLevel(currentLevel, 1);
            // Logic will go here to choose new level type(s) in upcoming assignments
            createOutdoorLevel(newLevel, 0);
        } else {
            // load level from memory
            newLevel = currentLevel->up;
            loadLevel(newLevel);
        }
        return newLevel;
    } else if (world[(int)floor(x)][(int)floor(y - 1)][(int)floor(z)] == 21) {
        // going down
        saveLevel(currentLevel);
        clearWorld();
        if (currentLevel->down == NULL) {
            // create new level
            newLevel = initNewLevel(currentLevel, -1);
            // Change the int argument to control number of teleport cubes
            createDungeonLevel(newLevel, 1);
        } else {
            // load level from memory
            newLevel = currentLevel->down;
            loadLevel(newLevel);
        }
        return newLevel;
    }
    return currentLevel;
}


/*
 * Function: saveLevel
 * -------------------
 *
 * Saves all cubes and current player location into
 * the level struct
 *
 * currentLevel: pointer to current level struct
 *
 */
void saveLevel(level* currentLevel) {
    float x, y, z;

    // saves world array actual vals
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDY; j++) {
            for (int k = 0; k < WORLDZ; k++) {
                currentLevel->worldLegend[i][j][k][1] = world[i][j][k];
            }
        }
    }

    // turn off all meshes 
    if (currentLevel->worldType == DUNGEON) {
        for (int i = 0; i < 9; i++) {
            hideMesh(i);
        }
    }

    // saves user position and orientation in the level struct
    getViewPosition(&x, &y, &z);
    setUserValues(currentLevel->lastLocation, x, y, z);
    getViewOrientation(&x, &y, &z);
    setUserValues(currentLevel->lastOrientation, x, y, z);
}


/*
 * Function: setUserValues
 * -------------------
 *
 * shortcut to update an array with 3 doubles
 *
 * var: array that holds 3 doubles
 * a: first double to store
 * b: second double to store
 * c: third double to store
 *
 */
void setUserValues(int var[3], double a, double b, double c) {
    var[0] = a;
    var[1] = b;
    var[2] = c;
}


/*
 * Function: initNewLevel
 * -------------------
 *
 * Creates a new level structure to store the level,
 * adds it to list of levels, uses direction to store
 * the new levels in the up or down direction.
 *
 * currentPos: struct containing current level
 * direction: int that tracks which direction user is moving in
 *            if it is positive, the new level is up
 *            if it is negative, the new level is down
 *
 * return: pointer to struct for new level
 *
 */
level* initNewLevel(level* currentPos, int direction) {
    level* l = (level*)malloc(sizeof(level));
    // manage linked list
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
 * Function: createDungeonLevel
 * -------------------
 *
 * Creates a dungeon level and sets player in one of the rooms.
 * Direction controls the teleport blocks
 *
 * currentLevel: pointer to struct to store new level
 * direction: int that tracks which direction the teleport blocks
 *            should go to. If direction is larger than 0 then there will
 *            be a single teleport block to go up to the next leve. If direction
 *            is less than 0 then there will be a single teleport cube to go down.
 *            If direction == 0 then there will be 2 teleport cubes, one going up and
 *            the other going down.
 *
 */
void createDungeonLevel(level* currentLevel, int direction) {
    // array to store 2D world
    int minimumRoomSize = 6; // measured inside length
    int worldLegend[WORLDX][1][WORLDZ];
    int startingPoints[9][2]; // Room 0 is bottom left, room 1 is bottom middle...
    int roomSizes[9][2];
    int i, j, k, x, z, block;

    // generate 9 rooms
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {

            // generate random room size that fits each of the quadrants
            x = (rand() % (30 - minimumRoomSize + 1)) + minimumRoomSize;
            z = (rand() % (30 - minimumRoomSize + 1)) + minimumRoomSize;
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

    // generate walls
    for (int i = 0; i < 9; i++) {
        // build walls along x axis (including corners)
        for (int j = startingPoints[i][0]; j < (startingPoints[i][0] + roomSizes[i][0] + 2); j++) {
            worldLegend[j][0][startingPoints[i][1]] = WALL;
            worldLegend[j][0][startingPoints[i][1] + roomSizes[i][1] + 1] = WALL;
            if (j > startingPoints[i][0] && j < (startingPoints[i][0] + roomSizes[i][0] + 1)) {
                for (int k = startingPoints[i][1] + 1; k < (startingPoints[i][1] + roomSizes[i][1] + 1); k++) {
                    worldLegend[j][0][k] = FLOOR;
                }
            }
        }
        // build walls along z axis
        for (int j = startingPoints[i][1] + 1; j < (startingPoints[i][1] + roomSizes[i][1] + 1); j++) {
            worldLegend[startingPoints[i][0]][0][j] = WALL;
            worldLegend[startingPoints[i][0] + roomSizes[i][0] + 1][0][j] = WALL;
        }
    }

    // generate corridors that connect the DOORWAYPOSTs along z axis
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
        worldLegend[rand1 + startingPoints[firstRoom][0]][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = DOORWAYPOST;
        worldLegend[rand1 + startingPoints[firstRoom][0] + 3][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = DOORWAYPOST;

        // select left facing corridor openings
        int rand2 = rand() % (roomSizes[secondRoom][0] - 1);
        for (int k = 0; k < 5; k++) {
            world[rand2 + startingPoints[secondRoom][0] + 1][25 + k][startingPoints[secondRoom][1]] = 0;
            world[rand2 + startingPoints[secondRoom][0] + 2][25 + k][startingPoints[secondRoom][1]] = 0;
        }
        worldLegend[rand2 + startingPoints[secondRoom][0]][0][startingPoints[secondRoom][1]] = DOORWAYPOST;
        worldLegend[rand2 + startingPoints[secondRoom][0] + 1][0][startingPoints[secondRoom][1]] = CORRIDORFLOOR;
        worldLegend[rand2 + startingPoints[secondRoom][0] + 2][0][startingPoints[secondRoom][1]] = CORRIDORFLOOR;
        worldLegend[rand2 + startingPoints[secondRoom][0] + 3][0][startingPoints[secondRoom][1]] = DOORWAYPOST;

        // select random location between both doorways to insert connecting corridor
        int yDistBetween = (startingPoints[secondRoom][1]) - (startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1) - 1;

        // choose where the connecting corridor starts
        int randDist = rand() % (yDistBetween - 1);

        // build corridor until join
        for (int i = 0; i < randDist; i++) {
            int x = rand1 + startingPoints[firstRoom][0];
            int z = startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + i;
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x][0][z] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
            worldLegend[x + 1][0][z] = CORRIDORFLOOR;
            worldLegend[x + 2][0][z] = CORRIDORFLOOR;
            worldLegend[x + 3][0][z] = worldLegend[x + 3][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x + 3][0][z] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
        }
        // build join
        if ((rand1 + startingPoints[firstRoom][0]) < (rand2 + startingPoints[secondRoom][0])) { // if left door lower than right door
            int xDistBetween = (rand2 + startingPoints[secondRoom][0]) - (rand1 + startingPoints[firstRoom][0]);
            // build top/bottom of join
            for (int j = 0; j < 3; j++) {
                if ((randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j) < startingPoints[secondRoom][1]) {
                    block = worldLegend[rand1 + startingPoints[firstRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[rand1 + startingPoints[firstRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    block = worldLegend[rand2 + startingPoints[secondRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[rand2 + startingPoints[secondRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j] = CORRIDORWALL;
                    }
                }
            }
            // build sides of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // left side
                if (i > 2) {
                    block = worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist] = CORRIDORWALL;
                    }
                }
                // right side
                if (i < xDistBetween + 1) {
                    block = worldLegend[rand1 + startingPoints[firstRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 4 + randDist];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
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
                    block = worldLegend[rand1 + startingPoints[firstRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[rand1 + startingPoints[firstRoom][0] + 3][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 2 + j] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    block = worldLegend[rand2 + startingPoints[secondRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[rand2 + startingPoints[secondRoom][0]][0][randDist + startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + j] = CORRIDORWALL;
                    }
                }
            }
            // build sides of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // left side
                if (i < xDistBetween + 1) {
                    block = worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1 + randDist] = CORRIDORWALL;
                    }
                }
                // right side
                if (i > 2) {
                    block = worldLegend[rand2 + startingPoints[secondRoom][0] + i][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 4 + randDist];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
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
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x][0][z] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
            worldLegend[x + 1][0][z] = CORRIDORFLOOR;
            worldLegend[x + 2][0][z] = CORRIDORFLOOR;
            worldLegend[x + 3][0][z] = worldLegend[x + 3][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x + 3][0][z] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
        }
    }

    // generate corridors that connect the doorways along x axis 
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
        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1]] = DOORWAYPOST;
        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 1] = CORRIDORFLOOR;
        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 2] = CORRIDORFLOOR;
        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 3] = DOORWAYPOST;
        // select down facing corridor openings
        int rand2 = rand() % (roomSizes[secondRoom][1] - 1);
        for (int k = 0; k < 5; k++) {
            world[startingPoints[secondRoom][0]][25 + k][rand2 + startingPoints[secondRoom][1] + 1] = 0;
            world[startingPoints[secondRoom][0]][25 + k][rand2 + startingPoints[secondRoom][1] + 2] = 0;
        }
        worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1]] = DOORWAYPOST;
        worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 1] = CORRIDORFLOOR;
        worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 2] = CORRIDORFLOOR;
        worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 3] = DOORWAYPOST;

        // select random location between both doorways to insert connecting corridor
        int yDistBetween = (startingPoints[secondRoom][0]) - (startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1) - 1;

        int randDist = rand() % (yDistBetween - 1); // choose where the connecting corridor starts
        // build corridor until join
        for (int i = 0; i < randDist; i++) {
            int x = startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + i;
            int z = rand1 + startingPoints[firstRoom][1];
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x][0][z] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
            worldLegend[x][0][z + 1] = CORRIDORFLOOR;
            worldLegend[x][0][z + 2] = CORRIDORFLOOR;
            worldLegend[x][0][z + 3] = worldLegend[x][0][z + 3] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x][0][z + 3] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
        }

        // build join
        if ((rand1 + startingPoints[firstRoom][1]) < (rand2 + startingPoints[secondRoom][1])) { // if bottom door lower (z val) than top door
            int xDistBetween = (rand2 + startingPoints[secondRoom][1]) - (rand1 + startingPoints[firstRoom][1]);
            // build right/left of join
            for (int j = 0; j < 3; j++) {
                if ((randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j) < startingPoints[secondRoom][0]) {
                    block = worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1]];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1]] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    block = worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1] + 3];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1] + 3] = CORRIDORWALL;
                    }
                }
            }
            // build sides of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // bottom side
                if (i > 2) {
                    block = worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand1 + startingPoints[firstRoom][1] + i];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand1 + startingPoints[firstRoom][1] + i] = CORRIDORWALL;
                    }
                }
                // top side
                if (i < xDistBetween + 1) {
                    block = worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 4 + randDist][0][rand1 + startingPoints[firstRoom][1] + i];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
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
                    block = worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1] + 3];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 2 + j][0][rand1 + startingPoints[firstRoom][1] + 3] = CORRIDORWALL;
                    }
                }
                if ((randDist + j) > 0) {
                    block = worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1]];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[randDist + startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + j][0][rand2 + startingPoints[secondRoom][1]] = CORRIDORWALL;
                    }
                }
            }
            // build side of join
            for (int i = 0; i < xDistBetween + 4; i++) {
                // bottom side
                if (i < xDistBetween + 1) {
                    block = worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand2 + startingPoints[secondRoom][1] + i];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
                        worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1 + randDist][0][rand2 + startingPoints[secondRoom][1] + i] = CORRIDORWALL;
                    }
                }
                // top side
                if (i > 2) {
                    block = worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 4 + randDist][0][rand2 + startingPoints[secondRoom][1] + i];
                    if (block != CORRIDORFLOOR && block != DOORWAYPOST) {
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
            worldLegend[x][0][z] = worldLegend[x][0][z] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x][0][z] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
            worldLegend[x][0][z + 1] = CORRIDORFLOOR;
            worldLegend[x][0][z + 2] = CORRIDORFLOOR;
            worldLegend[x][0][z + 3] = worldLegend[x][0][z + 3] == CORRIDORFLOOR ? CORRIDORFLOOR : (worldLegend[x][0][z + 3] == DOORWAYPOST ? DOORWAYPOST : CORRIDORWALL);
        }
    }

    // build items from legend
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDZ; j++) {
            // build walls
            if (worldLegend[i][0][j] == CORRIDORWALL || worldLegend[i][0][j] == WALL) {
                for (k = 0; k < 5; k++) {
                    if (((k % 2 == 0) && ((i + j) % 2 != 0)) || ((k % 2 != 0) && ((i + j) % 2 == 0))) {
                        world[i][25 + k][j] = 44;
                    } else {
                        world[i][25 + k][j] = 45;
                    }
                }
            }
            // build doorway (seperate color for better visibility)
            if (worldLegend[i][0][j] == DOORWAYPOST) {
                for (k = 0; k < 5; k++) {
                    if (k % 2 == 0) {
                        world[i][25 + k][j] = 46;
                    } else {
                        world[i][25 + k][j] = 46;
                    }
                }
            }
            // build floor and random cubes
            if (worldLegend[i][0][j] == FLOOR || worldLegend[i][0][j] == CORRIDORFLOOR) {
                // sets random blocks
                if (((rand() % (65 + 1 - 1)) + 1) == 1 && worldLegend[i][0][j] == FLOOR) {
                    world[i][26][j] = 47;
                }
                if ((j % 2 == 0 && !(i % 2 == 0) || (!(j % 2 == 0) && (i % 2 == 0)))) {
                    world[i][25][j] = 40;
                    world[i][29][j] = 42;
                } else {
                    world[i][25][j] = 41;
                    world[i][29][j] = 43;
                }
            }
            currentLevel->worldLegend[i][0][j][0] = worldLegend[i][0][j];
        }
    }

    // place player in random room
    int room = rand() % 9;
    setViewPosition(-(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
    handleGravityCollision();
    setOldViewPosition(-(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
    setViewOrientation(0, 135, 0);

    // clears cubes from around player spawn
    for (i = startingPoints[room][0]; i < startingPoints[room][0] + 6; i++) {
        for (j = startingPoints[room][1]; j < startingPoints[room][1] + 6; j++) {
            if (world[i][26][j] == 47) {
                world[i][26][j] == 0;
            }
        }
    }

    // sets teleport block(s)
    if (direction > 0) {
        world[startingPoints[room][0] + 4][26][startingPoints[room][1] + 2] = 5;
    } else if (direction < 0) {
        world[startingPoints[room][0] + 2][26][startingPoints[room][1] + 4] = 21;
    } else {
        world[startingPoints[room][0] + 4][26][startingPoints[room][1] + 2] = 5;
        world[startingPoints[room][0] + 2][26][startingPoints[room][1] + 4] = 21;
    }

    // create and places Meshes

    // loop through 9 rooms
    for (int i = 0; i < 9; i++) {
        // select random starting location in room
        int x = (rand() % (roomSizes[i][0] + startingPoints[i][0] - 2 - (startingPoints[i][0] + 2) + 1)) + startingPoints[i][0] + 2;
        float y = 26.5;
        int z = (rand() % (roomSizes[i][1] + startingPoints[i][1] - 2 - (startingPoints[i][1] + 2) + 1)) + startingPoints[i][1] + 2;

        // pick random mesh
        int type = (rand() % 4);
        //draw mesh
        // mesh id matches which room they are placed in (i.e. mesh 1 is in room 1)
        setMeshID(i, type, x, type >= 2 ? 26 : y, z);
        setRotateMesh(i, 0.0, 0, 0.0);
        setScaleMesh(i, 0.5);
        hideMesh(i);

        // room data to struct
        currentLevel->roomSizes[i][0] = roomSizes[i][0];
        currentLevel->roomSizes[i][1] = roomSizes[i][1];
        currentLevel->startingPoints[i][0] = startingPoints[i][0];
        currentLevel->startingPoints[i][1] = startingPoints[i][1];
    }

    // updates world array actual vals
    for (int i = 0; i < WORLDX; i++) {
        for (int j = 0; j < WORLDY; j++) {
            for (int k = 0; k < WORLDZ; k++) {
                currentLevel->worldLegend[i][j][k][1] = world[i][j][k];
            }
        }
    }

    // updates user position and orientation in the level struct
    setUserValues(currentLevel->lastLocation, -(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
    setUserValues(currentLevel->lastOrientation, 0, 135, 0);

    currentLevel->worldType = DUNGEON;


}


/*
 * Function: setColors
 * -------------------
 *
 * Sets all of the custom colors
 *
 */
void setColors() {
    // grey 
    setUserColour(21, 70.0 / 255.0, 70.0 / 255.0, 70.0 / 255.0, 1.0, 70.0 / 255.0, 70.0 / 255.0, 70.0 / 255.0, 1.0);

    // set textures

    // floors
    setUserColour(40, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(40, 37);
    setUserColour(41, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(41, 30);

    // ceiling
    setUserColour(42, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(42, 42);
    setUserColour(43, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(43, 30);

    // walls
    setUserColour(44, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(44, 2);
    setUserColour(45, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(45, 7);

    // doorways
    setUserColour(46, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(46, 3);

    // random blocks
    setUserColour(47, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(47, 22);

    // lava
    setUserColour(49, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(49, 24);
    setUserColour(48, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(48, 52);

    // flower
    setUserColour(50, 0.8, 0.8, 0.8, 1.0, 0.8, 0.8, 0.8, 1.0);
    setAssignedTexture(50, 25);

    // snow
    setUserColour(51, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(51, 38);
    setUserColour(52, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    setAssignedTexture(52, 5);
}


/*
 * Function: handleGravityCollision
 * -------------------
 *
 * Handles collisions when user is falling (in motion)
 * Used to prevent clipping when user jumps off blocks.
 *
 */
void handleGravityCollision() {
    float x, y, z;
    getViewPosition(&x, &y, &z);
    x = -x;
    y = -y;
    z = -z;
    if (world[(int)floor(x - 0.3)][(int)y][(int)z] != 0) {
        x = (ceil((x - 0.3)) + 0.31);
    }
    if (world[(int)x][(int)y][(int)floor((z - 0.3))] != 0) {
        z = (ceil(z - 0.3) + 0.31);
    }
    if (world[(int)floor((x + 0.3))][(int)y][(int)z] != 0) {
        x = (floor((x + 0.3)) - 0.31);
    }
    if (world[(int)x][(int)y][(int)floor((z + 0.3))] != 0) {
        z = (floor((z + 0.3)) - 0.31);
    }
    setViewPosition(-x, -y, -z);
}


/*
 * Function: handleCollision
 * -------------------
 *
 * Handles collision. Detects blocks, and allows user to 'slide' along surfaces.
 * Speed is reduced when sliding to give the walls a 'sticky' feeling.
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
        // check out of bounds
        if (((int)floor(xx - ((-newx + xx) * 2)) < 0 || (int)floor(xx - ((-newx + xx) * 2)) >= WORLDX ||
            (int)floor(zz - ((-newz + zz) * 2)) < 0 || (int)floor(zz - ((-newz + zz) * 2)) >= WORLDZ)) {
            setViewPosition(-xx, -yy, -zz);
            break;
            // check if new location is inside a block
        } else if (world[(int)floor(xx - ((-newx + xx) * 2))][(int)floor(yy)][(int)floor(zz - ((-newz + zz) * 2))] != 0 &&
            world[(int)floor(xx - ((-newx + xx) * 2))][(int)floor(yy + 1)][(int)floor(zz - ((-newz + zz) * 2))] == 0) {
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

float frustum[6][4];

/*
 * Function: cubeInFrustrum
 * -------------------
 *
 * Calculates if a cubes falls inside the viewing frustrum
 *
 * Returns true if cube should be visible
 *
 * Source: http://www.crownandcutlass.com/features/technicaldetails/frustum.html
 *
 */
bool cubeInFrustum(float x, float y, float z, float size) {
    int p;

    for (p = 0; p < 6; p++) {
        if (frustum[p][0] * (x - size) + frustum[p][1] * (y - size) + frustum[p][2] * (z - size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x + size) + frustum[p][1] * (y - size) + frustum[p][2] * (z - size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x - size) + frustum[p][1] * (y + size) + frustum[p][2] * (z - size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x + size) + frustum[p][1] * (y + size) + frustum[p][2] * (z - size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x - size) + frustum[p][1] * (y - size) + frustum[p][2] * (z + size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x + size) + frustum[p][1] * (y - size) + frustum[p][2] * (z + size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x - size) + frustum[p][1] * (y + size) + frustum[p][2] * (z + size) + frustum[p][3] > 0)
            continue;
        if (frustum[p][0] * (x + size) + frustum[p][1] * (y + size) + frustum[p][2] * (z + size) + frustum[p][3] > 0)
            continue;
        return false;
    }
    return true;
}

/*
 * Function: extractFrustrum
 * -------------------
 *
 * Calculates the viewing frustum and tests if objects fall inside it
 * Source: http://www.crownandcutlass.com/features/technicaldetails/frustum.html
 *
 */
void extractFrustum() {
    float   proj[16];
    float   modl[16];
    float   clip[16];
    float   t;

    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    /* Combine the two matrices (multiply projection by modelview) */
    clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
    clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
    clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
    clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

    clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
    clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
    clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
    clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

    clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
    clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
    clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

    /* Extract the numbers for the RIGHT plane */
    frustum[0][0] = clip[3] - clip[0];
    frustum[0][1] = clip[7] - clip[4];
    frustum[0][2] = clip[11] - clip[8];
    frustum[0][3] = clip[15] - clip[12];

    /* Normalize the result */
    t = sqrt(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
    frustum[0][0] /= t;
    frustum[0][1] /= t;
    frustum[0][2] /= t;
    frustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    frustum[1][0] = clip[3] + clip[0];
    frustum[1][1] = clip[7] + clip[4];
    frustum[1][2] = clip[11] + clip[8];
    frustum[1][3] = clip[15] + clip[12];

    /* Normalize the result */
    t = sqrt(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
    frustum[1][0] /= t;
    frustum[1][1] /= t;
    frustum[1][2] /= t;
    frustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    frustum[2][0] = clip[3] + clip[1];
    frustum[2][1] = clip[7] + clip[5];
    frustum[2][2] = clip[11] + clip[9];
    frustum[2][3] = clip[15] + clip[13];

    /* Normalize the result */
    t = sqrt(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
    frustum[2][0] /= t;
    frustum[2][1] /= t;
    frustum[2][2] /= t;
    frustum[2][3] /= t;

    /* Extract the TOP plane */
    frustum[3][0] = clip[3] - clip[1];
    frustum[3][1] = clip[7] - clip[5];
    frustum[3][2] = clip[11] - clip[9];
    frustum[3][3] = clip[15] - clip[13];

    /* Normalize the result */
    t = sqrt(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
    frustum[3][0] /= t;
    frustum[3][1] /= t;
    frustum[3][2] /= t;
    frustum[3][3] /= t;

    /* Extract the FAR plane */
    frustum[4][0] = clip[3] - clip[2];
    frustum[4][1] = clip[7] - clip[6];
    frustum[4][2] = clip[11] - clip[10];
    frustum[4][3] = clip[15] - clip[14];

    /* Normalize the result */
    t = sqrt(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
    frustum[4][0] /= t;
    frustum[4][1] /= t;
    frustum[4][2] /= t;
    frustum[4][3] /= t;

    /* Extract the NEAR plane */
    frustum[5][0] = clip[3] + clip[2];
    frustum[5][1] = clip[7] + clip[6];
    frustum[5][2] = clip[11] + clip[10];
    frustum[5][3] = clip[15] + clip[14];

    /* Normalize the result */
    t = sqrt(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
    frustum[5][0] /= t;
    frustum[5][1] /= t;
    frustum[5][2] /= t;
    frustum[5][3] /= t;
}