#include "stdafx.h"
#include "gui.h"
#include "global.h"
#include "astar.h"

#include <GL\glut.h>
#include <iostream>
#include <algorithm>

using std::find;

static const GLfloat PICK_TOL = 25.0f;  /* picking tolerance in pixels */
static const int PICK_BUFFER_SIZE = 256; /* how big to make the pick buffer */
static const GLfloat LINE_WIDTH = 3.0f;

static GLenum renderMode = GL_RENDER; /* GL_RENDER or GL_SELECT */
static int xMouse = 0;
static int yMouse = 0;

static vector<int> conditions; // start and end points
static vector<pair<int, int>> path; // path for a start point to end point

static void DrawGraph(GLenum mode)
{
	if(mode != GL_SELECT)
	{// In select mode we don't need lines
		//draw connections
		glColor3f(0.0, 1.0, 0.0); /* green connections */
		glLineWidth(LINE_WIDTH);
	
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

	if(mode != GL_SELECT)
	{
		//draw path
		glColor3f(1.0, 0.0, 0.0); 
		glLineWidth(LINE_WIDTH);
	
		glBegin(GL_LINES);
		for(unsigned int i = 0; i < path.size(); ++i)
		{
			const int indx1 = path[i].first;
			const int indx2 = path[i].second;
			glVertex2i(coordinates[indx1].first, coordinates[indx1].second);
			glVertex2i(coordinates[indx2].first, coordinates[indx2].second);
		}
		glEnd();

		// draw start and end
		glColor3f(1.0, 1.0, 0.0);	  /* yellow points*/
		glEnable(GL_POINT_SMOOTH); // smooth rectangles
	
		for(unsigned int i = 0; i < conditions.size(); ++i)
		{
			const int indx = conditions[i];
			glBegin(GL_POINTS);
			glVertex2i(coordinates[indx].first, coordinates[indx].second);
			glEnd();
		}
	}
}

static void PrintText(int x, int y, const std::string &string)
{
	//(x, y) is from the bottom left of the window
    glRasterPos2i(x, y);
    for (unsigned int i = 0; i < string.size(); ++i)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
}

static void DisplayCB(void)		      
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

	if(renderMode != GL_SELECT)
	{
		const int x = 10;
		const int y = 10;
		switch(conditions.size())
		{
		case 0:
			PrintText(x, y, std::string("Select a vertex"));
			break;
		case 1:
			PrintText(x, y, std::string("Select the finish vertex"));
			break;
		case 2:
			PrintText(x, y, std::string("Select the start vertex to try again"));
			break;
		default:
			break;
		}
	}

	DrawGraph(renderMode);

	/* swap the double-buffered framebuffers */
	if (renderMode == GL_RENDER)
	{
		glutSwapBuffers();
	}
}

static void KeyCB(unsigned char key, int x, int y)	/* called on key press */
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

static void ReshapeCB(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/*  ProcessHits process the contents of the selection array. */
static void ProcessHits (GLint hits, GLuint buffer[])
{
#ifdef DEBUG
	std::cout << "Pick hits = " << hits << ".\n";
#endif

	GLint *ptr = (GLint *) buffer; 
	if (hits > 0) 
	{
		ptr+=3; /* Skeep the first three element of picking array*/

		// We always will take the first in the list
		#ifdef DEBUG
			std::cout << "Was picked vertex = " << *ptr << ".\n";
		#endif

		if (conditions.size() == 2)
		{
			conditions.clear();
		}

		if(find(conditions.begin(), conditions.end(), *ptr) == conditions.end()) 
		{ // does not contain
			conditions.push_back(*ptr);

			conditions.size() == 2 ? path = PathFind(conditions[0], conditions[1]) : path.clear();
		}
	}
}

static void MouseCB(int button, int state, int x, int y)
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
	glutCreateWindow("A-star algorithm");	         /* create window */
	glClearColor(0.0, 0.0, 0.0, 0.0);	             /* set background to black */
	glutReshapeFunc(ReshapeCB);                      /* set window's reshape callback */
	glutDisplayFunc(DisplayCB);		                 /* set window's display callback */
	glutKeyboardFunc(KeyCB);		                 /* set window's key callback */
	glutMouseFunc (MouseCB);
}

