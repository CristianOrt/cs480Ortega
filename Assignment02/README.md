An example of intercations with an object
========================================

Building This Example
---------------------

*This example requires GLM*
*On ubuntu it can be installed with this command*

>$ sudo apt-get install libglm-dev

*On a Mac you can install GLM with this command(using homebrew)*
>$ brew install glm

To build this example just 

>$ cd build
>$ make

*If you are using a Mac you will need to edit the makefile in the build directory*

The excutable will be put in bin

Left click = reverse spinning of the cube.
any letter key = reverse rotation. Rotate in the opposite direction in the circle.
esc key = exits the program.
Right click menu - "Stop Rotation" will stop the rotation of the cube moving in the circle
		   but it doesn't stop the spinning which can still be reversed when it isn't
		   moving in a circle. 
		 - "Start Rotation" starts the square to move in the direction it was before 
		   unless it was altered.
		 - "Quit" exits the program.
