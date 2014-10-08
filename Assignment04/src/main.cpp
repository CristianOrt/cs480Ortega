#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier


//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

//--Evil Global variables
//Just for this example!
/////CHANGE FILE NAME HERE////////////////////////////////////////
const char * path = "final_table.obj";
int w = 640, h = 480;// Window size
static bool rotation = true;
static bool spin = true;
static bool rotate = true;
static int numFaces = 0;
GLuint program;// The GLSL program handle
GLuint vbo_geometry;// VBO handle for our geometry

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_color;

//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection

//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void demo_menu(int id);
void myMouse(int button, int state, int x, int y);
bool loadOBJ(const char * path, std::vector <glm::vec3> &out_vertices);

//--Resource management
bool initialize();
void cleanUp();

//--Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;


//--Main
int main(int argc, char **argv)
{
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Matrix Example");
    glutCreateMenu(demo_menu);
    glutAddMenuEntry("Quit", 1);
    glutAddMenuEntry("Start Rotation", 2);
    glutAddMenuEntry("Stop Rotation", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input
    glutMouseFunc(myMouse);// called if there is a mouse input

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if(init)
    {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}

//--Implementations
void render()
{
    //--Render the scene

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    mvp = projection * view * model;

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(glm::vec3) * 2,//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(glm::vec3) * 2,
                           (void*)sizeof(glm::vec3));

    glDrawArrays(GL_TRIANGLES, 0, numFaces * 3);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);
                           
    //swap the buffers
    glutSwapBuffers();
}

void update()
{
    //total time
    static float angle = 0.0;
    static float dist = 0.0;
    int one = 1;
    int two = 1;
    float dt = getDT();// if you have anything moving, use dt.


    if(spin == false){ one = -1;}
    if(rotate == false){two = -1;}

    if(rotation == true){
	angle += dt * two*M_PI/2; //move through 90 degrees a second
    dist  += dt * one*M_PI/2;
    model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle)));
    model = glm::rotate(model, dist, glm::vec3(0.0, 1.0, 0.0));
    }
    if(rotation == false){
    dist += dt * one * M_PI/2;
    model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle)));
    model = glm::rotate(model, dist, glm::vec3(0.0, 1.0, 0.0));
    }
    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}


void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;
    //Change the viewport to be correct
    glViewport( 0, 0, w, h);
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }
    if(key > 31 && key < 127){rotate = !rotate;}
}
void demo_menu(int id){
	switch(id)
	{
	   case 1:
		exit(0);
		break;
	   
	   case 2:
		if(rotation == false){
 		
		rotation = true;
		update();
		}
	  	break;

	   case 3:
		if(rotation == true){
		
		rotation = false;
		update();
		}
		break;
	 
	}
	glutPostRedisplay();	
}

void myMouse(int button, int state, int x, int y){
if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
	spin = !spin;
	}
}

bool loadOBJ(const char * path, std::vector <glm::vec3> &out_vertices)
{
 std::vector<unsigned int> vertexIndices;
 std::vector<glm::vec3> temp_vertices;
 
 FILE * file = fopen(path, "r");
 if(file == NULL){
 	printf("can't open file \n");
	return false;
 }
 while(1){
 	char lineHeader[128];
	//read the first word of the line
	int res = fscanf(file, "%s", lineHeader);
	if(res == EOF){
		break;//End of file
	}
	//else : parse lineHeader
	if(strcmp(lineHeader,"v") == 0){
	glm::vec3 vertex;
	fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
	temp_vertices.push_back(vertex);
	}
	else if(strcmp(lineHeader, "f") == 0){
		unsigned int vertexIndex[3];
		fscanf(file, " %d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
	vertexIndices.push_back(vertexIndex[0]);
	vertexIndices.push_back(vertexIndex[1]);
	vertexIndices.push_back(vertexIndex[2]);
	numFaces++;
	}
 }
 //for each vertex of each triangle
 for(unsigned int i=0; i<vertexIndices.size(); i++){
	unsigned int vertexIndex = vertexIndices[i];
	glm::vec3 vertex = temp_vertices[vertexIndex-1];
	glm::vec3 color = glm::vec3(1.0,1.0,0.0);
	out_vertices.push_back(vertex);
	out_vertices.push_back(color);
	}
 return true;
	
}
bool initialize()
{
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    std::vector<glm::vec3> vertices;

    bool flag;
    flag = loadOBJ(path, vertices);
    if(!flag){
	std::cerr << "ERROR IN FILE" <<	 std::endl;
	}

    
    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    //--Geometry done

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //Shader Sources
    // Put these into files and write a loader in the future
    // Note the added uniform!

    std::string line,temp;
   std::ifstream fin;
   fin.open("vs.vert");
   getline(fin, line);
    while(fin.good()){
   getline(fin, temp);
   line = line + temp;
   }
   
   const char * vs;
   vs = line.c_str();
    // read data as a block:

    fin.close();
    fin.clear();
std::string sent, set;
fin.open("fs.frag");
   getline(fin, sent);
    while(fin.good()){
   getline(fin, set);
   sent = sent + set;
   }
    const char *fs;
    fs = sent.c_str();
         fin.close();
         fin.clear();

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        return false;
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    //check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if(loc_color == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }
    
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry);
}

//returns the time delta
float getDT()
{
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}