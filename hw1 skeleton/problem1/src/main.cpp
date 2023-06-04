#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>

/*
use WASD for control, R for rotation
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float dt);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// global variables
// movement
glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

// rotation
float rotation = 0.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions
        // bottom right      
         0.5f, -0.25f, 0.0f,  
        // bottom left
        -0.5f, -0.25f, 0.0f, 
        // top
         0.0f,  0.5f, 0.0f,  
    };

    unsigned int VBO, VAO;
    /**********Fill in the blank*********/
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // read the positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // read the colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    /*************************************/
    float timeValue = glfwGetTime();
    float lastFrameTimeValue = timeValue;
    float blue = 0.0f;

    // uniform variables
    int vertexColorLocation = glGetUniformLocation(ourShader.ID, "ourColor");
    
    // Time
    float deltaTime = 0.f;
    float originTime = timeValue;
    float cumTime = 0.f;

    // offset
    glm::vec3 offset = glm::vec3(0.f, 0.f, 0.f);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        ourShader.use();

        // calculate current and delta time
        deltaTime = timeValue;
        timeValue = glfwGetTime();
        deltaTime = timeValue - deltaTime;
        cumTime = timeValue - originTime;
        /**********Fill in the blank*********/
        
        // change color
        if (timeValue - lastFrameTimeValue > 1.0f) {
            lastFrameTimeValue = timeValue;
            blue = blue < 0.5f ? 1.0f: 0.0f;
        }

        glUniform4f(vertexColorLocation, 0.0f, 0.0f, blue, 1.0f);

        // update offset
        offset = offset + velocity * deltaTime;
        ourShader.setVec3("offset", offset);

        // update rotation
        ourShader.setFloat("rotateZ", rotation);


        /*************************************/

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /**********Fill in the blank*********/
    velocity.x = velocity.y = 0.f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity.y = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocity.y = -1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocity.x = -1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocity.x = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rotation -= dt;
        if (rotation < -360.f) rotation += 360.f;    
    }
    /*************************************/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
