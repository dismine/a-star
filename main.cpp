#include "stdafx.h"
#include "gui.h"

#include <GL\glut.h>
#include <iostream>


int main(int argc, char **argv)
{
	if (argc < 2) 
	{ // Check the value of argc. If not enough parameters have been passed, inform user and exit.
        std::cout << "Please, provide file with graph description.\n"; // Inform the user of how to use the program
        std::cin.get(); // Stop console from closing 
        exit(0);
	}

	glutInit(&argc, argv);		                  /* initialize GLUT system */
	InitGUI();
	glutMainLoop();			                      /* start processing events... */

	/* execution never reaches this point */

	return 0;
}

