The app allow to:
- read a simple graph description from file
- plot this graph on screen (using opengl/glut)
- allow user to select two vertices by clicking them with mouse
- find & plot shortest path (using A* algorithm) between selected vertices

The program assume that we don't have to take care about a degenerate cases like no connection between selected vertices.
The repository contains glut dlls.

The file format for graph description is:
<number_of_vertices>
<x> <y>
 ....  ....
<x> <y>
<number_of_connections>
<0_point_idx> <1_point_idx> <weight>
 ....  ....  ....
<2_point_idx> <1_point_idx> <weight>

Note. Point coordinates can go outside of window size. And weight should be more than 0.

An example of a file:
4
1 1
100 100
100 200
300 300
4
0 1 1
0 2 0.5
1 3 1
2 3 0.5