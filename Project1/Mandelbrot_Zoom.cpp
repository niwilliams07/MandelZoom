/*--------------------------------------------------------*/
/*  CS-378        Computer Graphics     Nicholas Williams */
/*--------------------------------------------------------*/
/*                 Mandelbrot Zoom                        */
/*--------------------------------------------------------*/

//Favorite input
// -0.262856 -0.254055 0.652478 0.661279 800 800

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>
#include "Complex.h"
#include "Storage.h"
#include <iostream>
#include <vector>

using namespace std;

//width of window
double WIDTH;
//height of window
double HEIGHT;
//window x position
int initial_win_x = 150;
//window y position
int initial_win_y = 50;
//intial mandelbrot set
double x_min;
double x_max;
double y_min;
double y_max;
int MAX_ITERATIONS = 1000;
vector <Storage> Stack;
int index;
int stackCount = 0;
int xAnchor, yAnchor, xStretch, yStretch;
bool rubberBanding = false;
float red = 0.0;
float blue = 0.0;
float green = 0.0;
int indexCount = 0;


//Takes a pixel coordinate and transforms it to the complex plane
Complex complexCalculation(int u, int v) {
	double x, y;
	x = x_min + (u * ((x_max - x_min)
		/ (WIDTH - 1)));
	y = y_min + (v * ((y_max - y_min)
		/ (HEIGHT - 1)));
	return Complex(x, y);
}

void pixelColor(int i) {
	float half = MAX_ITERATIONS / 2.0;
	float r = 0.0;
	float g = 0.0;
	float b = 0.0;
	if (i < half) {
		r = i / half;
	}
	else {
		r = 1.0;
		g =b = (i - half) / half;
	}
	if (i == MAX_ITERATIONS) {
		red = 0.0;
		green = 0.0;
		blue = 0.0;
	}
	else {
		red = r;
		green = g;
		//blue = b;
	}
}

//iterates the mandelbrot equation over the complex number
int countIterations(Complex s) {
	Complex z = Complex(0, 0);
	int count = 0;
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		z = z * z + s;
		if ((z.x * z.x) + (z.y * z.y) > 4.0) {
			break;
		}
		count++;
	}
	return count;
}

//takes the pixel coordinate and color to color the pixel
void displayPixel(int u, int v) {
	glColor3f(red, green, blue);
	glBegin(GL_POINTS);
	glVertex2i(u, v);
	glEnd();
}

//computes and draws the mandelbrot set for every pixel in the window
void mandelbrot() {

	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	for (int u = 0; u < WIDTH; u++) {
		for (int v = 0; v < HEIGHT; v++) {
			//will take a pixel and make it a complex number
			Complex s = complexCalculation(u, v);
			//will iterate over the complex number using the mandelbrot equation
			int i = countIterations(s);
			pixelColor(i);
			displayPixel(u, v);
		}
	}
	glFlush();
}

void Pop() {
	//zoom out
	 if (index > 0) {
		Storage view = Stack.at(index-1);
		x_min = view.X1;
		x_max = view.X2;
		y_min = view.Y1;
		y_max = view.Y2;
		index--;
		glutPostRedisplay();
	}
}

void Push() {
	//zoom in

	if (index < stackCount) {
		Storage view = Stack.at(index+1);
		x_min = view.X1;
		x_max = view.X2;
		y_min = view.Y1;
		y_max = view.Y2;
		index++;
		glutPostRedisplay();
	}
}

void drawRect(int xOld, int yOld, int xNew, int yNew)
{
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINE_LOOP);
	glVertex2i(xOld, yOld); 
	glVertex2i(xNew, yOld);
	glVertex2i(xNew, yNew); 
	glVertex2i(xOld, yNew);
	glEnd();
	glFlush();
}

void drawRubberBand(int xA, int yA, int xS, int yS)
{
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glBegin(GL_LINE_LOOP);
	glVertex2i(xA, yA); 
	glVertex2i(xS, yA);
	glVertex2i(xS, yS); 
	glVertex2i(xA, yS);
	glEnd();
	glDisable(GL_COLOR_LOGIC_OP);
	glFlush();
}

void rubberBand(int x, int y)
// Callback for processing mouse motion.
{
	if (rubberBanding)
	{
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		y = HEIGHT - y;
		xStretch = x;
		yStretch = y;
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		glFlush();
	}
}


void reshape(int w, int h)
// Callback for processing reshape events.
{
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}

//exits the window
void escExit(GLubyte key, int, int)
// Callback for processing keyboard events.
{
	if (key == 27 /* ESC */) std::exit(0);
}

//callsback the menu
void mainMenu(int item)
{
	switch (item)
	{
	case 1: Push(); break;
	case 2: Pop(); break;
	case 3: std::exit(0);
	}
}


//Sets the menu items
void setMenus()
{
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Push", 1);
	glutAddMenuEntry("Pop", 2);
	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processLeftDown(int x, int y)
{
	if (!rubberBanding)
	{
		int xNew = x;
		int yNew = HEIGHT - y;
		xAnchor = xNew;
		yAnchor = yNew;
		xStretch = xNew;
		yStretch = yNew;
		rubberBanding = true;
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
	}
}
void processLeftUp(int x, int y)
{
	if (rubberBanding)
	{
		Complex temp(0,0);
		int xNew, yNew;
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		rubberBanding = false;
		xNew = x;
		yNew = HEIGHT - y;
		drawRubberBand(xAnchor, yAnchor, xNew, yNew);
		Complex x1y1 = complexCalculation(xAnchor, yAnchor);
		Complex x2y2 = complexCalculation(xNew, yNew);

		if (xAnchor > xNew) {
			temp = x1y1;
			x1y1 = x2y2;
			x2y2 = temp;
		}

		double xd = x2y2.x - x1y1.x;
		double yd = x2y2.y - x1y1.y;
		double Ar = yd / xd;
		double xs = x_max - x_min;
		double ys = y_max - y_min;
		double Aw = ys / xs;
		
		if (Ar == Aw) {
			x_min = x1y1.x;
			x_max = x2y2.x;
			y_min = x1y1.y;
			y_max = x2y2.y;
		}

		if (Ar > Aw) {
			double i = (((Ar / Aw) - 1) / 2) * xd;
			x_min = x1y1.x - i;
			x_max = x2y2.x + i;
			y_min = x1y1.y;
			y_max = x2y2.y;
		}

		if (Ar < Aw) {
			double i = (((Aw / Ar) - 1) / 2) * yd;
			x_min = x1y1.x;
			x_max = x2y2.x;
			y_min = x1y1.y - i;
			y_max = x2y2.y + i;
		}

		Storage New(x_min, x_max, y_min, y_max);
		index++;
		stackCount++;
		while (index < stackCount) {
			Stack.pop_back();
			stackCount--;
		}
		Stack.push_back(New);
		glutPostRedisplay();
		glFlush();
	}
}


//mouse function that takes in the state of the mouse and coordinate
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		switch (state)
		{
		case GLUT_DOWN: processLeftDown(x, y); break;
		case GLUT_UP: processLeftUp(x, y); break;
		}
}


int main(int argc, char* argv[])
{	//you need to put in command line arguments for
	//xmin,xmax,ymin,ymax,initial width, and initial height
	x_min = atof(argv[1]);
	x_max = atof(argv[2]);
	y_min = atof(argv[3]);
	y_max = atof(argv[4]);
	WIDTH = atoi(argv[5]);
	HEIGHT = atoi(argv[6]);
	Storage initial(x_min, x_max, y_min, y_max);
	Stack.push_back(initial);
	index = 0;

	// Mask floating point exceptions.
	_control87(MCW_EM, MCW_EM);

	// Choose RGB display mode for normal screen window.
	glutInitDisplayMode(GLUT_RGB);

	// Set initial window size, position, and title.
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(initial_win_x, initial_win_y);
	glutCreateWindow("Mandelbrot Zoom - Williams");

	// You don't (yet) want to know what this does.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)WIDTH, 0.0, (double)HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.375, 0.375, 0.0);

	// Set the callbacks for the normal screen window.
	glutDisplayFunc(mandelbrot);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(escExit);
	glutMotionFunc(rubberBand);
	glutPassiveMotionFunc(rubberBand);

	// Set the color for clearing the window.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// Set up the menus.
	setMenus();

	// Start the GLUT main loop.
	glutMainLoop();

}