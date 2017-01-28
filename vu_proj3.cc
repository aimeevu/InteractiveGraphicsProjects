/*
Thao-Nguyen Vu | CSC4820: Interactive Graphics | Friday 1:00PM - 4:25PM | Fall 2016
This program demonstrates multi-texture mapping. Only lighting provided in the code is visible.
No extra lighting was added.

Majority of the code was provided by Ying Zhu from his class website. The program contains ambient lighting as well as a light source.

*/

#include <GL/glew.h> // GLEW header
#include <GL/freeglut.h> // Freeglut header

// GLM header files
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <SOIL.h> // Simple OpenGL Image Library header file

#include <iostream> // C++ header files

using namespace std;
using namespace glm;

#define BUFFER_OFFSET(offset) ((GLvoid *) offset)
//Vertices of the pyramid
GLfloat pyramidVertices[][4] = {
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

int numVertices = 18;
// Normal points for pyramid
GLfloat pyramidNormals[][4] = {
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

// Coordinates for where the texture will be applied
GLfloat pyramidTextureCoords[][2] = {
	{ 0.0319, 0.4192 }, // textureCoord 1
	{ 0.3546, 0.0966 },
	{ 0.3546, 0.4192 },
	{ 0.4223, 0.5177 },	// textureCoord 2
	{ 0.2541, 0.8753 },
	{ 0.0996, 0.5116 },
	{ 0.8047, 0.5250 },	// textureCoord 3
	{ 0.6434, 0.8857 },
	{ 0.4820, 0.5250 },
	{ 0.6637, 0.0981 },	// textureCoord 4
	{ 0.5130, 0.4184 },
	{ 0.3748, 0.0926 },
	{ 0.8416, 0.4227 },	// textureCoord 5
	{ 0.6922, 0.0988 },
	{ 0.9834, 0.0954 },
	{ 0.0319, 0.4192 },	// textureCoord 6
	{ 0.0319, 0.0966 },
	{ 0.3546, 0.0966 }
};

// Relative file path for the texture images
const char *imageFileName = "..\\Images\\image.jpg";
const char *imageFileName2 = "..\\Images\\gold_textures3.jpg";
const char *imageFileName3 = "..\\Images\\marble1.jpg";

// VBO buffer IDs
GLuint vertexArrayBufferID = 0;
GLuint normalArrayBufferID = 0;
GLuint texCoordArrayBufferID = 0;

GLuint program; // shader program ID

// Shader variable IDs
GLint vPos; // vertex attribute for position
GLint normalID; // vertex attribute for normal
GLint textureCoordID; // vertex attribute for texture coordinates

GLint mvpMatrixID; // uniform variable for model, view, projection matrix
GLint modelMatrixID; // uniform variable for model-view matrix
GLint normalMatrixID; // uniform variable for transforming normals
GLint lightSourcePositionID; // uniform variable for lighting calculation
GLint diffuseLightProductID; // uniform variable for diffuse lighting calculation
GLint ambientID; // uniform variable for ambient lighting calculation
GLint attenuationAID; // attenuation variables
GLint attenuationBID;
GLint attenuationCID;
GLint textureSamplerID; // texture sampler ID
GLint textureSamplerID2;
GLint textureSamplerID3;

// Texture object ID
GLuint textureID;
GLuint textureID2;
GLuint textureID3;

// Texture unit ID
GLenum textureUnitID = GL_TEXTURE0;
GLenum textureUnitID2 = GL_TEXTURE1;
GLenum textureUnitID3 = GL_TEXTURE2;
// Texture sampler values
GLuint textureSamplerValue = 0;
GLuint textureSamplerValue2 = 1;
GLuint textureSamplerValue3 = 2;

// Transformation matrices
mat4 projMatrix;
mat4 mvpMatrix;
mat4 modelMatrix;
mat4 viewMatrix;
mat3 normalMatrix;

// Light parameters
vec4 lightSourcePosition = vec4(0.0f, 4.0f, 0.0f, 1.0f);
vec4 diffuseMaterial = vec4(0.5f, 0.5f, 0.5f, 1.0f);
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

// Mouse controlled rotation angles
float rotateX = 0;
float rotateY = 0;

// Initialize vertex arrays and VBOs
void prepareVBOs() {
	// Get an unused buffer object name.
	glGenBuffers(1, &vertexArrayBufferID);

	// If it's the first time the buffer object name is used, buffer is created.
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBufferID);

	/*
	Creates storage for the buffer objects while deleting pre-existing data.
	Parameter 1: Target where the buffer object is bound to
	Parameter 2: Size of the memory that is allocated on the graphics card for each vertex
	Parameter 3: Points to the vertex data that will be stored
	Parameter 4: Specifies how the data will be used, in this case it's for the GL Drawing command
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &normalArrayBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, normalArrayBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);

	// Creates a buffer object for the texture coordinates. 
	glGenBuffers(1, &texCoordArrayBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordArrayBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTextureCoords), pyramidTextureCoords, GL_STATIC_DRAW);
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
	// Vertex shader source code 
	const char* vSource = {
		"#version 330\n"
		"in vec4 vPos;"
		"in vec4 normal;"
		"in vec2 vTextureCoord;"

		"uniform mat4x4 mvpMatrix;"
		"uniform mat4x4 modelMatrix;"
		"uniform mat3x3 normalMatrix;"

		"out vec4 transformedPosition;"
		"out vec3 transformedNormal;"
		"out vec2 textureCoord;"

		"void main() {"
		"	gl_Position = mvpMatrix * vPos;"
		"	transformedPosition = modelMatrix * vPos;" // transforms the vPos to world space
		"   transformedNormal = normalize(normalMatrix * normal.xyz);" // transforms normal vector to world space
		"   textureCoord = vec2(vTextureCoord.x, vTextureCoord.y);"
		"}"
	};

	// Fragment shader source code
	const char* fSource = {
		"#version 330\n"
		"in vec4 transformedPosition;"
		"in vec3 transformedNormal;"
		"in vec2 textureCoord;"

		"uniform vec4 lightSourcePosition;"
		"uniform vec4 diffuseLightProduct;"
		"uniform vec4 ambient;"
		"uniform float attenuationA;"
		"uniform float attenuationB;"
		"uniform float attenuationC;"

		"uniform sampler2D tex;"
		"uniform sampler2D tex2;"
		"uniform sampler2D tex3;"

		"out vec4 fragColor;"
		"void main() {"
		"	vec3 lightVector = normalize(transformedPosition.xyz - lightSourcePosition.xyz);" // direction of the light
		"   float dist = distance(lightSourcePosition.xyz, transformedPosition.xyz);" // calculates distance between light source and vertex
		"   float attenuation = 1.0f / (attenuationA + (attenuationB * dist) + (attenuationC * dist * dist));" // calculates attenuation
		"	vec4 diffuse = attenuation * (max(dot(transformedNormal, lightVector), 0.0) * diffuseLightProduct);" // calculates diffuse

		// Assigns the textures to the color that will be used to determine which texture appears where
		"   vec4 textureColor = texture(tex, textureCoord);"
		"	vec4 textureColor2 = texture(tex2, textureCoord);"
		"	vec4 textureColor3 = texture2D(tex3, textureCoord);"

		// Combines the ambient component and diffuse component to display the texture onto the object
		"	fragColor = mix((ambient + diffuse), textureColor3, 0.6f);"
		
		/*
		The following if state determines the color of the black and white image. If it is black, the gold texture is applied. If it
		is white, the marble texture is applied.
		*/
		"	if(textureColor.r > 0.5)"
		"		fragColor = mix((ambient + diffuse), textureColor2, 0.6f);"
		"	else"
		"		fragColor = mix((ambient + diffuse), textureColor3, 0.6f);"

		
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

	// Attaches vertex and fragment shaders to the shader program
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	// Links the shader program
	glLinkProgram(program);
	printLog(program);
}

// The following function retrieves the IDs of the shader variables that will be passed to the shaders
void getShaderVariableLocations(GLuint shaderProgram) {
	vPos = glGetAttribLocation(shaderProgram, "vPos");
	normalID = glGetAttribLocation(shaderProgram, "normal");

	textureCoordID = glGetAttribLocation(shaderProgram, "vTextureCoord");

	mvpMatrixID = glGetUniformLocation(shaderProgram, "mvpMatrix");
	modelMatrixID = glGetUniformLocation(shaderProgram, "modelMatrix");
	normalMatrixID = glGetUniformLocation(shaderProgram, "normalMatrix");

	lightSourcePositionID = glGetUniformLocation(shaderProgram, "lightSourcePosition");
	diffuseLightProductID = glGetUniformLocation(shaderProgram, "diffuseLightProduct");
	ambientID = glGetUniformLocation(shaderProgram, "ambient");

	attenuationAID = glGetUniformLocation(shaderProgram, "attenuationA");
	attenuationBID = glGetUniformLocation(shaderProgram, "attenuationB");
	attenuationCID = glGetUniformLocation(shaderProgram, "attenuationC");

	textureSamplerID = glGetUniformLocation(shaderProgram, "tex");
	textureSamplerID2 = glGetUniformLocation(shaderProgram, "tex2");
	textureSamplerID3 = glGetUniformLocation(shaderProgram, "tex3");
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

	// Associates texture sampler ID in the shader with the active texture unit number.
	glUniform1i(textureSamplerID, textureSamplerValue);
	glUniform1i(textureSamplerID2, textureSamplerValue2);
	glUniform1i(textureSamplerID3, textureSamplerValue3);
}

// Lighting parameters are set
void setLightingParam() {
	diffuseLightProduct = diffuseMaterial * diffuseLightIntensity;
}

// Texture images are prepared using SOIL
void prepareTextureImage() {
	textureID = SOIL_load_OGL_texture(imageFileName,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	textureID2 = SOIL_load_OGL_texture(imageFileName2,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	textureID3 = SOIL_load_OGL_texture(imageFileName3,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	// Specifies the current active texture unit number for each texture used in the program. 
	glActiveTexture(textureUnitID);
	glActiveTexture(textureUnitID2);
	glActiveTexture(textureUnitID3);

	// Binds the texture ID with the active texture unit number.
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID3);
}

// Builds the model matrix with the transformations. 
mat4 buildModelMatrix() {
	mat4 rotationXMatrix = rotate(mat4(1.0f), radians(rotateX), vec3(1.0f, 0.0f, 0.0f)); // calculates rotation along x axis
	mat4 rotationYMatrix = rotate(mat4(1.0f), radians(rotateY), vec3(0.0f, 1.0f, 0.0f)); // calculates rotation along y axis
	mat4 matrix = rotationYMatrix * rotationXMatrix; // calculates the matrix taking transformations into account
	return matrix;
}

// Builds the MVP Matrix with the projection, view, and model matrices that are initialized in the reshape function and the buildModelMatrix function
void buildMatrices() {
	modelMatrix = buildModelMatrix();
	mvpMatrix = projMatrix * viewMatrix * modelMatrix; // calculates the model-view-projection matrix
	normalMatrix = column(normalMatrix, 0, vec3(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]));
	normalMatrix = column(normalMatrix, 1, vec3(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]));
	normalMatrix = column(normalMatrix, 2, vec3(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]));
	normalMatrix = inverseTranspose(normalMatrix); // computes the inverse transpose
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
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBufferID);

	// Associates the vertex array in the buffer object with the position vertex attribute
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPos); // enables the position vertex attribute

	glBindBuffer(GL_ARRAY_BUFFER, normalArrayBufferID); // binds to a buffer for the normal array
	glVertexAttribPointer(normalID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // associates the vertex array with the normal attribute
	glEnableVertexAttribArray(normalID); // enables the normal vertex attribute

	glBindBuffer(GL_ARRAY_BUFFER, texCoordArrayBufferID); // binds texture coordinates array to a buffer
	glVertexAttribPointer(textureCoordID, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // associates the vertex array with the texture coordinate attributes
	glEnableVertexAttribArray(textureCoordID); // enables the texture coordinate attributes

	glDrawArrays(GL_TRIANGLES, 0, numVertices); // draws the pyramid with the number of vertices as the third parameter

	glutSwapBuffers(); // refreshes the window
}

// Handles the reshape event
void reshape(int width, int height)
{
	glViewport(0, 0, width, height); // specifies the region used for drawing the Cartesian space
	projMatrix = perspective(fieldOfView, (float)width / (float)height, nearPlane, farPlane); // determines the projection matrix
	viewMatrix = lookAt(eyePosition, lookAtCenter, upVector); //  determines the matrix that displays the view of the display
}

// Reads mouse motion data and converts them to rotation angles that displays the object rotating with the location of the mouse
void passiveMotion(int x, int y) {

	rotateY = (float)x * -0.5f;
	rotateX = (float)y * 0.5f;

	glutPostRedisplay(); // refreshes the window to display the motion event
}

// The following function initializes the pyramid object as well as the shader, texture, and lighting
void init() {
	prepareVBOs(); // calls function to draw pyramid
	prepareShaders(); // calls the function to prepare the shader
	getShaderVariableLocations(program); 
	setLightingParam(); // calls the function to set the parameters for lighting
	prepareTextureImage(); // calls the function to prepare the texture image
	glClearColor(1, 1, 1, 1); // specify the background color
	glEnable(GL_DEPTH_TEST); // enables depth test
}

void main(int argc, char *argv[])
{
	glutInit(&argc, argv); // initializes Freeglut
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // initializes window display
	glutCreateWindow("Project 3"); // creates window with the bar
	glutReshapeWindow(800, 800); // default size and allows to reshape the window
	glewInit(); // initializes Glew
	init(); // transfers data from CPU to GPU
	glutDisplayFunc(display); // registers display function
	glutReshapeFunc(reshape); // registers reshape function
	glutPassiveMotionFunc(passiveMotion); // registers the passive mouse motion call back function
	glutMainLoop(); // starts the loop for the event
}