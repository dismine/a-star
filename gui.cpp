#include "stdafx.h"
#include "gui.h"
#include "global.h"

#include <GL\glut.h>

void DisplayCB(void)		      
{
	glClear(GL_COLOR_BUFFER_BIT); /* clear the display */

	//draw connections
	glColor3f(0.0, 1.0, 0.0); /* green connections */
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	for(unsigned int i = 0; i < edges.size(); ++i)
	{
		const int indx1 = edges[i].first;
		const int indx2 = edges[i].second;
		glVertex2i(coordinates[indx1].first, coordinates[indx1].second);
		glVertex2i(coordinates[indx2].first, coordinates[indx2].second);
	}
	glEnd();

	// draw vertices
	glColor3f(1.0, 1.0, 1.0);	  /* white points*/
	glPointSize(25.0f); //setting the size of the points
	glEnable(GL_POINT_SMOOTH); // smooth rectangles
	glBegin(GL_POINTS);
	for(unsigned int i = 0; i < coordinates.size(); ++i)
	{
		glVertex2i(coordinates[i].first, coordinates[i].second);
	}
	glEnd();

	glutSwapBuffers(); /* Complete any pending operations */
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); /* width=400pixels height=500pixels */
	glutCreateWindow("Test");	                     /* create window */
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glClearColor(0.0, 0.0, 0.0, 0.0);	             /* set background to black */
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);	 /* how object is mapped to window */
	glutDisplayFunc(DisplayCB);		                 /* set window's display callback */
	glutKeyboardFunc(KeyCB);		                 /* set window's key callback */
}

