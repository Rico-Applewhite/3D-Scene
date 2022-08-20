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
#include <camera.h> // Camera class


using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

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
    GLMesh CupHandle;
    GLMesh TCup;
    GLMesh Saucer;
    // Shader program
    GLuint gProgramId;

    // camera
    Camera gCamera(glm::vec3(2.0f, 2.0f, 6.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);



    float gDeltaTime = 0.0f; // Time between current frame and last frame
    float gLastFrame = 0.0f;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);

// Mesh Variables for my shapes
void UCreateMesh(GLMesh& Cube);
void UDestroyMesh(GLMesh& Cube);
void UCreateMesh2(GLMesh& Plane);
void UDestroyMesh2(GLMesh& Plane);
void UCreateMesh3(GLMesh& CupHandle);
void UDestroyMesh(GLMesh& CupHandle);
void UCreateMesh4(GLMesh& TCup);
void UDestroyMesh(GLMesh& TCup);
void UCreateMesh5(GLMesh& Saucer);
void UDestroyMesh(GLMesh& Saucer);

void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

// Camera control Variables
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
bool Perspective = true;

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
    UCreateMesh2(Plane);
    UCreateMesh3(CupHandle);
    UCreateMesh4(TCup);
    UCreateMesh5(Saucer);


    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

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
    UDestroyMesh(CupHandle);
    UDestroyMesh(TCup);
    UDestroyMesh(Saucer);



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
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

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
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UPWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWNWARD, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        cout << "G" << endl;
        if (projection == glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f)) {
            projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -50.0f, 50.0f);
            cout << " perspective to ortho" << endl;
        }

        else if (projection == glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -50.0f, 50.0f)) {
            (projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f));
            cout << "ortho to perspective" << endl;
        }

    }

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // Reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URender()
{
    // Clear the background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    //World view
    glm::mat4 mvp = view * projection * model;

    // Set the shader to be used
    glUseProgram(gProgramId);
    //------------------------------------------------------------------------
    //Draw The plane
    
        glm::mat4 shape3 = glm::mat4(1.0f);
        glm::mat4 scale = glm::scale(shape3, glm::vec3(30.0f, 5.0f, 60.0f));
        // glm::mat4 trans = glm::translate(shape3, glm::vec3(-2.0, -0.5f, 0.0f));

        glm::mat4 model3 = scale * shape3;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");
        GLint viewLoc = glGetUniformLocation(gProgramId, "view");
        GLint projLoc = glGetUniformLocation(gProgramId, "projection");


        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glBindVertexArray(Plane.vao);
        GLuint transformLocation = glGetUniformLocation(gProgramId, "shaderTransform");
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(model3));
        glDrawElements(GL_TRIANGLES, Plane.nIndices, GL_UNSIGNED_SHORT, NULL);
   

        glBindVertexArray(0);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(Cube.vao);
    {
        //------------------------------------------------------------------------
        //Draw Two books
        glDrawElements(GL_LINE_LOOP, Cube.nIndices, GL_UNSIGNED_SHORT, NULL);

        glm::mat4 shape2 = glm::mat4(1.0f);
        //Draws Top Book
        //Translate Book2 upwards and to the left in relation of the bottom book and a slight rotaton.
        glm::mat4 trans = glm::translate(shape2, glm::vec3(-0.07f, 0.4f, -0.2f));
        glm::mat4 rotate = glm::rotate(shape2, glm::radians(-10.0f), glm::vec3(0.0f, 2.0f, 0.0f));
        glm::mat4 model2 = trans * rotate;

        GLint modelLoc2 = glGetUniformLocation(gProgramId, "model");
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));

        //Draws Top Book
        glDrawElements(GL_LINE_LOOP, Cube.nIndices, GL_UNSIGNED_SHORT, NULL);
    }
        glBindVertexArray(0);
        {
            //------------------------------------------------------------------------
            //Draw The Cup handle

            glm::mat4 shape4 = glm::mat4(1.0f);
            //glm::mat4 scale = glm::scale(shape4, glm::vec3(-2.0f, -2.0f, -1.0f));
            // glm::mat4 trans = glm::translate(shape4, glm::vec3(-2.0, -0.5f, 0.0f));

            glm::mat4 model4 =shape4;

            // Retrieves and passes transform matrices to the Shader program
            GLint modelLoc = glGetUniformLocation(gProgramId, "model");
            GLint viewLoc = glGetUniformLocation(gProgramId, "view");
            GLint projLoc = glGetUniformLocation(gProgramId, "projection");


            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


            glBindVertexArray(CupHandle.vao);
            GLuint transformLocation = glGetUniformLocation(gProgramId, "shaderTransform");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(model4));
            glDrawElements(GL_TRIANGLE_FAN, CupHandle.nIndices, GL_UNSIGNED_SHORT, NULL);
        }

        glBindVertexArray(0);

        {
            //------------------------------------------------------------------------
            //Draw The Saucer

            glm::mat4 shape4 = glm::mat4(1.0f);
            glm::mat4 rotate = glm::rotate(shape4, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            glm::mat4 scale = glm::scale(shape4, glm::vec3(2.0f, 2.0f, 0.0f));
            glm::mat4 trans = glm::translate(shape4, glm::vec3(0.0, 0.6f, 0.0f));

            glm::mat4 Model6 = trans * rotate *scale;

            // Retrieves and passes transform matrices to the Shader program
            GLint modelLoc = glGetUniformLocation(gProgramId, "model");
            GLint viewLoc = glGetUniformLocation(gProgramId, "view");
            GLint projLoc = glGetUniformLocation(gProgramId, "projection");


            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model6));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


            glBindVertexArray(Saucer.vao);
            GLuint transformLocation = glGetUniformLocation(gProgramId, "shaderTransform");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(Model6));
            glDrawElements(GL_TRIANGLE_FAN, Saucer.nIndices, GL_UNSIGNED_SHORT, NULL);
        }
        glBindVertexArray(0);
        {
            //------------------------------------------------------------------------
            //Draw The Cup

            glm::mat4 shape4 = glm::mat4(1.0f);
            glm::mat4 rotate = glm::rotate(shape4, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            glm::mat4 trans = glm::translate(shape4, glm::vec3(0.0, 1.6f, 0.0f));

            glm::mat4 model5 = trans* rotate;

            // Retrieves and passes transform matrices to the Shader program
            GLint modelLoc = glGetUniformLocation(gProgramId, "model");
            GLint viewLoc = glGetUniformLocation(gProgramId, "view");
            GLint projLoc = glGetUniformLocation(gProgramId, "projection");


            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


            glBindVertexArray(TCup.vao);
            GLuint transformLocation = glGetUniformLocation(gProgramId, "shaderTransform");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(model5));
            glDrawElements(GL_TRIANGLES, TCup.nIndices, GL_UNSIGNED_SHORT, NULL);
        }
        glBindVertexArray(0);




    //GLuint transformLocation = glGetUniformLocation(gProgramId, "shaderTransform");
   //glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformation));



    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh functions
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
        0.5f, -0.2, 1.0f,       1.0f, 0.0f, 0.0f, 1.0f//11 front R bottom
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
void UCreateMesh2(GLMesh& Plane)
{
    // Specifies screen coordinates (x,y) and color for triangle vertices
    GLfloat verts[] =
    {
        //Plane Vertices
        -1.0f, -1.0, 1.0f,       0.5f, 0.5f, 0.5f, 1.0f, //0front L top
        1.0f, -1.0, 1.0f,       0.5f, 0.5f, 0.5f, 1.0f,//1 front L bottom
        -1.0f, -1.0, -1.0f,       0.5f, 0.5f, 0.5f, 1.0f,//2 front R top
        1.0f, -1.0, -1.0f,       0.5f, 0.5f, 0.5f, 1.0f,//3 front R bottom

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
    GLushort indices[] = { 0,1,2,
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
void UCreateMesh3(GLMesh& CupHandle)
{
    // Specifies screen coordinates (x,y) and color for triangle vertices
    GLfloat verts[] =
    {
        //CupHandle Vertices
        //outer handle
       -0.1f, 0.2f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f, //0front L top
        0.1f, 0.2f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f,//1 front L bottom
       -0.1f, 0.0f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f,//2 front R top
        0.1f, 0.0f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f,//3 front R bottom

       -0.1f, -0.5f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f,//4 front R top
        0.1f, -0.5f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f,//5 front R bottom
       -0.1f, -0.8f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f, //6front L top
        0.1f, -0.8f, 0.5f,       0.0f, 1.0f, 0.5f, 1.0f,//7 front L bottom

        //inner handle
        0.1f, 0.2f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f, //8front L top
        -0.1f, 0.2f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f,//9 front L bottom
        0.1f, 0.0f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f,//10 front R top
        -0.1f, 0.0f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f,//11 front R bottom

         0.1f, -0.5f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f, //12front L top
        -0.1f, -0.5f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f,//13 front L bottom
        0.1f, -0.8f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f,//14 front R top
        -0.1f, -0.8f, 0.0f,       0.0f, 1.0f, 0.5f, 1.0f,//15 front R bottom

        -0.1f, 0.1f, 0.4f,       0.0f, 1.0f, 0.5f, 1.0f, //16 front L top
        0.1f,0.1f, 0.4f,       0.0f, 1.0f, 0.5f, 1.0f,//17 front L bottom
        -0.1f, -0.6f, 0.4f,       0.0f, 1.0f, 0.5f, 1.0f,//18 front R top
        0.1f, -0.6f, 0.4f,       0.0f, 1.0f, 0.5f, 1.0f,//19 front R bottom
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    glGenVertexArrays(1, &CupHandle.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(CupHandle.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, CupHandle.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, CupHandle.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Data for the indices
    GLushort indices[] = {0,6,1,
        7,1,6,

        8,9,10,
        10,11,9,
        12,13,14,
        15,14,13,
        16,17,18,
        19,18,17,

        1,8,3,
        8,10,3,
        5,12,7,
        14,12,7,
        3,17,5,
        19,5,17,

        0,9,2,
        11,2,9,
        13,4,6,
        15,6,4,
        6,2,18,
        4,18,2
    };
    CupHandle.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CupHandle.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}
void UCreateMesh4(GLMesh& TCup)
{
    // Specifies screen coordinates (x,y) and color for triangle vertices
    GLfloat verts[] =
    {
        //Cup Vertices
        //Top
        0.0f, 0.0f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //0 front L top

        0.0f, 0.35f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //1 front L top
       0.25f, 0.25f,0.5f,       0.4f, 1.0f, 0.5f, 1.0f, //2 front L top
        0.35f, 0.0f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //3 front L top
        0.25f, -0.25f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //4 front L top
        0.0f, -0.35f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //5 front L top
       -0.25f, -0.25f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //6 front L top
       -0.35f, 0.0f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //7 front L top
       -0.25f, 0.25f,0.5f,         0.4f, 1.0f, 0.5f, 1.0f, //8 front L top

        0.0f, 0.15f,1.0f,         0.4f, 1.0f, 0.5f, 1.0f, //9 front L top
       0.125f, 0.125f,1.0f,       0.4f, 1.0f, 0.5f, 1.0f, //10 front L top
        0.15f, 0.0f,1.0f,         0.4f, 1.0f, 0.5f, 1.0f, //11 front L top
        0.125f, -0.125f,1.0f,     0.4f, 1.0f, 0.5f, 1.0f, //12 front L top
        0.0f, -0.15f,1.0f,        0.4f, 1.0f, 0.5f, 1.0f, //13 front L top
       -0.125f,-0.125f,1.0f,     0.4f, 1.0f, 0.5f, 1.0f, //14 front L top
       -0.15f, 0.0f,1.0f,         0.4f, 1.0f, 0.5f, 1.0f, //15 front L top
       -0.125f, 0.125f,1.0f,     0.4f, 1.0f, 0.5f, 1.0f, //16 front L top

        0.0f, 0.0f,1.0f,         0.4f, 1.0f, 0.5f, 1.0f, //7 front L top
        
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    glGenVertexArrays(1, &TCup.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(TCup.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, TCup.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, TCup.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Data for the indices
    GLushort indices[] = {

        //bottom of cylinder
        9,10,17,
        10,11,17,
        11,12,17,
        12,13,17,
        13,14,17,
        14,15,17,
        15,16,17,
        16,9,17,
        
        //top of cylinder
1,2,0,
2,3,0,
3,4,0,
4,5,0,
5,6,0,
6,7,0,
7,8,0,
8,1,0,

        //Sides
        1,2,9,
        9,10,2,
        2,3,10,
        10,11,3,
        3,4,11,
        11,12,4,
        4,5,12,
        12,13,5,
        5,6,13,
        13,14,6,
        6,7,14,
        14,15,7,
        7,8,15,
        15,16,8,
        8,1,16,
        16,9,1
    };
    TCup.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TCup.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}
void UCreateMesh5(GLMesh& Saucer)
{
    // Specifies screen coordinates (x,y) and color for triangle vertices
    GLfloat verts[] =
    {
        //Cup Vertices
        //Top
        0.0f, 0.0f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //0 front L top

        0.0f, 0.35f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //1 front L top
       0.25f, 0.25f,0.95f,       0.0f, 1.0f, 0.5f, 1.0f, //2 front L top
        0.35f, 0.0f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //3 front L top
        0.25f, -0.25f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //4 front L top
        0.0f, -0.35f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //5 front L top
       -0.25f, -0.25f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //6 front L top
       -0.35f, 0.0f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //7 front L top
       -0.25f, 0.25f,0.95f,         0.0f, 1.0f, 0.5f, 1.0f, //8 front L top

        0.0f, 0.15f,1.0f,         0.0f, 1.0f, 0.5f, 1.0f, //9 front L top
       0.125f, 0.125f,1.0f,       0.0f, 1.0f, 0.5f, 1.0f, //10 front L top
        0.15f, 0.0f,1.0f,         0.0f, 1.0f, 0.5f, 1.0f, //11 front L top
        0.125f, -0.125f,1.0f,     0.0f, 1.0f, 0.5f, 1.0f, //12 front L top
        0.0f, -0.15f,1.0f,        0.0f, 1.0f, 0.5f, 1.0f, //13 front L top
       -0.125f,-0.125f,1.0f,     0.0f, 1.0f, 0.5f, 1.0f, //14 front L top
       -0.15f, 0.0f,1.0f,         0.0f, 1.0f, 0.5f, 1.0f, //15 front L top
       -0.125f, 0.125f,1.0f,     0.0f, 1.0f, 0.5f, 1.0f, //16 front L top

        0.0f, 0.0f,1.0f,         0.0f, 1.0f, 0.5f, 1.0f, //7 front L top

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    glGenVertexArrays(1, &Saucer.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(Saucer.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, Saucer.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, Saucer.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Data for the indices
    GLushort indices[] = {

        //bottom of cylinder
        9,10,17,
        10,11,17,
        11,12,17,
        12,13,17,
        13,14,17,
        14,15,17,
        15,16,17,
        16,9,17,

        //top of cylinder
1,2,0,
2,3,0,
3,4,0,
4,5,0,
5,6,0,
6,7,0,
7,8,0,
8,1,0,

//Sides
1,2,9,
9,10,2,
10,11,3,
3,4,12,
12,13,5,
5,6,13,
13,14,6,
6,7,14,
14,15,7,
7,8,15,
15,16,8,
8,1,16,
16,9,1
    };
    Saucer.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Saucer.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

void UDestroyMesh(GLMesh& Cube)
{
    glDeleteVertexArrays(1, &Cube.vao);
    glDeleteBuffers(2, Cube.vbos);
}

void UDestroyMesh2(GLMesh& Plane)
{
    glDeleteVertexArrays(1, &Plane.vao);
    glDeleteBuffers(2, Plane.vbos);
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
