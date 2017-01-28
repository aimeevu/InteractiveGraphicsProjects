/*
Thao-Nguyen Vu | CSC4820: Interactive Graphics | Friday 1:00PM - 4:25PM | Fall 2016
This program displays the basics of drawing and transforming 3D geometry objects (triangle and cube) as well as 
the specification of a camera and its projection.

Majority of the code was provided by Ying Zhu from his class website. It was pieced together in order to display
the triangle object and cube object (that was hardcoded in) in one window. The transformations that were done on
the objects where translation, rotation, and scaling. The cube needed to appear under the triangle, and both of 
the objects needed to be centered on the screen. The vertex data for the two objects were changed in order to
accomplish the goals of this project.
*/

//Includes the neccessary pathways to the libraries needed to run the program
#include <stdlib.h>
#include <GL/glew.h> 
#include <GL/freeglut.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>  // Transformation related functions
#include <glm/gtc/type_ptr.hpp> 

//Defining the namespace to prevent any complications with similar code in another file
using namespace glm;

#define BUFFER_OFFSET(offset) ((GLvoid *) offset) 
enum { Vertices, Indices, NumBuffers }; //creating enumerated types

// GLuint is a typedef unsigned binary int specific to OpenGL data types
GLuint vertexArrayBufferID = 0;//the ID of the vertex array buffer
GLuint program;//program ID for the shader
GLuint vPos;//vertex position stored in the ID of the vertex attribute variable.
GLuint mvpMatrixID;//ID of the uniform variable that stores the model-view-projection matrix
mat4 modelMatrix, viewMatrix, projMatrix; // 4x4 identity matrices
GLfloat x = .5; //declares x
GLuint buffers[NumBuffers]; //array of buffers
GLintptr vertexOffset; //declares offset variable

// Stores the data of the vertexes for the cube into an array
struct VertexData {
	GLfloat vertex[3];

	VertexData(GLfloat x, GLfloat y, GLfloat z) {
		vertex[0] = x; vertex[1] = y; vertex[2] = z;
	}
};

//Function to create the cube object
void cube() {
	// Vertex data stored into the array
	VertexData vertexData[] = {
		VertexData(-0.25, -0.5, -0.25), /* Index 0 */
		VertexData(-0.25, -0.5, 0.25), /* Index 1 */
		VertexData(-0.25, 0, -0.25), /* Index 2 */
		VertexData(-0.25, 0, 0.25), /* Index 3 */
		VertexData(0.25, -0.5, -0.25), /* Index 4 */
		VertexData(0.25, -0.5, 0.25), /* Index 5 */
		VertexData(0.25, 0, -0.25), /* Index 6 */
		VertexData(0.25, 0, 0.25), /* Index 7 */
	};

	// Indices for the difference faces of the cube in the positive and negative directions
	GLubyte indices[] = {
		4, 5, 7,
		4, 7, 6,
		0, 2, 3,
		0, 3, 1,
		2, 6, 7,
		2, 7, 3,
		0, 1, 5,
		0, 5, 4,
		0, 4, 6,
		0, 6, 2,
		1, 3, 7,
		1, 7, 5
	};
	vertexOffset = 0;

	// Generates an unused buffer object name and specifies the array 
	// created above to store the buffer object names
	glGenBuffers(NumBuffers, buffers);

	// Binds the buffer objects from the array to the target GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices]);

	/*
	Creates storage for the buffer objects while deleting pre-existing data.
	Parameter 1: Target where the buffer object is bound to
	Parameter 2: Size of the memory that is allocated on the graphics card for each vertex
	Parameter 3: Points to the vertex data that will be stored
	Parameter 4: Specifies how the data will be used, in this case it's for the GL Drawing command
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	//Does the same thing as the previous two lines of code for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

//Function to create the triangle object
void triangle() {

	// Vertices of the triangle (x, y, z)
	GLfloat vertices[][4] = {
		{ -0.5, 0, 0.0, 1.0 },
		{ 0.5, 0, 0.0, 1.0 },
		{ 0.0, 1, 0.0, 1.0 }
	};

	// Generates an unused buffer object name and specifies the array buffer IDs
	glGenBuffers(1, &vertexArrayBufferID);

	// Since this is the first time the buffer object appears in triangle(), it must be created.
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBufferID);

	/* 
	Creates storage for the buffer objects while deleting pre-existing data. 
	Parameter 1: Target where the buffer object is bound to
	Parameter 2: Size of the memory that is allocated on the graphics card for each vertex
	Parameter 3: Points to the vertex data that will be stored
	Parameter 4: Specifies how the data will be used, in this case it's for the GL Drawing command
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// OpenGL vertex shader source code
	const char* vSource = {
		"#version 330\n"
		"in vec4 vPos;"
		"uniform mat4 mvp_matrix;"
		"void main() {"
		"	gl_Position = mvp_matrix * vPos;"
		"}"
	};

	// OpenGL fragment shader source code
	const char* fSource = {
		"#version 330\n"
		"out vec4 fragColor;"
		"void main() {"
		"	fragColor = vec4(1, 0, 1, 1.0);"
		"}"
	};

	GLuint vShader, fShader; //shader IDs

	// Create empty shader objects
	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Attach shader source code the shader objects
	glShaderSource(vShader, 1, &vSource, NULL);
	glShaderSource(fShader, 1, &fSource, NULL);

	// Compile shader objects
	glCompileShader(vShader);
	glCompileShader(fShader);

	// Create an empty shader program object
	program = glCreateProgram();

	// Attach vertex and fragment shaders to the shader program
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	// Link the shader program
	glLinkProgram(program);

	// Retrieves the ID of the vertex attribute variable and the uniform matrix variable and returns the index of
	// the variable's attribute 
	vPos = glGetAttribLocation(program, "vPos");
	mvpMatrixID = glGetUniformLocation(program, "mvp_matrix");
}

// Initialize data arrays and shaders
void init()
{
	cube(); // Calls the cube function to draw the cube
	triangle(); // Calls the triangle function to draw the triangle

	glEnable(GL_DEPTH_TEST);//enables depth test

	glClearColor(0, 0, 0, 1);//background color
}

// Handles how the objects look when reshaped from the camera's perspective
void reshape(int width, int height)
{
	/* 
	This line is handled by the hardware. It specifies the size of the content inside the window
	and creates a viewport matrix that is inserted into the graphics pipeline.
	*/
	glViewport(0, 0, width, height);

	/*
	The projection matrix created here is essentially the camera's perspective.
	Parameter 1: Field of view
	Parameter 2: Window aspect ratio (relationship between width and height)
	Parameter 3: Near clipping plane
	Parameter 4: Far clipping plane
	*/
	projMatrix = perspective(radians(60.0f), (float)width / (float)height, 0.1f, 5000.0f);

	/*
	This view matrix stores the data from the location and orientation of the camera.
	Parameter 1: Location of camera
	Parameter 2: Location of where the camera is pointing
	Parameter 3: Up vector of camera
	*/
	viewMatrix = lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}

// Handles what is displayed
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears buffers, values are reset to preset values

	glUseProgram(program); //installs program to use while rendering

	/*
	The following code handles all of the transformations of the vertices: scaling, translation, and rotation.
	*/
	mat4 scaleMatrix = scale(mat4(1.0f), vec3(0.8f));
	mat4 translateMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	mat4 rotationMatrixX = rotate(mat4(1.0f), radians(5.0f), vec3(1.0f, 0.0f, 0.0f));
	mat4 rotationMatrixY = rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));

	// Combines all of the transformations and returns one final answer for the location of each vertex
	modelMatrix = translateMatrix * rotationMatrixY * rotationMatrixX * scaleMatrix;

	// Takes the data from the three matrices, determines where the projection, view, and
	// model vertex data is located, and projects the data onto the screen. 
	mat4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;

	/*
	This function transfers the MVP Matrix to the graphics memory. To prevent multiple calculations, the matrix is fed
	into the vertex shader. 
	Parameter 1: Location of the uniform value
	Parameter 2: Number of matrices to be modified (usually left at 1)
	Parameter 3: Whether or not to transpose (in this case, it does not)
	Parameter 4: Points to the array that will update the uniform value from first parameter
	*/
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, value_ptr(mvpMatrix));

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBufferID);//activates the buffer that already exists

	/*
	This function defines an array of vertex attribute data used during rendering of the triangle
	Parameter 1: Vector position defines the vertex array through which the shader program object
					and buffer object is connected.
	Parameter 2: Number of componenets per attribute
	Parameter 3: Data type of array
	Parameter 4: Whether or not the fixed-point data value is normalized (no, in this case)
	Parameter 5: Byte offset of attributes
	Parameter 6: Offset of attribute stored
	*/
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(vPos);//enables vPos as the vertex array that the shader program will look at

	glDrawArrays(GL_TRIANGLES, 0, 3); //starts shader to draw the triangle

	glBindBuffer(GL_ARRAY_BUFFER, 0); //disables buffer

	glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices]); //binds the vertices to the buffer

	//This function defines an array of vertex attribute data used during rendering of the cube
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Indices]); //binds the indices to the buffer
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)); //starts shader to draw cube

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//disables buffer

	glutSwapBuffers();// swaps the front and back buffers to refresh the window
}

void main(int argc, char *argv[])
{
	glutInit(&argc, argv);//initializes Freeglut
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //initializes window display
	glutCreateWindow("Thao-Nguyen Vu - Project 1"); //creates window with the bar
	glutReshapeWindow(400, 400); //default size and allows to reshape the window

	glewInit(); //initializes Glew

	init(); //transfers data from CPU to GPU
	glutDisplayFunc(display);//registers display function
	glutReshapeFunc(reshape);//registers reshape function
	glutMainLoop(); //starts the main event loop
}