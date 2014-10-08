
Building This Example
---------------------

*This example requires GLM and assimp and SOIL*
*On ubuntu it can be installed with this command*

>$ sudo apt-get install libglm-dev
>$ sudo apt-get install libassimp-dev
>$ sudo apt-get install libsoil-dev

*On a Mac you can install GLM with this command(using homebrew)*
>$ brew install glm

To build this example just 

>$ cd build
>$ make

*If you are using a Mac you will need to edit the makefile in the build directory*

The excutable will be put in bin

My program should load "capsule" file and display it on the screen using the assimp loader along with the shader.
