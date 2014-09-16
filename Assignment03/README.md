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

Earth is the BIG Cube
Moon is the SMALL Cube
Left click = reverse spinning of the  Earth cube.
'M' or 'm' letter key = reverse rotation of the Moon. Rotate in the opposite direction in the circle.
'E' or 'e' letter key = reverse rotation of the Earth. Rotate in the opposite direction in the circle.
esc key = exits the program.
Right click menu - "Stop Earth Rotation" will stop the rotation of the Earth moving in the circle
		   but it doesn't stop the spinning which can still be reversed when it isn't
		   moving in a circle. 
		 - "Start Earth Rotation" starts the Earth to move in the direction it was before 
		   unless it was altered.
    		 - "Stop Moon Rotation" will stop the rotation of the Moon moving in the circle
		   but it doesn't stop the spinning which can still be reversed when it isn't
		   moving in a circle. 
		- "Start Moon Rotation" starts the Moon to move in the direction it was before 
		   unless it was altered.
		- "Change Moon Spin" makes the Moon spin in the other direction
		 - "Quit" exits the program.
