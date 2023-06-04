#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "texture.h"
#include "texture_cube.h"
#include "math_utils.h"
#include <map>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool isWindowed = true;
bool isKeyboardProcessed[1024] = {0};

// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(4.0f, 1.0f, -4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// dayfactor
float dayfactor = 0.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2018-19427 HanjunKim", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // define normal shader and skybox shader.
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like
    Shader skyboxShader("../shaders/shader_skybox.vs","../shaders/shader_skybox.fs");


    // TODO : define required VAOs(textured cube, skybox, quad)
    // data are defined in geometry_primitives.h
    // DEFINE BOXES
    // -----
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions_textures), cube_positions_textures, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f,  2.2f, -2.5f),
        glm::vec3(-3.8f,  2.0f, -12.3f),
        glm::vec3( 2.4f,  1.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  1.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // DEFINE SKYBOX
    // -----
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_positions), skybox_positions, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // DEFINE GROUND
    // -----
    unsigned int quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_positions_textures),quad_positions_textures, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices,
        GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    // DEFINE GRASS
    // -----
    const int n_grass = 1000;
    float grassGroundSize = 20;
    glm::vec3 grassPositions[n_grass];

    // positions of our grasses
    for(int i=0; i<n_grass; i++){
        float s = grassGroundSize/2;
        float x = getRandomValueBetween(-s, s);
        float z = getRandomValueBetween(-s, s);
        grassPositions[i].x = x;
        grassPositions[i].y = 0.5f;
        grassPositions[i].z = z;
    }


    // TODO : define textures (container, grass, grass_ground) & cubemap textures (day, night)
        // load and create a texture 
    // -------------------------
    unsigned int container_texture;
    // container
    // ---------
    glGenTextures(1, &container_texture);
    glBindTexture(GL_TEXTURE_2D, container_texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("../resources/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // skybox texture
    // -----
    stbi_set_flip_vertically_on_load(false);

    const std::vector<std::string> faces{
        "../resources/Sky Textures/right.jpg",
        "../resources/Sky Textures/left.jpg",
        "../resources/Sky Textures/top.jpg",
        "../resources/Sky Textures/bottom.jpg",
        "../resources/Sky Textures/front.jpg",
        "../resources/Sky Textures/back.jpg"
    };
    
    const std::vector<std::string> faces2{
        "../resources/Night Sky Textures/nightRight.png",
        "../resources/Night Sky Textures/nightLeft.png",
        "../resources/Night Sky Textures/nightTop.png",
        "../resources/Night Sky Textures/nightBottom.png",
        "../resources/Night Sky Textures/nightBack.png",
        "../resources/Night Sky Textures/nightFront.png"
    };

    
    CubemapTexture cubemapDay = CubemapTexture(faces);
    CubemapTexture cubemapNight = CubemapTexture(faces2);

    // ground texture
    // -----
    unsigned int ground_texture;
    glGenTextures(1, &ground_texture);
    glBindTexture(GL_TEXTURE_2D, ground_texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("../resources/grass_ground.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load ground texture" << std::endl;
    }
    stbi_image_free(data);


    // ground texture
    // -----
    unsigned int grass_texture;
    glGenTextures(1, &grass_texture);
    glBindTexture(GL_TEXTURE_2D, grass_texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    data = stbi_load("../resources/grass.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load grass texture" << std::endl;
    }
    std::cout << "loaded png with channels: " << nrChannels << std::endl;
    stbi_image_free(data);

    // TODO : set texture & skybox texture uniform value (initialization)
    // e.g) shader.use(), shader.setInt("texture", 0);
    // shader.use();
    // skyboxShader.use();


    //test if camera-constructor works.
    //camera = Camera(glm::vec3(4.0f, 1.0f, -4.0f), glm::vec3(0.f, 1.f, 0.f), 0.f, 0.f);
    
    // render loop
    // -----------

    while (!glfwWindowShouldClose(window)){

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


        /////////////////////////////////////////////////////
        // TODO : Main Rendering Loop
        /////////////////////////////////////////////////////

        // pass projection matrix to shader (note that in this case it could change every frame)
        GLfloat FoV = glm::radians(camera.Zoom);
        GLfloat aspect = 16 / 9;
        GLfloat nearClippingDistance = 0.1f;
        GLfloat farClippingDistance = 1000.0f;

        glm::mat4 projection = glm::perspective(FoV, aspect, nearClippingDistance, farClippingDistance);

        // (4) render skybox using skybox shader.
        // dont use depth for skybox - for rendering in background
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        skyboxShader.setFloat("dayfactor", dayfactor);

        glBindVertexArray(skyboxVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapDay.textureID);
        skyboxShader.setInt("skyboxDay", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapNight.textureID);
        skyboxShader.setInt("skyboxNight", 1);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // (1) render boxes(cube) using normal shader.
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", camera.GetViewMatrix());

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, container_texture);

        // set model matrices
        for (unsigned int i = 0; i < 10; ++i) {
            glm::mat4 model = glm::mat4(1.f);
            // some random angle
            float angle = 20.f * i;
            // composition of rotation about some random axis
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.4f));
            model = glm::translate(model, cubePositions[i]);

            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // (2) render ground(quad) using normal shader.
        glm::mat4 model = glm::mat4(1.f);
        float angle = 90.f;
        // placing on the ground
        model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));

        shader.setMat4("model", model);
        
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, ground_texture);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // (3) render billboard grasses(quad) using normal shader.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, grass_texture);
        
        // sort order
        std::map<float, glm::vec3> sortedGrass;
        for (unsigned int i = 0; i < n_grass; i++)
        {
            float distance = glm::length(camera.Position - grassPositions[i]);
            sortedGrass[distance] = grassPositions[i];
        }

        angle = atan(camera.Front.x / camera.Front.z);
        for (std::map<float, glm::vec3>::reverse_iterator it = sortedGrass.rbegin(); it != sortedGrass.rend(); ++it)
        {
            model = glm::mat4(1.f);
            model = glm::translate(model, it->second);
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        /*
         
        for (unsigned int i = 0; i < n_grass; ++i) {
            model = glm::mat4(1.f);
            // composition of rotation about some random axis
            float angle = 20.f;
            model = glm::translate(model, grassPositions[i]);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        */
        glDisable(GL_BLEND);


        // rendering pseudo-code

        // update projection, view matrix to shader
        // for each model:
        //      bind VAO, texture
        //      for each entity that belongs to the model:
        //          update model(transformation) matrix to shader
        //          glDrawArrays or glDrawElements




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // TODO : make camera movable (WASD) & increase or decrease dayFactor(press O: increase, press P: decrease)
    const float cameraSpeed = 0.05f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime); 
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_O)) dayfactor += deltaTime * 0.3f;
    if (glfwGetKey(window, GLFW_KEY_P)) dayfactor -= deltaTime * 0.3f;
    if (dayfactor < 0.f) dayfactor = 0.0f;
    if (dayfactor > 1.f) dayfactor = 1.0f;
    
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
    // TODO: calculate how much cursor have moved, rotate camera proportional to the value, using ProcessMouseMovement.
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
