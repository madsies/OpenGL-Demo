
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
#include "perlin.h"
#include "skybox.h"
#include "texture.h"
#include "shadow.h"
#include "frustum_culling.h"


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
static const GLfloat bgd[] = { 0.35f, 0.35f, 0.35f, 0.8f };
GLuint envCubemap;
GLuint skyShader;
GLuint skyboxVAO;
GLuint boxShader;

// Camera Vars
Camera *camera;
FlyingCamera flyingCamera;

// DirectionalLight
glm::vec3 dirLight = glm::vec3(0.0f);
DirectionalShadow* sunShadow;

// Culling
FCulling* culler;

/*
    Skybox setup
*/
void setupSkybox(GLFWwindow* window)
{
    skyboxVAO = createCube();
    GLuint texture = loadTexture("img/skybox.jpg");

    if (texture == 0) {
        std::cerr << "Failed to load skybox panorama!" << std::endl;
        return;
    }

    glUseProgram(skyShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(skyShader, "panorama"), 0);
}

void renderSkybox(glm::mat4 view, glm::mat4 projection)
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyShader);

    glm::mat4 rotView = glm::mat4(glm::mat3(view)); 
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "view"), 1, GL_FALSE, glm::value_ptr(rotView));
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

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

    glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, glm::value_ptr(camera->Position));
    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, glm::value_ptr(sunShadow->lightPos));
    glUniform3fv(glGetUniformLocation(program, "dirLightDirection"), 1, glm::value_ptr(dirLight));
    glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"),1, GL_FALSE,glm::value_ptr(sunShadow->lightSpaceMatrix));
    
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, sunShadow->depthMap);
    glUniform1i(glGetUniformLocation(program, "shadowMap"), 5);

    glm::mat4 projection = glm::mat4(1.f);
    projection = glm::perspective(glm::radians(85.f), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, .1f, 10000.f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FCulling culler(&projection, &view);

    objManager->renderObjects(program); 
    cubeBatch->draw(program, &culler);
    
    glDisable(GL_CULL_FACE);
    renderSkybox(view, projection);
    glEnable(GL_CULL_FACE);
}

void update(GLuint program, float deltaTime)
{
    static float sunTime = 0.0f;
    sunTime += deltaTime; 

    float speed = 0.1f;
    float angle = sunTime * speed;

    dirLight.x = sin(angle);
    dirLight.y = -0.2f;      
    dirLight.z = cos(angle);

    dirLight = glm::normalize(dirLight);

    sunShadow->updateLightSpaceMatrix(dirLight, camera->Position, camera->Front);
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
        ref->setScale({ 32.0f, 32.0f, 32.0f });
        ref->setPosition({ 250.0f, 20.0f, 250.0f });
    }

    Mesh cube2 = Cube::generate();

    objId = objManager->registerMeshObject(cube2);
    obj = objManager->getObject(objId);

    if (CustomObject* ref = dynamic_cast<CustomObject*>(objManager->getObject(objId)))
    {
        ref->setScale({ 1.0f, 1.0f, 1.0f });
        ref->setPosition({ 5.0f, 0.0f, 0.0f });
    }

    cubeBatch = new InstancedObjectBatch(cube, 1024*1024, glm::vec3(-512.0f, 0.0f, -512.0f), glm::vec3(512.0f, 0.0f, 512.0f));
    for (int i = 0; i < 1024 * 1024; ++i)
    {
        int x = (i % 1024) - 512;
        int z = (i / 1024) - 512;;
        float y = floor((perlinNoiseFractal(x / 512.f, z / 512.f)+1.f) * 256.f - 256.f);
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,z));
        cubeBatch->addInstance(m);
    }
    cubeBatch->finalizeChunks();
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

void renderShadowPass(GLuint shadowShader)
{
    glDisable(GL_MULTISAMPLE);
    glCullFace(GL_FRONT);

    glViewport(0, 0, DirectionalShadow::SHADOW_WIDTH, DirectionalShadow::SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, sunShadow->depthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(shadowShader);

    glUniformMatrix4fv(
        glGetUniformLocation(shadowShader, "lightSpaceMatrix"),
        1, GL_FALSE, glm::value_ptr(sunShadow->lightSpaceMatrix)
    );

    FCulling culler(sunShadow->lightSpaceMatrix);

    objManager->renderObjects(shadowShader);
    cubeBatch->draw(shadowShader, &culler);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);
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


int frame = 0;

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

    

    sunShadow = new DirectionalShadow();

    // Debug
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, 0);

    // Mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    
    // MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    glGenVertexArrays(VAO_COUNT, VAOs);
    glGenBuffers(BUFFER_COUNT, Buffers);
    glGenBuffers(EBO_COUNT, EBOs);

    glEnable(GL_CULL_FACE);   
    glCullFace(GL_BACK);        
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    camera = &flyingCamera;

    glm::mat4 model = glm::mat4(1.0f); // Default model to stop bug
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);

    GLuint program = CompileShader("basic_shader.vert", "basic_shader.frag");
    skyShader = CompileShader("skybox.vert", "skybox.frag");

    createItems();

    GLuint shadowShader = CompileShader("shadow.vert", "shadow.frag");

    setupSkybox(window);
    
    

    // Main Window Loop
    float last = 0.f;

    glEnable(GL_DEPTH_TEST);
    

    while (!glfwWindowShouldClose(window))
    {
        // DeltaTime for updates
        float current = glfwGetTime();
        float deltaTime = current - last;
        last = current;
        updateWindowTitle(window, camera);
        update(program, deltaTime);

        renderShadowPass(shadowShader);
        render(program);
        
        camera->ProcessKeyboardInput(window, deltaTime);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        processKeyboard(window);
    }

}

