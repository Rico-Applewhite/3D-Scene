//Rico D Applewhite
//CS-330-T6633 Comp Graphic and Visualization 22EW6
//Project One
//July 2022

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

typedef Struct{
    float px,py,pz;
    float r, g, b, t;
}my_GL_Vertex;

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Project"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh Cube;
    GLMesh Plane;
    // Shader program
    GLuint gProgramId;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLuint numberOfSides);

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

out vec4 vertexColor; // variable to transfer color data to the fragment shader

//Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexColor = color; // references incoming color data
}
);




/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(vertexColor);
}
);



int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(Cube); // Calls the function to create the Vertex Buffer Object
    UCreateMesh(Plane);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(Cube);
    UDestroyMesh(Plane);


    // Release shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }


    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// Functioned called to render a frame
void URender()
{
    // Clear the background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //Create World View
    // Creates a perspective projection (field of view, aspect ratio, near plane, and far plane are the four parameters)
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    // Transforms the camera
    glm::mat4 view = glm::lookAt(
        glm::vec3(2, 2, 6),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
    glm::mat4 model = glm::mat4(1.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(Cube.vao);

    //Draws Bottom Book
    glDrawElements(GL_LINE_LOOP, Cube.nIndices, GL_UNSIGNED_SHORT, NULL);

    glm::mat4 shape2 = glm::mat4(1.0f);
    //Draws Top Book
    //Translate Book2 upwards and to the left in relation of the bottom book and a slight rotaton.
    glm::mat4 trans = glm::translate(shape2, glm::vec3(-0.05f, 0.5f, 0.0f));
    glm::mat4 rotate = glm::rotate(shape2, glm::radians(-10.0f), glm::vec3(0.0f, 2.0f, 0.0f));
    glm::mat4 model2 = trans * rotate;

    GLint modelLoc2 = glGetUniformLocation(gProgramId, "model");
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));

    //Draws Top Book
    glDrawElements(GL_LINE_LOOP, Cube.nIndices, GL_UNSIGNED_SHORT, NULL);

    glBindVertexArray(Plane.vao);
    glDrawElements(GL_LINE_LOOP, Plane.nIndices, GL_UNSIGNED_SHORT, NULL);






    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& Cube)
{
    // Specifies screen coordinates (x,y) and color for triangle vertices
    GLfloat verts[] =
    {
        //Book Vertices
        -0.5f, 0.2, 1.0f,       0.0f, 1.0f, 0.5f, 1.0f, //0front L top
        -0.5f, -0.2, 1.0f,       0.0f, 1.0f, 0.5f, 1.0f,//1 front L bottom
        0.5f, 0.2, 1.0f,       0.0f, 1.0f, 0.5f, 1.0f,//2 front R top
        0.5f, -0.2, 1.0f,       0.0f, 1.0f, 0.5f, 1.0f,//3 front R bottom
        -0.5f, 0.2, -1.0f,       1.0f, 0.0f, 0.5f, 1.0f,//4 back l top
        -0.5f, -0.2, -1.0f,       1.0f, 0.0f, 0.5f, 1.0f,//5 back l bottom
        0.5f, 0.2, -1.0f,       1.0f, 0.0f, 0.5f, 1.0f,//6 back right top
        0.5f, -0.2, -1.0f,       1.0f, 0.0f, 0.5f, 1.0f,//7 back right bottom

        // Running into a problem dawing multiple objects
        -0.5f, 0.2, 1.0f,       1.0f, 0.0f, 0.0f, 1.0f, //8front L top
        -0.5f, -0.2, 1.0f,       1.0f, 0.0f, 0.0f, 1.0f,//9 front L bottom
        0.5f, 0.2, 1.0f,       1.0f, 0.0f, 0.0f, 1.0f,//10 front R top
        0.5f, -0.2, 1.0f,       1.0f, 0.0f, 0.0f, 1.0//11 front R bottom
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    glGenVertexArrays(1, &Cube.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(Cube.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, Cube.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, Cube.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Data for the indices
    GLushort indices[] = { 0,2,1,
       2,3,1,
       5,7,1,
       7,3,1,
       4,6,5,
       6,7,5,
       4,6,0,
       6,2,0,
       4,0,1,
       0,1,5,
       6,2,7,
       2,3,7
    };
   Cube.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Cube.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}
// Implements the UCreateMesh function
void UCreateMesh(GLMesh& Plane)
{
    // Specifies screen coordinates (x,y) and color for triangle vertices
    GLfloat verts[] =
    {
        //Book Vertices
        -1.0f, -1.0, 1.0f,       0.0f, 1.0f, 0.5f, 1.0f, //0front L top
        1.0f, 1.0, 1.0f,       0.0f, 1.0f, 0.5f, 1.0f,//1 front L bottom
        -1.0f, -1.0, -1.0f,       0.0f, 1.0f, 0.5f, 1.0f,//2 front R top
        1.0f, 1.0, -1.0f,       0.0f, 1.0f, 0.5f, 1.0f,//3 front R bottom

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    glGenVertexArrays(1, &Plane.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(Plane.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, Plane.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, Plane.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Data for the indices
    GLushort indices[] = { 0,2,1,
       2,3,1,
    };
    Plane.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Plane.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

/*glGenVertexArrays(1, &vaoCube);
glGenVertexArrays(1, &vaoPlane);
glGenBuffers(2, &vboCube);
glGenBuffers(2, &vboPlane);

glBindVertexArray(vaoCube);

glBindBuffer(GL_ARRAY_BUFFER, vboCube);
glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

glBindVertexArray(vaoPlane);

glBindBuffer(GL_ARRAY_BUFFER, vboPlane);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_plane), vertices_plane, GL_STATIC_DRAW);*/

void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}



void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
