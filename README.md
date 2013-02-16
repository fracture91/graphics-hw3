Andrew Hurle

CS 4731 - Computer Graphics

Homework 3


Renders five Lindenmayer systems defined in the lsystems directory.
Can cycle though the five systems with 'a', 'b', 'c', 'd', 'e', and show
all of them at random positions with 'f'.  In "forest" mode, the cow and
car meshes are also drawn.

The program is linked against whatever files are present on the machine.
The Zoo Lab machines contain the needed library files, so they're not
included here.

Running
=====

To compile and run on Linux:

1. `make && ./hw3`


To compile and run on Windows (Zoo Lab machines, tested on FLA21-02):

1. Open "Visual Studio Command Prompt (2010)" from the start menu
2. `cd C:\wherever\these\files\are`
3. `nmake /f Win-Makefile`
4. `hw3.exe`

Notes
=====

LSystemReader is responsible for pulling the system data out of files
and putting it into an LSystem instance.  This instance will iterate the
start string based on the grammar it is given.  The LSystem provides a
Turtle instance.  This Turtle can be given all of the commands in the
turtle string, and will modify a given transform matrix stack.
LSystemRenderer will actually give the commands to the turtle and draw
its progress to the screen as a PolyCylinder.  hw3.cpp hooks everything
up with the standard GLUT callbacks.

The camera position is in the +x/+y/+z octant, looking to -x/-y/-z, so x
and z axes both point out of the screen.

None of the trees are scaled in any way, so some are rather small
compared to others.

