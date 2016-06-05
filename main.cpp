#include "stdafx.h"
#include "gui.h"

#include <GL\glut.h>

int main(int argc, char **argv)
{
	glutInit(&argc, argv);		                  /* initialize GLUT system */
	InitGUI();
	glutMainLoop();			                      /* start processing events... */

	/* execution never reaches this point */

	return 0;
}

