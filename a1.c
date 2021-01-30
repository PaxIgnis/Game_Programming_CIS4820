
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h> 
#include "graphics.h"

#define WALL 0
#define CORRIDORWALL 1
#define FLOOR 2
#define CORRIDORFLOOR 3

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];

	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void setOldViewPosition(float, float, float);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern void addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* tube controls */
extern void createTube(int, float, float, float, float, float, float, int);
extern void hideTube(int);
extern void showTube(int);

	/* 2D drawing functions */
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);


	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate the space bar has been pressed */
extern int space;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 
	/* size of the window in pixels */
extern int screenWidth, screenHeight;
	/* flag indicates if map is to be printed */
extern int displayMap;
	/* flag indicates use of a fixed viewpoint */
extern int fixedVP;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

	/* allows users to define colours */
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);
void unsetUserColour(int);
extern void getUserColour(int, GLfloat *, GLfloat *, GLfloat *, GLfloat *,
    GLfloat *, GLfloat *, GLfloat *, GLfloat *); 

/********* end of extern variable declarations **************/
// variables to keep track of time
   struct timeval t1, t2, t3;

	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {

	/* your code for collisions goes here */
   float x, y, z, nextx, nexty, nextz;
   float xx, yy, zz;
   float alp1, alp2, alp3, a2, a3, u, v, RHS1, RHS2, newx, newz;

   getViewPosition(&x, &y, &z);
   getOldViewPosition(&xx, &yy, &zz);
   x=-x;
   nextx = x;
   z=-z;
   nextz = z;
   xx=-xx;
   yy=-yy;
   zz=-zz;
   float z2Array[4] = {z,z,zz,zz};
   float z1Array[4] = {zz,zz,z,z};
   float x1Array[4] = {xx,xx,x,x};
   float x2Array[4] = {x,x,xx,xx};
   float alp1Array[4] = {45.0,22.5,78.75,67.5};
   float alp2Array[4] = {67.5,78.75,22.5,45.0};
   // check 5 directions if obstruction is ahead (45 to 135 degree in direction of movement, every 22.5 degrees)
   for (int i = 0; i < 5; i++) {
      
      int deg = (int)(450.0 + atan2f(z-(zz), x-(xx)) * (180.0 / 3.14159265)) % 360;
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
         a2 = a3*sin(alp2)/sin(alp3);
         RHS1 = x1*u + z1*v + a2*a3*cos(alp1);
         RHS2 = z2*u - x2*v - a2*a3*sin(alp1);
         newx = (1/pow(a3, 2))*(u*RHS1-v*RHS2);
         newz = (1/pow(a3, 2))*(v*RHS1+u*RHS2);
      }
      // distance of next move
      float dist = fabs(sqrt(pow(xx-((-newx+xx)*2) - xx,2)+pow(zz-((-newz+zz)*2) - zz,2)));
      // check if new location is inside a block
      if (world[(int)floor(xx-((-newx+xx)*2))][(int)floor(yy)][(int)floor(zz-((-newz+zz)*2))] != 0 && world[(int)floor(xx-((-newx+xx)*2))][(int)floor(yy+1)][(int)floor(zz-((-newz+zz)*2))] == 0) {
         // move on top of single block
         setViewPosition(-(xx-((-newx+xx)*2)),-(yy+1),-(zz-((-newz+zz)*2)));
      } else if (world[(int)floor(xx-((-newx+xx)*2))][(int)floor(yy)][(int)floor(zz-((-newz+zz)*2))] != 0) {
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
                  x = floor(x + dist) - (dist/4);
               // if next x is free and block above is free
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0){
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                  z = ceil((z - (2 * dist))) + dist;
               } else {
                  z = z - (dist/4);
               }
            //check up
            } else if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  z = ceil(z - dist) + (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = floor((x + (2 * dist))) - dist;
               } else {
                  x = x + (dist/4);
               }
            }
         // second octant
         } else if (deg >= 45 && deg < 90) {
            //check up
            if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  z = ceil(z - dist) + (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z-dist)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = floor((x + (2 * dist))) - dist;
               } else {
                  x = x + (dist/4);
               }
            //check left
            } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  x = floor(x + dist) - (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0) {
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                  z = ceil((z - (2 * dist))) + dist;
               } else {
                  z = z - (dist/4);
               }
            }
         // third octant
         }  else if (deg >= 90 && deg < 135) {
            //check up
            if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  z = floor(z + dist) - (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = floor((x + (2 * dist))) - dist;
               } else {
                  x = x + (dist/4);
               }
            //check right
            } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  x = floor(x + dist) - (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0) {
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                  z = floor((z + (2 * dist))) - dist;
               } else {
                  z = z + (dist/4);
               }
            }
         // fourth octant
         } else if (deg >= 135 && deg < 180) {
            // check right
            if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  x = floor(x + dist) - (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)ceil(x)][(int)floor(y)][(int)floor(z)] == 0){
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                  z = floor((z + (2 * dist))) - dist;
               } else {
                  z = z + (dist/4);
               }
            // check up
            } else if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x + dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  z = floor(z + dist) - (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x + (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = floor((x + (2 * dist))) - dist;
               } else {
                  x = x + (dist/4);
               }
            }
         // fifth octant
         } else if (deg >= 180 && deg < 225) {
            // check right
            if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  x = ceil(x - dist) + (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x-dist)][(int)floor(y)][(int)floor(z)] == 0) {
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                  z = floor((z + (2 * dist))) - dist;
               } else {
                  z = z + (dist/4);
               }
            // check down
            } else if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  z = floor(z + dist) - (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = ceil((x - (2 * dist))) + dist;
               } else {
                  x = x - (dist/4);
               }
            }
         // sixth octant
         } else if (deg >= 225 && deg < 270) {
            // check down
            if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  z = floor(z + dist) - (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)ceil(z)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = ceil((x - (2 * dist))) + dist;
               } else {
                  x = x - (dist/4);
               }
            // check right
            } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + dist)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z + dist)] != 0) {
                  x = ceil(x - dist) + (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z + (2 * dist))] != 0) {
                  z = floor((z + (2 * dist))) - dist;
               } else {
                  z = z + (dist/4);
               }
            }
         // seventh octant
         } else if (deg >= 270 && deg < 315) {
            // check down
            if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  z = ceil(z - dist) + (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z-dist)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = ceil((x - (2 * dist))) + dist;
               } else {
                  x = x - (dist/4);
               }
            // check left
            } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  x = ceil(x - dist) + (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                  z = ceil((z - (2 * dist))) + dist;
               } else {
                  z = z - (dist/4);
               }
            }
         // eight octant
         } else if (deg >= 315 && deg <= 360) {
            // check left
            if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - dist)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  x = ceil(x - dist) + (dist/4);
               } else if (world[(int)floor(nextx)][(int)floor(y)][(int)floor(z)] == 0 && world[(int)floor(x-dist)][(int)floor(y)][(int)floor(z)] == 0) {
                  x = nextx;
               }
               if (world[(int)floor(x)][(int)floor(y)][(int)floor(z - (2 * dist))] != 0) {
                  z = ceil((z - (2 * dist))) + dist;
               } else {
                  z = z - (dist/4);
               }
            // check down
            } else if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z)] == 0) {
               if (world[(int)floor(x - dist)][(int)floor(y)][(int)floor(z - dist)] != 0) {
                  z = ceil(z - dist) + (dist/4);
               } else if (world[(int)floor(x)][(int)floor(y)][(int)floor(nextz)] == 0 && world[(int)floor(x)][(int)floor(y)][(int)floor(z-dist)] == 0) {
                  z = nextz;
               }
               if (world[(int)floor(x - (2 * dist))][(int)floor(y)][(int)floor(z)] != 0) {
                  x = ceil((x - (2 * dist))) + dist;
               } else {
                  x = x - (dist/4);
               }
            }
         }

         if (world[(int)floor(x)][(int)floor(y)][(int)floor(z)] == 0) {
            setViewPosition(-x,-y,-z);
         } else {
            setViewPosition(-xx, -yy, -zz);
         }
         break;
      }      
   }
}


	/******* draw2D() *******/
	/* draws 2D shapes on screen */
	/* use the following functions: 			*/
	/*	draw2Dline(int, int, int, int, int);		*/
	/*	draw2Dbox(int, int, int, int);			*/
	/*	draw2Dtriangle(int, int, int, int, int, int);	*/
	/*	set2Dcolour(float []); 				*/
	/* colour must be set before other functions are called	*/
void draw2D() {

   if (testWorld) {
		/* draw some sample 2d shapes */
      if (displayMap == 1) {
         GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
         set2Dcolour(green);
         draw2Dline(0, 0, 500, 500, 15);
         draw2Dtriangle(0, 0, 200, 200, 0, 200);

         GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
         set2Dcolour(black);
         draw2Dbox(500, 380, 524, 388);
      }
   } else {

	/* your code goes here */

   }

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k;
float *la;
float x, y, z;

	/* sample animation for the testworld, don't remove this code */
	/* demo of animating mobs */
   if (testWorld) {

	/* update old position so it contains the correct value */
	/* -otherwise view position is only correct after a key is */
	/*  pressed and keyboard() executes. */
#if 0
// Fire a ray in the direction of forward motion
float xx, yy, zz;
getViewPosition(&x, &y, &z);
getOldViewPosition(&xx, &yy, &zz);
printf("%f %f %f %f %f %f\n", xx, yy, zz, x, y, z);
printf("%f %f %f\n",  -xx+((x-xx)*25.0), -yy+((y-yy)*25.0), -zz+((z-zz)*25.0));
createTube(2, -xx, -yy, -zz, -xx-((x-xx)*25.0), -yy-((y-yy)*25.0), -zz-((z-zz)*25.0), 5);
#endif

      getViewPosition(&x, &y, &z);
      setOldViewPosition(x,y,z);

	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;
	/* counter for user defined colour changes */
      static int colourCount = 0;
      static GLfloat offset = 0.0;

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else 
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;

	/* change user defined colour over time */
      if (colourCount == 1) offset += 0.05;
      else offset -= 0.01;
      if (offset >= 0.5) colourCount = 0;
      if (offset <= 0.0) colourCount = 1;
      setUserColour(9, 0.7, 0.3 + offset, 0.7, 1.0, 0.3, 0.15 + offset, 0.3, 1.0);

	/* sample tube creation  */
	/* draws a purple tube above the other sample objects */
       createTube(1, 45.0, 30.0, 45.0, 50.0, 30.0, 50.0, 6);

    /* end testworld animation */


   } else {
	   /* your code goes here */
      float x, y, z;
      double r,g,b;

      gettimeofday(&t2, NULL);

      // calculate time passed since last run
      double elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; // sec to ms
      elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
      double elapsedTime2 = (t2.tv_sec - t3.tv_sec) * 1000.0; // sec to ms
      elapsedTime2 += (t2.tv_usec - t3.tv_usec) / 1000.0; // us to ms

      // change color of random blocks
      if (elapsedTime2 > 600.0) {
         r = ((rand() % (255 + 1 - 1)) + 1)/255.0;
         g = ((rand() % (255 + 1 - 1)) + 1)/255.0;
         b = ((rand() % (255 + 1 - 1)) + 1)/255.0;
         setUserColour(10, r, g, b, 1.0, r, g, b, 1.0);
         gettimeofday(&t3, NULL);
      }
      // add gravity
      if (elapsedTime > 20.0) {
         getViewPosition(&x, &y, &z);
         if(world[(int)floor(-x)][(int)floor(-(y))][(int)floor(-z)] == 0){ 
            y+=0.1;
            if (world[(int)floor(-x)][(int)floor(-(y))][(int)floor(-z)] != 0) {
               y = floor(y);
            }
         }
         setViewPosition(x, y, z);
         gettimeofday(&t1, NULL);
      }
   }
}


	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */ 
void mouse(int button, int state, int x, int y) {

   if (button == GLUT_LEFT_BUTTON)
      printf("left button - ");
   else if (button == GLUT_MIDDLE_BUTTON)
      printf("middle button - ");
   else
      printf("right button - ");

   if (state == GLUT_UP)
      printf("up - ");
   else
      printf("down - ");

   printf("%d %d\n", x, y);
}



int main(int argc, char** argv)
{
int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);


	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* create user defined colour and draw cube */
      setUserColour(9, 0.7, 0.3, 0.7, 1.0, 0.3, 0.15, 0.3, 1.0);
      world[54][25][50] = 9;


	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);
   } else {

	/* your code to build the world goes here */
   // array to store 2D world
   int worldLegend[WORLDX][1][WORLDZ];
   int startingPoints[9][2]; // Room 0 is bottom left, room 1 is bottom middle...
   int roomSizes[9][2];
   int x, z;

   srand((unsigned)time(NULL));
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
   for(i = 0; i < WORLDX; i++) {
      for(j = 0; j < WORLDZ; j++) {
         worldLegend[i][0][j] = -1;
      }
   }

   /* Set colors */
   // orange
   setUserColour(10, 1.0, 0.60, 0.20, 1.0, 1.0, 0.60, 0.20, 1.0);
   // vanadyl blue
   setUserColour(11, 0, 151.0/255.0, 230.0/255.0, 1.0, 0, 151.0/255.0, 230.0/255.0, 1.0);
   // matt purple
   setUserColour(12, 140.0/255.0, 122.0/255.0, 230.0/255.0, 1.0, 140.0/255.0, 122.0/255.0, 230.0/255.0, 1.0);
   // nanohanacha gold
   setUserColour(13, 225.0/255.0, 177.0/255.0, 44.0/255.0, 1.0, 225.0/255.0, 177.0/255.0, 44.0/255.0, 1.0);
   //skirret green
   setUserColour(14, 68.0/255.0, 189.0/255.0, 50.0/255.0, 1.0, 68.0/255.0, 189.0/255.0, 50.0/255.0, 1.0);
   // naval
   setUserColour(15, 64.0/255.0, 115.0/255.0, 158.0/255.0, 1.0, 64.0/255.0, 115.0/255.0, 158.0/255.0, 1.0);

   // hd orange
   setUserColour(16, 194.0/255.0, 54.0/255.0, 22.0/255.0, 1.0, 194.0/255.0, 54.0/255.0, 22.0/255.0, 1.0);
   // hint of pensive
   setUserColour(17, 220.0/255.0, 221.0/255.0, 22.0/255.0, 1.0, 220.0/255.0, 221.0/255.0, 22.0/255.0, 1.0);
   // chain gang grey
   setUserColour(18, 113.0/255.0, 128.0/255.0, 147.0/255.0, 1.0, 113.0/255.0, 128.0/255.0, 147.0/255.0, 1.0);
   // pico void
   setUserColour(19, 25.0/255.0, 42.0/255.0, 86.0/255.0, 1.0, 25.0/255.0, 42.0/255.0, 86.0/255.0, 1.0);
   // electromagnetic
   setUserColour(20, 47.0/255.0, 54.0/255.0, 64.0/255.0, 1.0, 47.0/255.0, 54.0/255.0, 64.0/255.0, 1.0);


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
      int firstRoom = ((double)(0.25 * (pow(-1,i))) * ((6 * (pow(-1,i)) * i - 7 * (pow(-1,i)) - 1))); // corridor on right side (pattern: 0,1,3,4,6,7)
      int secondRoom = ((double)(0.25 * (pow(-1,i))) * ((6 * (pow(-1,i)) * i - 7 * (pow(-1,i)) - 1))) + 1; // corridor on left side (pattern: 1,2,4,5,7,8)
      
      // select right facing corridor openings
      int rand1 = rand () % (roomSizes[firstRoom][0] - 1);
      for (int k = 0; k < 5; k++) {
         world[rand1 + startingPoints[firstRoom][0] + 1][25 + k][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = 0; 
         world[rand1 + startingPoints[firstRoom][0] + 2][25 + k][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = 0;
      }
      worldLegend[rand1 + startingPoints[firstRoom][0] + 1][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = CORRIDORFLOOR; 
      worldLegend[rand1 + startingPoints[firstRoom][0] + 2][0][startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1] = CORRIDORFLOOR;

      // select left facing corridor openings
      int rand2 = rand () % (roomSizes[secondRoom][0] - 1);
      for (int k = 0; k < 5; k++) {
         world[rand2 + startingPoints[secondRoom][0] + 1][25 + k][startingPoints[secondRoom][1]] = 0; 
         world[rand2 + startingPoints[secondRoom][0] + 2][25 + k][startingPoints[secondRoom][1]] = 0;
      }
      worldLegend[rand2 + startingPoints[secondRoom][0] + 1][0][startingPoints[secondRoom][1]] = CORRIDORFLOOR; 
      worldLegend[rand2 + startingPoints[secondRoom][0] + 2][0][startingPoints[secondRoom][1]] = CORRIDORFLOOR;

      // select random location between both doorways to insert connecting corridor
      int yDistBetween = (startingPoints[secondRoom][1]) - (startingPoints[firstRoom][1] + roomSizes[firstRoom][1] + 1) - 1;
      
      // choose where the connecting corridor starts
      int randDist = rand () % (yDistBetween - 1);
      
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
            if ((randDist + j) > 0 ) {
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
            if ((randDist + j) > 0 ) {
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
      int rand1 = rand () % (roomSizes[firstRoom][1] - 1);
      for (int k = 0; k < 5; k++) {
         world[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][25 + k][rand1 + startingPoints[firstRoom][1] + 1] = 0; 
         world[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][25 + k][rand1 + startingPoints[firstRoom][1] + 2] = 0;
      }
      worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 1] = CORRIDORFLOOR; 
      worldLegend[startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1][0][rand1 + startingPoints[firstRoom][1] + 2] = CORRIDORFLOOR;
      // select down facing corridor openings
      int rand2 = rand () % (roomSizes[secondRoom][1] - 1);
      for (int k = 0; k < 5; k++) {
         world[startingPoints[secondRoom][0]][25 + k][rand2 + startingPoints[secondRoom][1] + 1] = 0; 
         world[startingPoints[secondRoom][0]][25 + k][rand2 + startingPoints[secondRoom][1] + 2] = 0;
      }
      worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 1] = CORRIDORFLOOR; 
      worldLegend[startingPoints[secondRoom][0]][0][rand2 + startingPoints[secondRoom][1] + 2] = CORRIDORFLOOR;

      // select random location between both doorways to insert connecting corridor
      int yDistBetween = (startingPoints[secondRoom][0]) - (startingPoints[firstRoom][0] + roomSizes[firstRoom][0] + 1) - 1;
      
      int randDist = rand () % (yDistBetween - 1); // choose where the connecting corridor starts
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
            if ((randDist + j) > 0 ) {
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
            if ((randDist + j) > 0 ) {
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

   // place player in random room
   int room = rand () % 9;
   setViewPosition(-(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
   setOldViewPosition(-(startingPoints[room][0] + 2), -26, -(startingPoints[room][1] + 2));
   setViewOrientation(0,135,0);
   
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
      }
   }
   // start timer
   gettimeofday(&t1, NULL);

   }
	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

