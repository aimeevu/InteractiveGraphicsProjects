/*
Thao-Nguyen Vu | CSC4820: Interactive Graphics | Friday 1:00PM - 4:25PM | Fall 2016
This program demonstrates lighting and camera control based off of user input.

Majority of the code was provided by Ying Zhu from his class website. The program contains ambient lighting as well as a light source.

*/

// GLEW header
#include <GL/glew.h> // This must appear before freeglut.h

// Freeglut header
#include <GL/freeglut.h>

// GLM header files
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <iostream>

using namespace std;
using namespace glm;

#define BUFFER_OFFSET(offset) ((GLvoid *) offset)

// VBO buffer IDs
GLuint pyramidVertexArrayBufferID = 0;
GLuint pyramidNormalArrayBufferID = 0;
GLuint boxVertexArrayBufferID = 0;
GLuint boxElementArrayBufferID = 0;

GLuint colorID;

GLuint program; // shader program ID

// Shader variable IDs
GLint vPos; // position vertex attribute
GLint normalID; // normal vertex attribute

GLint mvpMatrixID; // uniform variable for model, view, projection matrix
GLint modelMatrixID; // uniform variable for model, view matrix
GLint normalMatrixID; // uniform variable for normal matrix for transforming normals
GLint lightSourcePositionID; // uniform variable position of the light source for lighting calculation
GLint diffuseLightProductID; // uniform variable diffuse light for lighting calculation
GLint ambientID; // ambient variable 
GLint attenuationAID; // attenuation variable
GLint attenuationBID;
GLint attenuationCID;

// Transformation matrices
mat4 projMatrix;
mat4 mvpMatrix;
mat4 modelMatrix;
mat4 viewMatrix;
mat3 normalMatrix;

// Light parameters
vec4 lightSourcePosition = vec4(0.0f, 4.0f, 0.0f, 1.0f);
vec4 diffuseMaterial = vec4(0.5f, 0.5f, 1.0f, 1.0f);
vec4 diffuseLightIntensity = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
float attenuationA = 1.0f;
float attenuationB = 0.2f;
float attenuationC = 0.0f;

vec4 diffuseLightProduct;

// Camera parameters
vec3 eyePosition = vec3(0.0f, 0.0f, 4.0f);
vec3 lookAtCenter = vec3(0.0f, 0.0f, 0.0f);
vec3 upVector = vec3(0.0f, 1.0f, 0.0f);
float fieldOfView = 30.0f;
float nearPlane = 0.1f;
float farPlane = 1000.0f;

// Keyboard controlled camera rotation
int pitchAngle = 0; // Camera pitch angle
int rotationStep = 5; // Control the speed of camera rotation

// X and Y position of camera
int xpos;
int ypos;

float rotateX = 0;
float rotateY = 0;

// Initialize vertex arrays and VBOs for the pyramid
void preparePyramidVBOs() {
	GLfloat vertices[][4] = {
		{ 1.0f, -1.0f, 1.0f, 1.0f }, // face 1
		{ -1.0f, -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f, 1.0f }, // face 2
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f }, // face 3
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f, 1.0f }, // face 4
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f }, // face 5
		{ 1.0f, -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f }, // face 6
		{ -1.0f, -1.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f, 1.0f }
	};

	GLfloat normals[][4] = {
		{ 0.0f, -1.0f, 0.0f, 1.0f }, // normal 1
		{ 0.0f, -1.0f, 0.0f, 1.0f },
		{ 0.0f, -1.0f, 0.0f, 1.0f },
		{ 0.8944f, 0.4472f, 0.0f, 1.0f }, // normal 2
		{ 0.8944f, 0.4472f, 0.0f, 1.0f },
		{ 0.8944f, 0.4472f, 0.0f, 1.0f },
		{ -0.0f, 0.4472f, 0.8944f, 1.0f }, // normal 3
		{ -0.0f, 0.4472f, 0.8944f, 1.0f },
		{ -0.0f, 0.4472f, 0.8944f, 1.0f },
		{ -0.8944f, 0.4472f, 0.0f, 1.0f }, // normal 4
		{ -0.8944f, 0.4472f, 0.0f, 1.0f },
		{ -0.8944f, 0.4472f, 0.0f, 1.0f },
		{ 0.0f, 0.4472f, -0.8944f, 1.0f }, // normal 5
		{ 0.0f, 0.4472f, -0.8944f, 1.0f },
		{ 0.0f, 0.4472f, -0.8944f, 1.0f },
		{ 0.0f, -1.0f, 0.0f, 1.0f }, // normal 6
		{ 0.0f, -1.0f, 0.0f, 1.0f },
		{ 0.0f, -1.0f, 0.0f, 1.0f }
	};

	// Get an unused buffer object name.
	glGenBuffers(1, &pyramidVertexArrayBufferID);

	// If it's the first time the buffer object name is used, buffer is created. 
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVertexArrayBufferID);

	/*
	Creates storage for the buffer objects while deleting pre-existing data.
	Parameter 1: Target where the buffer object is bound to
	Parameter 2: Size of the memory that is allocated on the graphics card for each vertex
	Parameter 3: Points to the vertex data that will be stored
	Parameter 4: Specifies how the data will be used, in this case it's for the GL Drawing command
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &pyramidNormalArrayBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidNormalArrayBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
}

// Initialize vertex arrays and VBOs for the box
void prepareBoxVBOs() {
	GLfloat boxVertices[][4] = {
		{ 0.0, 0.0, 0.0, 1.0 }, /* Index 0 */
		{ 0.0, 0.0, 1.0, 1.0 }, /* Index 1 */
		{ 0.0, 1.0, 0.0, 1.0 }, /* Index 2 */
		{ 0.0, 1.0, 1.0, 1.0 }, /* Index 3 */
		{ 1.0, 0.0, 0.0, 1.0 }, /* Index 4 */
		{ 1.0, 0.0, 1.0, 1.0 }, /* Index 5 */
		{ 1.0, 1.0, 0.0, 1.0 }, /* Index 6 */
		{ 1.0, 1.0, 1.0, 1.0 } /* Index 7 */
	};

	GLubyte boxElements[] = {
		4, 5, 7, // +X face
		4, 7, 6,
		0, 2, 3, // ‐X face
		0, 3, 1,
		2, 6, 7, // +Y face
		2, 7, 3,
		0, 1, 5, // ‐Y face
		0, 5, 4,
		0, 4, 6, // +Z face
		0, 6, 2,
		1, 3, 7, // ‐Z face
		1, 7, 5
	};
	
	// Get an unused buffer object name.
	glGenBuffers(1, &boxVertexArrayBufferID);

	// If it's the first time the buffer object name is used, buffer is created. 
	glBindBuffer(GL_ARRAY_BUFFER, boxVertexArrayBufferID);

	/*
	Creates storage for the buffer objects while deleting pre-existing data.
	Parameter 1: Target where the buffer object is bound to
	Parameter 2: Size of the memory that is allocated on the graphics card for each vertex
	Parameter 3: Points to the vertex data that will be stored
	Parameter 4: Specifies how the data will be used, in this case it's for the GL Drawing command
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &boxElementArrayBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxElementArrayBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxElements), boxElements, GL_STATIC_DRAW);

}

// The following method prints out the output of the shader compiler
void printLog(GLuint obj)
{
	int infologLength = 0;
	char infoLog[1024];

	if (glIsShader(obj)) {
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	}
	else {
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);
	}

	if (infologLength > 0) {
		cout << infoLog;
	}
}

void prepareShaders() {
	// The vertex position and normal vector are transformed in this vertex shader and then passed on to the fragment shader. 
	const char* vSource = {
		"#version 330\n"
		"in vec4 vPos;"
		"in vec4 normal;"

		"uniform mat4x4 mvpMatrix;"
		"uniform mat4x4 modelMatrix;"
		"uniform mat3x3 normalMatrix;"

		"out vec4 transformedPosition;"
		"out vec3 transformedNormal;"

		"void main() {"
		"	gl_Position = mvpMatrix * vPos;"
		"	transformedPosition = modelMatrix * vPos;" // transform the vertex position
		"   transformedNormal = normalize(normalMatrix * normal.xyz);" // transform the normal vector 
		"}"
	};

	// Fragment shader source code
	const char* fSource = {
		"#version 330\n"
		"in vec4 transformedPosition;"
		"in vec3 transformedNormal;"

		"uniform vec4 lightSourcePosition;"
		"uniform vec4 diffuseLightProduct;"
		"uniform vec4 ambient;"
		"uniform float attenuationA;"
		"uniform float attenuationB;"
		"uniform float attenuationC;"

		"out vec4 fragColor;"
		"void main() {"
		"	vec3 lightVector = normalize(transformedPosition.xyz - lightSourcePosition.xyz);" // light direction
		"   float dist = distance(lightSourcePosition.xyz, transformedPosition.xyz);" // distance between the light source and vertex
		"   float attenuation = 1.0f / (attenuationA + (attenuationB * dist) + (attenuationC * dist * dist));" // attenuation factor
		"	vec4 diffuse = attenuation * (max(dot(transformedNormal, lightVector), 0.0) * diffuseLightProduct);" // diffuse component of the lighting equation.
		"	fragColor = ambient + diffuse;" // adds the ambient component and diffuse component. 
		"}"
	};

	// Declare shader IDs
	GLuint vShader, fShader;

	// Create empty shader objects
	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Attach shader source code the shader objects
	glShaderSource(vShader, 1, &vSource, NULL);
	glShaderSource(fShader, 1, &fSource, NULL);

	// Compile shader objects
	glCompileShader(vShader);
	printLog(vShader);

	glCompileShader(fShader);
	printLog(fShader);

	program = glCreateProgram(); // creates an empty shader program object

	// Attach vertex and fragment shaders to the shader program
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	// Link the shader program
	glLinkProgram(program);
	printLog(program);
}

// The following function retrieves the IDs of the shader variables that will be passed to the shaders
void getShaderVariableLocations(GLuint shaderProgram) {
	vPos = glGetAttribLocation(shaderProgram, "vPos");
	normalID = glGetAttribLocation(shaderProgram, "normal");

	mvpMatrixID = glGetUniformLocation(shaderProgram, "mvpMatrix");
	colorID = glGetUniformLocation(shaderProgram, "objColor");

	modelMatrixID = glGetUniformLocation(shaderProgram, "modelMatrix");
	normalMatrixID = glGetUniformLocation(shaderProgram, "normalMatrix");

	lightSourcePositionID = glGetUniformLocation(shaderProgram, "lightSourcePosition");
	diffuseLightProductID = glGetUniformLocation(shaderProgram, "diffuseLightProduct");
	ambientID = glGetUniformLocation(shaderProgram, "ambient");

	attenuationAID = glGetUniformLocation(shaderProgram, "attenuationA");
	attenuationBID = glGetUniformLocation(shaderProgram, "attenuationB");
	attenuationCID = glGetUniformLocation(shaderProgram, "attenuationC");
}

// The following function sets all of the variables
void setShaderVariables() {
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, value_ptr(mvpMatrix));
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, value_ptr(modelMatrix));
	glUniformMatrix3fv(normalMatrixID, 1, GL_FALSE, value_ptr(normalMatrix));

	glUniform4fv(lightSourcePositionID, 1, value_ptr(lightSourcePosition));
	glUniform4fv(diffuseLightProductID, 1, value_ptr(diffuseLightProduct));
	glUniform4fv(ambientID, 1, value_ptr(ambient));
	glUniform1f(attenuationAID, attenuationA);
	glUniform1f(attenuationBID, attenuationB);
	glUniform1f(attenuationCID, attenuationC);
}

// Sets lighting related parameters
void setLightingParam() {
	diffuseLightProduct = diffuseMaterial * diffuseLightIntensity;
}

// Build the model matrix. This matrix will transform the 3D object to the proper place. 
mat4 buildModelMatrix() {
	// Rotates the 3D object
	mat4 rotationXMatrix = rotate(mat4(1.0f), radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
	mat4 rotationYMatrix = rotate(mat4(1.0f), radians(20.0f), vec3(0.0f, 1.0f, 0.0f));

	mat4 matrix = rotationYMatrix * rotationXMatrix;

	return matrix;
}

// Build the matrix to transform camera for the following two functions
mat4 buildCameraTransformationMatrix() {
	// Controls the camera's pitch which is affected by key input
	mat4 cameraRotationYMatrix = rotate(mat4(1.0f), radians((float)pitchAngle), vec3(0.0f, 1.0f, 0.0f));

	mat4 RotationX = rotate(mat4(1.0f), radians((float)rotateY), vec3(1.0f,0.0f,0.0f));
	mat4 RotationY = rotate(mat4(1.0f), radians((float)rotateX), vec3(0.0f, 1.0f, 0.0f));

	return cameraRotationYMatrix * RotationY * RotationX;
}

mat4 moveCameraTransformationMatrix() {
	mat4 cameraRotationYMatrix = translate(vec3(xpos, 0.0f, ypos));

	return cameraRotationYMatrix;
}

void buildMatrices() {
	modelMatrix = buildModelMatrix();
	// Create a camera transformation and translation matrix
	mat4 cameraTransformationMatrix = buildCameraTransformationMatrix();
	mat4 cameraTrans = moveCameraTransformationMatrix();

	// Transform the camera after view transformation. 
	mvpMatrix = projMatrix * cameraTransformationMatrix * cameraTrans * viewMatrix * modelMatrix;

	normalMatrix = column(normalMatrix, 0, vec3(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]));
	normalMatrix = column(normalMatrix, 1, vec3(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]));
	normalMatrix = column(normalMatrix, 2, vec3(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]));

	// Creates a normal matrix, which is used to transform normal vectors. 
	normalMatrix = inverseTranspose(normalMatrix);
}

// Handles the display event
void display()
{
	// Clear the window with the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	buildMatrices(); // calls the function to build the matrices needed to display the image
	setShaderVariables(); // sets the variables for the shader
	glUseProgram(program); // activates the shader program

	// If the buffer object already exists, makes that buffer the current active one. If the buffer object name is 0, disable buffer objects. 
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVertexArrayBufferID);

	// Associate the vertex array in the buffer object with the vertex attribute "position"
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(vPos); // enables the position vertex attribute

	glBindBuffer(GL_ARRAY_BUFFER, pyramidNormalArrayBufferID); // binds to a buffer for the normal array
	glVertexAttribPointer(normalID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // associates the vertex array with the normal attribute
	glEnableVertexAttribArray(normalID); // enables the normal vertex attribute

	glDrawArrays(GL_TRIANGLES, 0, 18); // starts the shader program. Draws the pyramid.

	buildMatrices(); // creates the matrices
	setShaderVariables(); // sets the variables for the shader
	glUseProgram(program); // activates the shader program

	glBindBuffer(GL_ARRAY_BUFFER, boxVertexArrayBufferID);
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxElementArrayBufferID);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)); // draws the cube
	glutSwapBuffers(); // refreshes the window
}

// Handles the reshape event
void reshape(int width, int height)
{
	// Specify the width and height of the picture within the window
	glViewport(0, 0, width, height);
	projMatrix = perspective(fieldOfView, (float)width / (float)height, nearPlane, farPlane);
	viewMatrix = lookAt(eyePosition, lookAtCenter, upVector);
}

void mouse(int x, int y) {
	rotateX = (float)x * -0.8f;
	rotateY = (float)y * -0.8f;

	glutPostRedisplay();
}

// Reads keyboard inputs for rotation
void keyboard(unsigned char key, int x, int y) {
	float fraction = 0.1f;

	switch (key) {
	case ',':
		pitchAngle -= rotationStep;
		pitchAngle %= 360;
		break;

	case '.':
		pitchAngle += rotationStep;
		pitchAngle %= 360;
		break;

	default:
		break;
	}

	glutPostRedisplay(); // refreshes display to generate a display event.
}

// Reads keyboard inputs for moving the camera up, down, left, and right
void keyboard2(int key, int x, int y) {
	float fraction = 0.1f;
	switch (key) {
	case GLUT_KEY_UP:
		ypos += 1;
		break;

	case GLUT_KEY_DOWN:
		ypos -= 1;
		break;

	case GLUT_KEY_LEFT:
		xpos -= 1;
		break;

	case GLUT_KEY_RIGHT:
		xpos += 1;
		break;

	default:
		break;
	}

	glutPostRedisplay(); // refreshes display to generate a display event.
}

// The following function initializes the pyramid and box objects as well as the shader and lighting
void init() {
	preparePyramidVBOs(); // calls function to draw pyramid
	prepareBoxVBOs(); // calls function to draw the box
	prepareShaders(); // calls the function to prepare the shader
	getShaderVariableLocations(program); 
	setLightingParam(); // calls the function to set the parameters for lighting
	glClearColor(1, 1, 1, 1); // specify the background color
	glEnable(GL_DEPTH_TEST); // enables depth test
}

void main(int argc, char *argv[])
{
	glutInit(&argc, argv); // initializes Freeglut
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // initializes window display
	glutCreateWindow("Project 2"); // creates window with the bar
	glutReshapeWindow(800, 800); // default size and allows to reshape the window

	glewInit(); // initializes Glew

	init(); // transfers data from CPU to GPU
	glutDisplayFunc(display); // registers display function
	glutReshapeFunc(reshape); // registers reshape function
	glutSpecialFunc(keyboard2); // registers the keyboard input for arrow keys
	glutKeyboardFunc(keyboard); // registers the keyboard input for basic keys
	glutPassiveMotionFunc(mouse);
	glutMainLoop();
}