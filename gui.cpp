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
	switch (key) 
	{
      case 27: // ESC
         exit(0);
         break;
	  default:
		  break;
	}
}

void ReshapeCB(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void InitGUI()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); /* width=400pixels height=500pixels */
	glutCreateWindow("Test");	                     /* create window */
	glClearColor(0.0, 0.0, 0.0, 0.0);	             /* set background to black */
	glutReshapeFunc(ReshapeCB);                      /* set window's reshape callback */
	glutDisplayFunc(DisplayCB);		                 /* set window's display callback */
	glutKeyboardFunc(KeyCB);		                 /* set window's key callback */
}

