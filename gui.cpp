#include "stdafx.h"
#include "gui.h"
#include "global.h"

#include <GL\glut.h>

void DisplayCB(void)		      
{
	glClear(GL_COLOR_BUFFER_BIT); /* clear the display */
	glColor3f(1.0, 1.0, 1.0);	  /* set current color to white */
	glBegin(GL_POLYGON);		  /* draw filled triangle */
	glVertex2i(200,125);		  /* specify each vertex of triangle */
	glVertex2i(100,375);
	glVertex2i(300,375);
	glEnd();				      /* OpenGL draws the filled triangle */
	glFlush();				      /* Complete any pending operations */
}

void KeyCB(unsigned char key, int x, int y)	/* called on key press */
{
	if( key == 'q' ) 
	{
		exit(0);
	}
}

void InitGUI()
{
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); /* width=400pixels height=500pixels */
	glutCreateWindow("Test");	                     /* create window */
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glClearColor(0.0, 0.0, 0.0, 0.0);	             /* set background to black */
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);	 /* how object is mapped to window */
	glutDisplayFunc(DisplayCB);		                 /* set window's display callback */
	glutKeyboardFunc(KeyCB);		                 /* set window's key callback */
}

