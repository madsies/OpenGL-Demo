
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"
#include "flying_camera.h"
#include "shader.h"
#include "cube.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <sstream>
#include <iomanip> 
#include "objectManager.h"
#include "instancedObjects.h"

// Buffer Consts
constexpr int BUFFER_COUNT = 3;
constexpr int VAO_COUNT = 3;
constexpr int EBO_COUNT = 3;
GLuint Buffers[BUFFER_COUNT];
GLuint VAOs[VAO_COUNT];
GLuint EBOs[EBO_COUNT];

// Window Variables
constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

// Object Management
ObjectManager* objManager = new ObjectManager(Buffers, VAOs, EBOs);
InstancedObjectBatch* cubeBatch;

// Background Col
static const GLfloat bgd[] = { 0.35f, 0.35f, 0.35f, 0.35f };

// Camera Vars
Camera *camera;
FlyingCamera flyingCamera;

void SizeCallback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void processKeyboard(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void render(GLuint program)
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glClearBufferfv(GL_COLOR, 0, bgd);
    glClear(GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(program);

    glm::mat4 view;
    view = glm::lookAt(camera->Position, camera->Position + camera->Front, camera->Up);

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::mat4(1.f);
    projection = glm::perspective(glm::radians(45.f), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, .1f, 50.f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    objManager->renderObjects(program); 

    cubeBatch->draw(program);
}

void createItems()
{
    int objId;
    Object* obj;
    CustomObject* ref;

    Mesh cube = Cube::generate();
    objId = objManager->registerMeshObject(cube);
    obj = objManager->getObject(objId);

    if (CustomObject* ref = dynamic_cast<CustomObject*>(objManager->getObject(objId)))
    {
        ref->setScale({ 4.0f, 4.0f, 4.0f });
        ref->setPosition({ 0.0f, 0.0f, 0.0f });
    }

    Mesh cube2 = Cube::generate();

    objId = objManager->registerMeshObject(cube2);
    obj = objManager->getObject(objId);

    if (CustomObject* ref = dynamic_cast<CustomObject*>(objManager->getObject(objId)))
    {
        ref->setScale({ 1.0f, 1.0f, 1.0f });
        ref->setPosition({ 5.0f, 0.0f, 0.0f });
    }

    cubeBatch =  new InstancedObjectBatch(cube, VAOs[2], Buffers[2], EBOs[2], 50*50);
    for (int i = 0; i < 50 * 50; ++i) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3((i * 1) % 50 , sin(i) * cos(i), (i / 50)));
        cubeBatch->addInstance(m);
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    static float lastX = SCREEN_WIDTH / 2.0f;
    static float lastY = SCREEN_HEIGHT / 2.0f;
    static bool first = true; 

    if (first)
    {
        lastX = xpos;
        lastY = ypos;
        first = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (camera) camera->ProcessMouseMovement(xoffset, yoffset);
}

/*
    Temporary viewer for testing.
*/
void updateWindowTitle(GLFWwindow* window, const Camera* cam)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "OpenGL Test - Camera Pos: ("
        << cam->Position.x << ", "
        << cam->Position.y << ", "
        << cam->Position.z << ") | Looking At: ("
        << cam->Front.x << ", "
        << cam->Front.y << ", "
        << cam->Front.z << ")";

    glfwSetWindowTitle(window, ss.str().c_str());
}

/*
    Printing OpenGL Errors to console
*/

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        fprintf(stderr, "ERROR > %s\n", message);
    }
}


int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Demo", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, SizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Debug
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, 0);

    // Mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);


    glGenVertexArrays(VAO_COUNT, VAOs);
    glGenBuffers(BUFFER_COUNT, Buffers);
    glGenBuffers(EBO_COUNT, EBOs);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    camera = &flyingCamera;

    glm::mat4 model = glm::mat4(1.0f); // Default model to stop bug
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);

    GLuint program = CompileShader("basic_shader.vert", "basic_shader.frag");
    
    createItems();

    // Main Window Loop
    float last = 0.f;

    while (!glfwWindowShouldClose(window))
    {
        // DeltaTime for updates
        float current = glfwGetTime();
        float deltaTime = current - last;
        updateWindowTitle(window, camera);

        last = current;

        render(program);
        camera->ProcessKeyboardInput(window, deltaTime);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        processKeyboard(window);
    }

}

