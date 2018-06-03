/*
 *  Asteroids 
 *
 *  Noah Attwood
 *  B00718872
 *  CSCI 3161

 todo:
    make ship enter the screen based on its velocity direction in which it left the screen. 
        Do ray casting to trace reverse of position until the ray is out of bounds and then use that position?

    ship projectiles leaving at the same direction as the ship is facing

    circular asteroids
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0

#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)

#define TIME_DELTA      33

#define MAX_VELOCITY    0.1
#define MAX_PHOTONS	    8
#define MAX_ASTEROIDS	8
#define MAX_VERTICES	16


#define drawCircle() glCallList(circle)


/* -- display list for drawing a circle ------------------------------------- */

static GLuint	circle;

void
buildCircle() {
    GLint   i;

    circle = glGenLists(1);
    glNewList(circle, GL_COMPILE);
      glBegin(GL_POLYGON);
        for(i=0; i<40; i++)
            glVertex2d(cos(i*M_PI/20.0), sin(i*M_PI/20.0));
      glEnd();
    glEndList();
}


/* -- type definitions ------------------------------------------------------ */

typedef struct Coords {
	double x, y;
} Coords;

typedef struct {
	double x, y, phi, dx, dy, radius, turnSpeed, acceleration;
} Ship;

typedef struct {
	int	active;
	double x, y, dx, dy, phi;
} Photon;

typedef struct {
	int	active, nVertices;
	double x, y, phi, dx, dy, dphi;
	Coords	coords[MAX_VERTICES];
} Asteroid;


/* -- function prototypes --------------------------------------------------- */

static void	myDisplay(void);
static void	myTimer(int value);
static void	myKey(unsigned char key, int x, int y);
static void	keyPress(int key, int x, int y);
static void	keyRelease(int key, int x, int y);
static void	myReshape(int w, int h);

static void	init(void);
static void	initAsteroid(Asteroid *a, double x, double y, double size);
static void	drawShip(Ship *s);
static void	drawPhoton(Photon *p);
static void	drawAsteroid(Asteroid *a);

static double myRandom(double min, double max);

static double clamp(double value, double min, double max);


/* -- global variables ------------------------------------------------------ */

static int	up = 0, down = 0, left = 0, right = 0, firing = 0; // state of user input
static double xMax, yMax;
static float timer;
static Ship	ship;
static Photon	photons[MAX_PHOTONS];
static Asteroid	asteroids[MAX_ASTEROIDS];


/* -- main ------------------------------------------------------------------ */

int
main(int argc, char *argv[])
{
    srand((unsigned int) time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Asteroids");
    buildCircle();
    glutDisplayFunc(myDisplay);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(myKey);
    glutSpecialFunc(keyPress);
    glutSpecialUpFunc(keyRelease);
    glutReshapeFunc(myReshape);
    glutTimerFunc(33, myTimer, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    init();
    
    glutMainLoop();
    
    return 0;
}


/* -- callback functions ---------------------------------------------------- */

void
myDisplay()
{
    /*
     *	display callback function
     */

    int	i;

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    drawShip(&ship);

    for (i=0; i<MAX_PHOTONS; i++)
    	if (photons[i].active)
            drawPhoton(&photons[i]);

    for (i=0; i<MAX_ASTEROIDS; i++)
    	if (asteroids[i].active)
            drawAsteroid(&asteroids[i]);
    
    glutSwapBuffers();
}

void
myTimer(int value)
{
    /*
     *	timer callback function
     */

    /* advance the ship */

    /* advance photon laser shots, eliminating those that have gone past
      the window boundaries */

    /* advance asteroids */

    /* test for and handle collisions */

    timer += TIME_DELTA;

    if (timer >= 1000){
        printf("ship coords: (%.2f, %.2f)\n", ship.x, ship.y);
        printf("direction: %.2f\n", ship.phi);
        printf("dx: %.2f\n", ship.dx);
        printf("dy: %.2f\n", ship.dy);
        printf("\n\n");
        timer -= 1000;
    }


    /****   update ship position  ****/

    ship.x = ship.x + ship.dx*TIME_DELTA;
    ship.y = ship.y + ship.dy*TIME_DELTA;

    // check if ship left the screen
    if (ship.x > xMax || ship.x < 0.0 || ship.y > yMax || ship.y < 0.0){
        // set the ship's new position on the screen dependent on their velocity

        double collisionCheckX = clamp(ship.x, 0.0, xMax);
        double collisionCheckY = clamp(ship.y, 0.0, yMax);

        while (collisionCheckX <= xMax && collisionCheckX >= 0 && collisionCheckY <= yMax && collisionCheckY >= 0){
            collisionCheckX -= ship.dx*2;
            collisionCheckY -= ship.dy*2;
        }

        collisionCheckX = clamp(collisionCheckX, 0.0, xMax);
        collisionCheckY = clamp(collisionCheckY, 0.0, yMax);

        ship.x = collisionCheckX;
        ship.y = collisionCheckY;
    }


    /****   process user input   ****/

    if (right == 1){
        ship.phi -= ship.turnSpeed;
        if (ship.phi < 0.0){
            ship.phi += 360.0;
        }
    }

    if (left == 1){
        ship.phi += ship.turnSpeed;
        if (ship.phi > 360.0){
            ship.phi -= 360.0;
        }
    }

    if (up == 1){
        double newDx = ship.dx - (ship.acceleration * sin((ship.phi - 90.0) * DEG2RAD) * TIME_DELTA);
        double newDy = ship.dy + (ship.acceleration * cos((ship.phi - 90.0) * DEG2RAD) * TIME_DELTA);

        if (sqrt(pow(newDx, 2) + pow(newDy, 2)) < MAX_VELOCITY){
            ship.dx = newDx;
            ship.dy = newDy;
        }
    }

    if (down == 1){
        double newDx = ship.dx + ((ship.acceleration/1.5) * sin((ship.phi - 90.0) * DEG2RAD) * TIME_DELTA);
        double newDy = ship.dy - ((ship.acceleration/1.5) * cos((ship.phi - 90.0) * DEG2RAD) * TIME_DELTA);

        if (sqrt(pow(newDx, 2) + pow(newDy, 2)) < MAX_VELOCITY){
            ship.dx = newDx;
            ship.dy = newDy;
        }
    }

    glutPostRedisplay();
    
    glutTimerFunc(TIME_DELTA, myTimer, value);		/* 30 frames per second */
}

double clamp (double value, double min, double max){
    value = value <= max ? value : max;
    value = value >= min ? value : min;
    return value;
}

void
myKey(unsigned char key, int x, int y)
{
    /*
     *	keyboard callback function; add code here for firing the laser,
     *	starting and/or pausing the game, etc.
     */

    switch(key){
        case ' ':
            firing = 1; break;
        case 'q':
            exit(0); break;
    }
}

void
keyPress(int key, int x, int y)
{
    /*
     *	this function is called when a special key is pressed; we are
     *	interested in the cursor keys only
     */

    switch (key)
    {
        case 100:
            left = 1; break;

        case 101:
            up = 1; break;

	    case 102:
            right = 1; break;

        case 103:
            down = 1; break;
    }
}

void
keyRelease(int key, int x, int y)
{
    /*
     *	this function is called when a special key is released; we are
     *	interested in the cursor keys only
     */

    switch (key)
    {
        case 100:
            left = 0; break;

        case 101:
            up = 0; break;

	    case 102:
            right = 0; break;

        case 103:
            down = 0; break;
    }
}

void
myReshape(int w, int h)
{
    /*
     *	reshape callback function; the upper and lower boundaries of the
     *	window are at 100.0 and 0.0, respectively; the aspect ratio is
     *  determined by the aspect ratio of the viewport
     */

    xMax = 100.0*w/h;
    yMax = 100.0;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, xMax, 0.0, yMax, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
}


/* -- other functions ------------------------------------------------------- */

void
init()
{
    /*
     * set parameters including the numbers of asteroids and photons present,
     * the maximum velocity of the ship, the velocity of the laser shots, the
     * ship's coordinates and velocity, etc.
     */

    /** initialize ship **/
    ship.x = 50.0;
    ship.y = 50.0;
    ship.dx = 0.0;
    ship.dy = 0.0;
    ship.phi = 90.0;
    ship.radius = 2.5;
    ship.acceleration = 0.00002;
    ship.turnSpeed = 2.6;
}

void
initAsteroid(
	Asteroid *a,
	double x, double y, double size)
{
    /*
     *	generate an asteroid at the given position; velocity, rotational
     *	velocity, and shape are generated randomly; size serves as a scale
     *	parameter that allows generating asteroids of different sizes; feel
     *	free to adjust the parameters according to your needs
     */

    double	theta, r;
    int		i;
        
    a->x = x;
    a->y = y;
    a->phi = 0.0;
    a->dx = myRandom(-0.8, 0.8);
    a->dy = myRandom(-0.8, 0.8);
    a->dphi = myRandom(-0.2, 0.2);
    
    a->nVertices = 6+rand()%(MAX_VERTICES-6);
    
    for (i=0; i<a->nVertices; i++) {
	   theta = 2.0*M_PI*i/a->nVertices;
	   r = size*myRandom(2.0, 3.0);
	   a->coords[i].x = -r*sin(theta);
	   a->coords[i].y = r*cos(theta);
    }
    
    a->active = 1;
}

void
drawShip(Ship *s) {

    float xVertex, yVertex;
    float t1 = (0.0f+s->phi) * DEG2RAD, 
          t2 = (135.0f+s->phi) * DEG2RAD, 
          t3 = (225.0f+s->phi) * DEG2RAD; 

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_LINE_LOOP);
    xVertex = s->x + (s->radius * cos(t1));
    yVertex = s->y + (s->radius * sin(t1));
    glVertex2f(xVertex, yVertex);

    xVertex = s->x + (s->radius * cos(t2));
    yVertex = s->y + (s->radius * sin(t2));
    glVertex2f(xVertex, yVertex);

    xVertex = s->x + (s->radius * cos(t3));
    yVertex = s->y + (s->radius * sin(t3));
    glVertex2f(xVertex, yVertex);

    glEnd();
}

void
drawPhoton(Photon *p)
{
}

void
drawAsteroid(Asteroid *a)
{
}


/* -- helper function ------------------------------------------------------- */

double
myRandom(double min, double max)
{
	double	d;
	
	/* return a random number uniformly draw from [min,max] */
	d = min+(max-min)*(rand()%0x7fff)/32767.0;
	
	return d;
}
