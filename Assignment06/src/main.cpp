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
#include <SOIL/SOIL.h>
#include <assimp/Importer.hpp> //includes the importer, which is used to read our obj file
#include <assimp/scene.h> //includes the aiScene object
#include <assimp/postprocess.h> //includes the postprocessing variables for the importer
#include <assimp/color4.h> //includes the aiColor4 object, which is used to handle the colors from the mesh objects

//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat texture[2];
};

//--Evil Global variables
//Just for this example!
/////CHANGE FILE NAME HERE////////////////////////////////////////
const char * path = "capsule.obj";
int w = 640, h = 480;// Window size
static bool rotation = true;
static bool spin = true;
static bool rotate = true;
std::vector<unsigned int> outIndices;
GLuint program;// The GLSL program handle
GLuint elementBuffer;
GLuint vertexBuffer;
GLuint uvBuffer;
GLuint normalBuffer;
GLuint textureID;
GLuint textureSamplerUniform;


//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_texture;

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerUniform,0);
    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glDrawElements(GL_TRIANGLES, outIndices.size(), GL_UNSIGNED_INT, (void*)0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
                           
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

bool initialize()
{
   
   textureID = SOIL_load_OGL_texture
	(
		"capsule.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, 0);
	if(textureID == 0)
		printf("SOIL Loading Error: %s\n", SOIL_last_result());
   
    textureSamplerUniform = glGetUniformLocation

(program,"textureSampler");
    std::vector<float> outVertices;
    std::vector<float> outUVs;
    std::vector<float> outNormals;
   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
   aiMesh* meshPtr = scene->mMeshes[0];

   int numFaces = meshPtr ->mNumFaces;
   int numIndices = numFaces * 3;
   outIndices.resize(numIndices);
   
   for(unsigned int i = 0; i < meshPtr->mNumFaces; ++i)
	{
	const aiFace &Face = meshPtr->mFaces[i];
	assert(Face.mNumIndices == 3);
	outIndices[i*3+0] = Face.mIndices[0];
	outIndices[i*3+1] = Face.mIndices[1];
	outIndices[i*3+2] = Face.mIndices[2];
	}

    int numVertices = meshPtr->mNumVertices;
    outVertices.resize(numVertices * 3);
    outNormals.resize(numVertices * 3);
    outUVs.resize(numVertices*2);
    for(unsigned int i =0; i< meshPtr->mNumVertices; ++i){
	if(meshPtr->HasPositions()){
		outVertices[i*3+0] = meshPtr->mVertices[i].x;
		outVertices[i*3+1] = meshPtr->mVertices[i].y;
		outVertices[i*3+2] = meshPtr->mVertices[i].z;
	}

	if (meshPtr->HasNormals()) {
            outNormals[i * 3 + 0] = meshPtr->mNormals[i].x;
            outNormals[i * 3 + 1] = meshPtr->mNormals[i].x;
            outNormals[i * 3 + 2] = meshPtr->mNormals[i].x;
        }

        if (meshPtr->HasTextureCoords(0)) {
            outUVs[i * 2 + 0] = meshPtr->mTextureCoords[0][i].x;
            outUVs[i * 2 + 1] = meshPtr->mTextureCoords[0][i].y;
        }
    }
    
    
    // Create a Vertex Buffer object to store this vertex info on the GPU
    
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, outIndices.size() * sizeof(unsigned int), &outIndices[0], GL_STATIC_DRAW);

    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, outVertices.size() * sizeof(aiVector3D), &outVertices[0], GL_STATIC_DRAW);

    
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, outUVs.size() * sizeof(aiVector2D), &outUVs[0], GL_STATIC_DRAW);

    
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, outNormals.size() * sizeof(aiVector3D), &outNormals[0], GL_STATIC_DRAW);

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
/*
    loc_texture = glGetAttribLocation(program,
                    const_cast<const char*>("v_texture"));
    if(loc_texture == -1)
    {
        std::cerr << "[F] V_TEXTURE NOT FOUND" << std::endl;
        return false;
    }
*/
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
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
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
