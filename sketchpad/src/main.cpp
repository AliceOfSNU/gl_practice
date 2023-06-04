#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"
#include <iostream>
#include <vector>

#include "stb_image.h"
using namespace std;
bool isWindowed = true;
bool isKeyboardProcessed[1024] = { 0 };
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(-0.7f, 2.5f, 5.0f), glm::vec3(0.0f, 1.0f, 1.0f), 100.f, -30.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = false;
bool enableCameraMovement = false;
bool init_mouse_flag = false;

void main() {
    // glfw: initialize and configure
// ------------------------------'
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    const char* title = "Plane geomtries";
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        //return -1;
    }
    glEnable(GL_DEPTH_TEST);


    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like

    // draw ground

    float ground_vertices[16000]{ 0 };
    float ground_cell_size = 0.6f;
    int ground_divs = 8;
    int GND_VTX_CNT = 0;
    for (int i = 0; i <= ground_divs; ++i) {
        for (int j = 0; j <= ground_divs; ++j) {
            ground_vertices[GND_VTX_CNT++] = 1.0f * (-ground_divs / 2 + i) * ground_cell_size;
            ground_vertices[GND_VTX_CNT++] = 0.0f;
            ground_vertices[GND_VTX_CNT++] = 1.0f * (-ground_divs / 2 + j) * ground_cell_size;
        }
    }

    unsigned int ground_indices[16000]{0};
    int k = 0;
    for (int i = 0; i < ground_divs; ++i) {
        for (int j = 0; j < ground_divs; ++j) {
            int base = i * (ground_divs + 1) + j;
            ground_indices[k++] = base;
            ground_indices[k++] = base + 1;
            ground_indices[k++] = base + (ground_divs + 1);
            ground_indices[k++] = base + (ground_divs + 1);
            ground_indices[k++] = base + 1;
            ground_indices[k++] = base + (ground_divs + 1) + 1;
        }
    }

    unsigned int VBO_G, VAO_G, EBO_G;
    glGenVertexArrays(1, &VAO_G);
    glGenBuffers(1, &VBO_G);
    glGenBuffers(1, &EBO_G);

    glBindVertexArray(VAO_G);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_G);
    glBufferData(GL_ARRAY_BUFFER, GND_VTX_CNT* sizeof(float), ground_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_G);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ground_divs * ground_divs * 6) * sizeof(unsigned int), ground_indices, GL_STATIC_DRAW);


    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // draw shapes

    // can load from file
    unsigned int M = 3;
    unsigned int Ns[10]{ 0 };

    Ns[0] = 3; Ns[1] = 5; Ns[2] = 4;
    float vertices[100] = {
        // triangle
        -0.9f, 0.0f, -0.5f,
        -0.0f, 0.0f, -0.5f,
        -0.45f, 0.0f, 0.5f,
        -0.9f, 0.2f, -0.5f,
        -0.0f, 0.2f, -0.5f,
        -0.45f, 0.2f, 0.5f,

        // star
        -0.3f,   0.0f, -0.4f,
        0.3f,    0.0f, -0.4f,
        0.5f,    0.0f, 0.1f,
        0.0f,    0.0f, 0.4f,
        -0.5f,   0.0f, 0.1f,
        -0.3f,   0.2f, -0.4f,
        0.3f,    0.2f, -0.4f,
        0.5f,    0.2f, 0.1f,
        0.0f,    0.2f, 0.4f,
        -0.5f,   0.2f, 0.1f,

        // random 4-sided 
        0.0f,   0.0f,   0.0f,
        0.2f,   0.0f,   0.0f,
        0.0f,   0.0f,   0.6f,
        -0.4f,  0.0f,   0.2f,
        0.0f,   0.2f,   0.0f,
        0.2f,   0.2f,   0.0f,
        0.0f,   0.2f,   0.6f,
        -0.4f,  0.2f,   0.2f,
    };


    unsigned int VTX_CNT = 0;
    for (int i = 0; i < M; ++i) {
        VTX_CNT += Ns[i];
    }

    unsigned int indices[400]{ 0 };
    unsigned int skip_shape = 0;
    unsigned int j = 0;

    for (int shape = 0; shape < M; ++shape) {
        unsigned int N = Ns[shape];
        for (int i = skip_shape; i < skip_shape+N; ++i) {
            indices[j++] = i + N;
        }
        for (int i = 0; i < N; ++i) { ///loop ccw
            indices[j++] = skip_shape + i;
            indices[j++] = skip_shape + (i + 1) % N;
            indices[j++] = skip_shape + i + N;
            indices[j++] = skip_shape + (i + 1) % N + N;
        }
        skip_shape += 2 * N; //how many idxs to skip

    }


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VTX_CNT * 2 * 3  * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (VTX_CNT * 5) * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //color
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);


    struct Frustum {
        GLfloat FOV = glm::radians(45.f);
        GLfloat aspect = 16 / 9;
        GLfloat nearClippingPlane = 0.1f;
        GLfloat farClippingPlane = 1000.0f;

        glm::mat4 GetPerspective() {
            return glm::perspective(FOV, aspect, nearClippingPlane, farClippingPlane);
        }
    };

    Frustum frustum1 = Frustum();
    frustum1.FOV = glm::radians(camera.Zoom);

    while (!glfwWindowShouldClose(window))
    {
        // time
        // -----
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // shader
        ourShader.use();
        
        // context
        glBindVertexArray(VAO);

        // draw
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 projection = frustum1.GetPerspective();
        ourShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);


        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);

        // draw ground
        glBindVertexArray(VAO_G);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_G);

        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);
        for (int cell = 0; cell < ground_divs * ground_divs; cell++) {
            int i = cell / ground_divs;
            int j = cell % ground_divs;
            if ((i + j) % 2) {
                ourShader.setVec4("MainColor", 1.0f, 1.0f, 1.0f, 1.0f);
            }
            else {
                ourShader.setVec4("MainColor", 0.0f, 0.0f, 0.0f, 1.0f);
            }
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(cell*6 * sizeof(unsigned int)));
        }

        // draw shapes
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        unsigned int skip = 0;
        for (int i = 0; i < M; ++i) {

            //translate
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f*i, 0.0f, 0.0f));
            ourShader.setMat4("model", model);

            // draw top surface
            unsigned int N = Ns[i];
            ourShader.setVec4("MainColor", 0.3*sinf(1.0f * i)+0.5, 0.3*cosf(1.0f * i)+0.5, (float)i/M, 1.0f);
            glDrawElements(GL_TRIANGLE_FAN, N, GL_UNSIGNED_INT, (void*)(skip * sizeof(unsigned int)));


            // draw sides
            ourShader.setVec4("MainColor", 0.3 * sinf(1.0f * i)+0.3, 0.3 * cosf(1.0f * i) + 0.3, (float)i/M/2, 1.0f);
            glDrawElements(GL_TRIANGLE_STRIP, 4 * N, GL_UNSIGNED_INT, (void*)((N+skip) * sizeof(unsigned int)));

            skip += 5*N;
        }
        
        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO_G);
    glDeleteBuffers(1, &VBO_G);

    glfwTerminate();
    //return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);



    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS ) {
        firstMouse = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE & firstMouse) {
        enableCameraMovement = !enableCameraMovement;
        init_mouse_flag = true;
        firstMouse = false;
    }

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
    if (!enableCameraMovement) return;
    if (init_mouse_flag) {
        lastX = xpos;
        lastY = ypos;
        init_mouse_flag = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;

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
