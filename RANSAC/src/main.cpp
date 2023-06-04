#include "GLVis.h"
#include "PointCloud.hpp"
#include "PointKDTree.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// run with file
// C:\OpenGLProjects\graphicsProgramming2022\assignments\RANSAC\data\cube.pts

using namespace std;
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;



// view setting
bool isWindowed = true;
bool isKeyboardProcessed[1024] = { 0 };
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// input
bool firstMouse = false;
bool enableCameraMovement = false;
bool init_mouse_flag = false;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
//view
Frustum frustum1 = Frustum();
//frustum1.FoV = glm::radians(camera.Zoom);
Scene scene{};
Camera camera{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0, 0.0};



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    const char* title = "Point Cloud";
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

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        //return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);

    // compute segment
    AxisAlignedBox3 aabb(Vector3d(0.0, 0.0, 0.0), Vector3d(1.0, 1.0, 1.0));
    Slab slab{ 2.0, 2.0, 0.4 };
    slab.position[1] = 1.0;
    slab.rotation[1] = 45;



    // build and compile our shader program
    scene.camera = &camera;
    scene.frustum = frustum1;
    Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like
    slab.ourShader = ourShader;
    slab.updateView();


    // init pcloud
	PointCloud pcloud;
	string datapath = "";
	cout << "Hello, type file to load" << endl;
	cin >> datapath;
	if (!pcloud.load(datapath)) {
		cout << "cannot load file " << datapath << endl;
		return -1;
	}
    pcloud.updateView();
    
	cout << "pcloud size:" << pcloud.size() << endl;

	pcloud.bbox.Print();
    vector<Point*> slab_points;
    pcloud.ransac(1, 0.1, 6, slab, slab_points);
    slab.updateView();
    // intersection of aabb test
    //AxisAlignedBox3 abox(Vector3d(0.0, 0.0, 0.0), Vector3d(1.0, 1.0, 1.0));
    //vector<pair<Vector3d, Vector3d>> tests{
    //    {Vector3d(0.0, 0.0, 0.0), Vector3d(1.0, 1.0, 1.0)}, //true
    //    {Vector3d(0.5, 0.5, 0.5), Vector3d(1.5, 1.5, 1.5)}, //true
    //    {Vector3d(0.0, 1.1, 0.0), Vector3d(1.0, 2.0, 1.0)}, //false
    //    {Vector3d(0.3, 0.3, 0.3), Vector3d(0.6, 0.6, 0.6)}, //true
    //    {Vector3d(1.1, 1.1, 1.1), Vector3d(1.2, 1.2, 1.2)}, //false
    //};
    //
    //for (pair<Vector3d, Vector3d> vv : tests) {
    //    AxisAlignedBox3 bbox(vv.first, vv.second);
    //    cout << (abox.Intersects(bbox)? "TRUE": "FALSE") << endl;
    //}

    float vertices[72] = {
        // triangle
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * 9 * sizeof(float), vertices, GL_STATIC_DRAW);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //drawing order(just the sides)
    glGenBuffers(1, &EBO);
    unsigned int order[24]{
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7,
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(unsigned int), order, GL_STATIC_DRAW);


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


        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            isKeyboardProcessed[GLFW_KEY_RIGHT] = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE && isKeyboardProcessed[GLFW_KEY_RIGHT]) {
            //do it again
            pcloud.ransac(1, 0.05, 6, slab, slab_points);
            slab.updateView();
            isKeyboardProcessed[GLFW_KEY_RIGHT] = 0;
        }
        // move slab
        //if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        //    slab.position -= Vector3d(1.0, 0.0, 0.0) * deltaTime;
            //slab.position += Vector3d(1.0, 0.0, 0.0) * deltaTime;
        //if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        //    slab.position -= Vector3d(0.0, 1.0, 0.0) * deltaTime;
        //if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        //    slab.position += Vector3d(0.0, 1.0, 0.0) * deltaTime;
        //if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        //    slab.rotation += Vector3d(1.0, 0.0, 0.0) * 50 * deltaTime;
        //if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        //    slab.rotation += Vector3d(0.0, 1.0, 0.0) * 50 * deltaTime;


        slab.draw(scene);
        pcloud.draw(scene);
        // shader
        ourShader.use();
        ourShader.setVec4("MainColor", 1.0f, 0.0f, 0.0f, 1.0f);
        
        // context
        glBindVertexArray(VAO);
        
        ourShader.use();
        // draw
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 projection = glm::perspective(frustum1.FoV, frustum1.aspect, frustum1.nearClippingDistance, frustum1.farClippingDistance);
        ourShader.setMat4("projection", projection);
        
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);
        
        //draw points
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, 8);

        // draw lines
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();

}

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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
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
//int
//main(int argc, char * argv[])
//{
//  if (argc < 2)
//    return usage(argc, argv);
//
//  bool ransac = false;
//  long num_planes = -1;
//  long num_iters = -1;
//  Real thickness = 0;
//  long min_points = -1;
//
//  bool downsample = false;
//
//  std::string in_path;
//  std::string out_path;
//
//  int pos_arg = 0;
//  for (int i = 1; i < argc; ++i)
//  {
//    std::string arg = argv[i];
//    if (arg == "--ransac")
//    {
//      ransac = true;
//      if (i + 4 >= argc)
//        return usage(argc, argv);
//
//      num_planes = std::atol(argv[++i]);
//      num_iters  = std::atol(argv[++i]);
//      thickness  = std::atof(argv[++i]);
//      min_points = std::atol(argv[++i]);
//
//    }
//    else if (arg == "--downsample")
//    {
//      downsample = true;
//
//    }
//    else if (pos_arg == 0)
//    {
//      in_path = arg;
//      pos_arg++;
//    }
//    else if (pos_arg == 1)
//    {
//      out_path = arg;
//      pos_arg++;
//    }
//    else
//      return usage(argc, argv);
//  }
//
//  if (pos_arg <= 0)
//    return usage(argc, argv);
//
//  if (downsample && !ransac)
//  {
//    return usage(argc, argv);
//  }
//
//  PointCloud pcloud;
//  if (!pcloud.load(in_path))
//    return -1;
//
//
//  std::vector<Slab> slabs;
//  bool save = false;
//  if (ransac)
//  {
//    Real abs_thickness = thickness * pcloud.getAABB().getExtent().length();
//    pcloud.ransacMultiple(num_planes, num_iters, abs_thickness, min_points, slabs);
//
//    for (size_t i = 0; i < slabs.size(); ++i)
//      DGP_CONSOLE << "[RANSAC]   - Plane " << i << ": " << slabs[i].getPlane().toString();
//
//
//
//    if (downsample)
//    {
//      pcloud.adaptiveDownsample(slabs);
//      save = true;
//    }
//  }
//
//  if (save && !out_path.empty())
//  {
//    if (!pcloud.save(out_path))
//      return -1;
//
//    DGP_CONSOLE << "Saved point cloud to " << out_path;
//  }
//
//  Viewer viewer;
//  viewer.setObject(&pcloud);
//  viewer.setSlabs(slabs);
//  viewer.launch(argc, argv);
//
//  return 0;
//}
//