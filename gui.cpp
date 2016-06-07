#include "stdafx.h"
#include "gui.h"
#include "global.h"

#include <GL\glut.h>
#include <iostream>

const GLfloat PICK_TOL = 25.0f;  /* picking tolerance in pixels */
const int PICK_BUFFER_SIZE = 256; /* how big to make the pick buffer */

GLenum renderMode = GL_RENDER; /* GL_RENDER or GL_SELECT */
int xMouse = 0;
int yMouse = 0;

void DrawGraph(GLenum mode)
{
	if(mode != GL_SELECT)
	{// In select mode we don't need lines
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
	}

	// draw vertices
	glColor3f(1.0, 1.0, 1.0);	  /* white points*/
	glPointSize(PICK_TOL); //setting the size of the points
	glEnable(GL_POINT_SMOOTH); // smooth rectangles
	
	for(unsigned int i = 0; i < coordinates.size(); ++i)
	{
		glLoadName(i); //Calls to glLoadName() are ignored if you're not in selection mode.
		glBegin(GL_POINTS);
		glVertex2i(coordinates[i].first, coordinates[i].second);
		glEnd();
	}
}

void DisplayCB(void)		      
{
	GLint viewport[4]; /* place to retrieve the viewport numbers */

	glClear(GL_COLOR_BUFFER_BIT); /* clear the display */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (renderMode == GL_SELECT)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		glRenderMode(renderMode);
		glPushMatrix();
		gluPickMatrix((GLdouble) xMouse, (GLdouble) (viewport[3] - yMouse), PICK_TOL, PICK_TOL, viewport);

		/* init the picking buffer */
		glInitNames();
		glPushName(0xffffffff); /* a strange value */
	}

	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DrawGraph(renderMode);

	/* swap the double-buffered framebuffers */
	if (renderMode == GL_RENDER)
	{
		glutSwapBuffers();
	}
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

/*  ProcessHits process the contents of the selection array. */
void ProcessHits (GLint hits, GLuint buffer[])
{
#ifdef DEBUG
	std::cout << "Pick hits = " << hits << ".\n";
#endif

	GLint *ptr = (GLint *) buffer; 
	for (int i = 0; i < hits; ++i) 
	{
	/*  for each hit  */
		const GLint names = *ptr; /* hit found N objects*/
		ptr+=3; /* Skeep the first three element of picking array*/
		for (int j = 0; j < names; ++j) 
		{
			#ifdef DEBUG
				std::cout << "Was picked virtices = " << *ptr << ".\n";
			#endif
			ptr++;
		}
	}
}

void MouseCB(int button, int state, int x, int y)
{
	xMouse = x;
	yMouse = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		GLint hits;
		GLuint selectBuf[PICK_BUFFER_SIZE];

		renderMode = GL_SELECT;
		glSelectBuffer(PICK_BUFFER_SIZE, selectBuf); /* setup the picking buffer */
		glRenderMode(renderMode);
		DisplayCB();
		renderMode = GL_RENDER;
		hits = glRenderMode(renderMode);

		/* Some of the TNT2-based graphics cards have a picking bug. You need to 
		pick twice to get a pick to register correctly. You can either click 
		everything twice (pretty lame thing to have to do), or you can add these 
		lines. */
#ifdef BUG_KLUDGE
		if (hits == 0)
		{
			renderMode = GL_SELECT;
			glRenderMode(renderMode);
			DisplayCB();
			renderMode = GL_RENDER;
			hits = glRenderMode(renderMode);
		}
#endif

		ProcessHits (hits, selectBuf);
		glutPostRedisplay();
	}
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
	glutMouseFunc (MouseCB);
}

