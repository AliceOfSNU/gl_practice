#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "gameobjects.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "text_renderer.h"
#include <string>

/*
use A <<< left
    D >>> right
to move bar
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Bar& bar, float dt);

// setting
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;

bool previousKeyState[1024];

int main()
{
    
    // glfw: initialize and configure
    // ------------------------------'
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


    /////////////////////////////////////////////////////
    // TODO : Define VAO and VBO for triangle and quad(bar).
    /////////////////////////////////////////////////////
    GeometryPrimitives triangle{
        triangle_position_colors,
        triangle_indices
    };

    GeometryPrimitives quad{
        quad_positions_colors,
        quad_indices
    };



    unsigned int VAOtris, VBOtris;

    glGenVertexArrays(1, &VAOtris);
    glGenBuffers(1, &VBOtris);

    glBindVertexArray(VAOtris);
    glBindBuffer(GL_ARRAY_BUFFER, VBOtris);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle.vertices[0]) * 18 , triangle.vertices, GL_STATIC_DRAW);

    // read the positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // read the colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    std::vector<Entity> entities;

    unsigned int VAObar, VBObar, EBObar;

    glGenVertexArrays(1, &VAObar);
    glGenBuffers(1, &VBObar);

    glBindVertexArray(VAObar);
    glBindBuffer(GL_ARRAY_BUFFER, VBObar);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad.vertices[0]) * 24, quad.vertices, GL_STATIC_DRAW);
    
    // read the positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // read the colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // create element buffer
    glGenBuffers(1, &EBObar);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBObar);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad.indices[0]) * 6, quad.indices,
        GL_STATIC_DRAW);

    TextRenderer  *Text;
    Text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
    Text->Load("../fonts/OCRAEXT.TTF", 24);

    Bar bar{ 0,0,0 };
    bar.speed = 1.0f;

    // render loop
    // -----------
    float generationInterval = 0.3f;
    float dt = 0.0f;
    float currTime = glfwGetTime();
    float lastSpawnTime = 0.0f;
    int score = 0;
    int displayCount = 0;

    float barWidth = 0.3f;
    float barHeight = 0.05f;
    float barYPosition = -0.8f;
    
    glm::vec3 downVector = glm::vec3(0.0f, -1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, bar, dt);

        // clear background
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        /////////////////////////////////////////////////////
        // TODO : Main Game Loop
        /////////////////////////////////////////////////////

        // (1) generate new triangle at the top of the screen for each time interval
        dt = glfwGetTime() - currTime;
        currTime = glfwGetTime();
        if (currTime - lastSpawnTime > generationInterval) {
            entities.push_back(getRandomEntity());
            lastSpawnTime = currTime;
        }

        // (2) make triangles fall and rotate!!!!!
        // (3) Every triangle has different shape, falling speed, rotating speed.
        
        
        glBindVertexArray(VAOtris);
        ourShader.use();


        for (auto it = entities.begin(); it !=  entities.end(); ++it) {
            Entity go = *it;
            it -> position = it -> position + it -> dropSpeed * dt * downVector;

            it->rotate = it->rotate + it->rotateSpeed * dt;
            if(it->rotate > 360.f) it-> rotate -= 360;

            ourShader.setVec3("size", (it->scale)* glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setVec3("offset", it->position);
            ourShader.setFloat("rotateZ", it->rotate);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // remove all triangles outside view
        entities.erase(std::remove_if(entities.begin(), entities.end(), [](Entity go){ return go.position.y < -1.0f; }), entities.end());
        
        // (4) Render a red box
        glBindVertexArray(VAObar);

        ourShader.setVec3("size", glm::vec3(barWidth, barHeight, 0));
        ourShader.setVec3("offset", glm::vec3(bar.xPosition, barYPosition, 0));
        ourShader.setFloat("rotateZ", 0.0f);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        

        // (5) Implement simple collision detection. Details are on the pdf file.

        auto it = entities.begin();
        while (it != entities.end()) {
            if (
                (it->position.y < barYPosition + barHeight / 2.0f)
                && (it->position.y > barYPosition - barHeight / 2.0f)
                && (it->position.x < bar.xPosition + barWidth / 2.0f)
                && (it->position.x > bar.xPosition - barWidth / 2.0f)
                ) {
                it = entities.erase(it);
                score++;
            }
            else {
                ++it;
            }
        }

        // (6) Visualize the score & print score (you can use TextRenderer)
        Text->RenderText("score: " + std::to_string(score), 0.8f, 0.9f, 1.0f);

        // draw triangles.
        glBindVertexArray(VAOtris);
        ourShader.use();

        displayCount = score % 20;
        for (int i = 0; i < displayCount; ++i) {
            ourShader.setVec3("size", glm::vec3(0.05f, 0.05f, 1.0f));
            ourShader.setVec3("offset", glm::vec3(-0.9f + i * 0.1f, -0.9f , 0.0f));
            ourShader.setFloat("rotateZ", 0.0f);

            glDrawArrays(GL_TRIANGLES, 0, 3);
            
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Bar& bar, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //////////////////////////////////
    // TODO : make the bar movable (consider interval time dt!)

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && bar.xPosition < 1.0f)
        bar.xPosition = bar.xPosition + bar.speed * dt;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && bar.xPosition > -1.0f)
        bar.xPosition = bar.xPosition - bar.speed * dt;

    //////////////////////////////////

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
