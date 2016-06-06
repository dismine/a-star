#include "stdafx.h"
#include "gui.h"
#include "parsing.h"
#include "global.h"

#include <GL\glut.h>
#include <iostream>

int main(int argc, char *argv[])
{
	if (!Parse(argc, argv))
	{
		std::cout << "Parsing error.\n";
		std::cin.get(); // Stop console from closing 
        exit(0);
	}

	// compile with: /DDEBUG
#ifdef DEBUG
	// Let's see a parsing result
	std::cout << "Number of points " << coordinates.size() << ".\n";
	for (unsigned int i = 0; i < coordinates.size() ; ++i)
	{
		std::cout << "Point" << i << " (" << coordinates[i].first << "," << coordinates[i].second << ").\n";
	}

	std::cout << "Connections " << edges.size() << ".\n";
	for (unsigned int i = 0; i < edges.size() ; ++i)
	{
		const int indx1 = edges[i].first;
		const int indx2 = edges[i].second;

		std::cout << "Vertex" << indx1 << " to Vertex" << indx2 << " = " << map[indx1][indx2] << ".\n";
	}
#endif

	glutInit(&argc, argv);		                  /* initialize GLUT system */
	InitGUI();
	glutMainLoop();			                      /* start processing events... */

	/* execution never reaches this point */

	return 0;
}

