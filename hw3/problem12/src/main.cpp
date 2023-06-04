#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "resource_utils.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "math_utils.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool isKeyboardDone[1024] = {0};

// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// show outer line?
bool showLine = true;
bool fillFace = false;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2018-19427_HanjunKim", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    

    // build and compile our shader program
    // ------------------------------------
    // TODO: define 3 shaders
    // (1) geometry shader for spline render.
    // (2) simple shader for spline's outer line render.
    // (optional) (3) tessellation shader for bezier surface.
    Shader splineShader("../shaders/splines/spline_shader.vs", "../shaders/splines/spline_shader.fs", "../shaders/splines/spline_shader.gs");
    Shader simpleShader("../shaders/outer_line_shader.vs", "../shaders/outer_line_shader.fs");
    // TODO : load requied model and save data to VAO. 
    // Implement and use loadSplineControlPoints/loadBezierSurfaceControlPoints in resource_utils.h


    VAO* splineVAO;
    splineVAO = loadSplineControlPoints("../resources/spline_control_point_data/spline_simple.txt");

    VAO* fontVAO;
    fontVAO = loadSplineControlPoints("../resources/spline_control_point_data/spline_u.txt");

    VAO* loopVAO;
    loopVAO = loadSplineControlPoints("../resources/spline_control_point_data/spline_complex.txt");

    // -----------
    // load patches
    glPatchParameteri(GL_PATCH_VERTICES, 16);

    Shader tessShader("../shaders/bezier_surface/tess.vs", "../shaders/bezier_surface/tess.fs", "../shaders/bezier_surface/tess.gs",
        "../shaders/bezier_surface/TCS.glsl", "../shaders/bezier_surface/TES.glsl");
    VAO* gumboVAO;
    gumboVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/gumbo.bpt");

    VAO* heartVAO;
    heartVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/heart.bpt");

    VAO* teapotVAO;
    teapotVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/teapot.bpt");
    // render loop
    // -----------
    float oldTime = 0;
    GLfloat aspect = SCR_WIDTH / SCR_HEIGHT;
    GLfloat nearClippingDistance = 0.1f;
    GLfloat farClippingDistance = 1000.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - oldTime;
        deltaTime = dt;

        oldTime = currentTime;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        splineShader.use();

        //camera
        GLfloat FoV = glm::radians(camera.Zoom);
        glm::mat4 projection = glm::perspective(FoV, aspect, nearClippingDistance, farClippingDistance);

        

#pragma region SimpleSplines

        // Bezier form
        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(-.6f, 0.6f, -2.f));
        model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
        splineShader.setMat4("projection", projection);
        splineShader.setMat4("view", camera.GetViewMatrix());
        splineShader.setMat4("model", model);

        glm::mat4 Bezier = glm::mat4(glm::vec4(-1, 3, -3, 1), glm::vec4(3, -6, 3, 0), glm::vec4(-3, 3, 0, 0), glm::vec4(1, 0, 0, 0));
        splineShader.setMat4("B", Bezier);
        glBindVertexArray(splineVAO->ID);
        glDrawArrays(GL_LINES_ADJACENCY, 0, splineVAO->dataSize);

        if (showLine) {
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", camera.GetViewMatrix());
            simpleShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, splineVAO->dataSize);
        }


        // BSpline - cubic
        splineShader.use();
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.0f, 0.6f, -2.0f));
        model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
        splineShader.setMat4("model", model);

        //glm::mat4 BSpline = glm::mat4(glm::vec4(-1, 3, -3, -1), glm::vec4(3, -6, 0, 4), glm::vec4(-3, 3, 3, 1), glm::vec4(1, 0, 0, 0))/6.f;
        glm::mat4 BSpline = glm::mat4(glm::vec4(-1, 3, -3, 1), glm::vec4(3, -6, 3, 0), glm::vec4(-3, 0, 3, 0), glm::vec4(1, 4, 1, 0))/6.f;
        splineShader.setMat4("B", BSpline);

        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, splineVAO->dataSize);

        if (showLine) {
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", camera.GetViewMatrix());
            simpleShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, splineVAO->dataSize);
        }



        // Cat
        // TODO : render Bezier surfaces using tessellation shader.
        splineShader.use();
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.6f, 0.6f, -2.f));
        model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
        splineShader.setMat4("model", model);

        //glm::mat4 CatmullRom = glm::mat4(glm::vec4(.0f, -0.5f, 1.0f, -0.5f), glm::vec4(1.0f, 0.0f, -2.5f, 1.5f), glm::vec4(0.0f, 0.5f, 2.0f, -1.5f), glm::vec4(0.0f, 0.0f, -0.5f, 0.5f));
        glm::mat4 CatmullRom = glm::mat4(glm::vec4(-1.0f, 3.0f, -3.0f, 1.0f), glm::vec4(2.0f, -5.0f, 4.0f, -1.0f), glm::vec4(-1.0f, 0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 2.0f, 0.0f, 0.0f))/2.0f;
        splineShader.setMat4("B", CatmullRom);

        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, splineVAO->dataSize);

        if (showLine) {
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", camera.GetViewMatrix());
            simpleShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, splineVAO->dataSize);
        }

#pragma endregion
        
#pragma region FONT
        // Bezier
        splineShader.use();
        splineShader.setMat4("B", Bezier);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(-.9f, -0.3f, -2.f));
        model = glm::scale(model, glm::vec3(.1f, .1f, 1.f));
        splineShader.setMat4("model", model);

        glBindVertexArray(fontVAO->ID);
        glDrawArrays(GL_LINES_ADJACENCY, 0, fontVAO->dataSize);

        if (showLine) {
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", camera.GetViewMatrix());
            simpleShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, fontVAO->dataSize);
        }

#pragma endregion

#pragma region LOOP
        // Bezier Does not work.

        // BSpline
        splineShader.use();
        splineShader.setMat4("B", BSpline);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(-.0f, -.2f, -2.f));
        model = glm::scale(model, glm::vec3(.3f, .3f, 1.f));
        splineShader.setMat4("model", model);

        glBindVertexArray(loopVAO->ID);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, loopVAO->dataSize);

        if (showLine) {
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", camera.GetViewMatrix());
            simpleShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, loopVAO->dataSize);
        }


        // Catmull-Rom
        splineShader.use();
        splineShader.setMat4("B", CatmullRom);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(.6f, -.2f, -2.f));
        model = glm::scale(model, glm::vec3(.3f, .3f, 1.f));
        splineShader.setMat4("model", model);

        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, loopVAO->dataSize);

        if (showLine) {
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", camera.GetViewMatrix());
            simpleShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, loopVAO->dataSize);
        }

#pragma endregion

        // tesellation
        if (!fillFace) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.8f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        tessShader.use();
        tessShader.setMat4("model", model);
        tessShader.setMat4("projection", projection);
        tessShader.setMat4("view", camera.GetViewMatrix());
        tessShader.setVec3("cameraPosition", camera.Position);
        tessShader.setFloat("scale", 0.08f);
        

        glBindVertexArray(gumboVAO->ID);
        glDrawArrays(GL_PATCHES, 0, gumboVAO->dataSize);
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.0f, -0.8f, -2.f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        tessShader.setMat4("model", model);
        tessShader.setFloat("scale", 0.1f);


        glBindVertexArray(heartVAO->ID);
        glDrawArrays(GL_PATCHES, 0, teapotVAO->dataSize);

        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.6f, -0.8f, -2.f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        tessShader.setMat4("model", model);
        tessShader.setFloat("scale", 0.1f);

        glBindVertexArray(teapotVAO->ID);
        glDrawArrays(GL_PATCHES, 0, teapotVAO->dataSize);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1,&VAO);
    //glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

bool is9Pressed = false;
bool is0Pressed = false;
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // TODO : 
    // (1) (for spline) if we press key 9, toggle whether to render outer line.

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !is9Pressed) {
        showLine = !showLine;
        is9Pressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE && is9Pressed)
        is9Pressed = false;
    // (2) (Optional, for Bezier surface )if we press key 0, toggle GL_FILL and GL_LINE.
    
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && !is0Pressed) {

        fillFace = !fillFace;
        is0Pressed = true;
    }


    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE && is0Pressed)
        is0Pressed = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}