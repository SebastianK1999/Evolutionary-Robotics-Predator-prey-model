#include <iostream>
#include <filesystem>
#include <utility>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "oglUtil_OLD/Cube.hpp"
#include "oglUtil_OLD/Mesh.hpp"

#include "oglUtil/ogluTools.hpp"
#include "oglUtil/drawables/Cube.hpp"
#include "oglUtil/drawables/Plane.hpp"
#include "oglUtil/drawables/Sphere.hpp"
#include "oglUtil/drawables/Mesh.hpp"

#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/LaserSensor.hpp"

#define RENDER_FLOOR    true
#define RENDER_WALL     true
#define RENDER_CUBES    true
#define RENDER_PLANE    true
#define RENDER_SPHERE   true
#define RENDER_MESH     true
#define RENDER_ROBOT    true


class MainWindow 
: public oglu::Window 
{
public:
    bool active;
#if RENDER_FLOOR
    std::shared_ptr<erppm::Floor> floor;
#endif
#if RENDER_WALL
    erppm::Wall wall;
#endif
#if RENDER_CUBES
    std::vector<oglu::Cube> cubes;
#endif
#if RENDER_PLANE
    oglu::Plane plane;
#endif
#if RENDER_SPHERE
    oglu::Sphere sphere;
#endif
#if RENDER_MESH
    oglu::Mesh palmMesh;
#endif
#if RENDER_ROBOT
    erppm::RoundRobot robot;
#endif

    MainWindow(const int _width, const int _height, const std::string& _name);
    ~MainWindow();
    oglu::Camera mainCamera;
    void mainFunction() override;
    void drawFunction() override;
    void moveCamera();
};

MainWindow::~MainWindow()
{}

MainWindow::MainWindow(const int _width, const int _height, const std::string& _name) 
    : oglu::Window(_width, _height, _name, false)
    , active(true)
#if RENDER_FLOOR
    , floor(new erppm::Floor)
#endif
#if RENDER_WALL
    , wall()
#endif
#if RENDER_CUBES
    , cubes()
#endif
#if RENDER_PLANE
    , plane()
#endif
#if RENDER_SPHERE
    , sphere()
#endif
#if RENDER_MESH
    , palmMesh(std::filesystem::path() / ".." / "res" / "obj_models" / "palm_tree")
#endif
#if RENDER_ROBOT
    , robot()
#endif
    , mainCamera(getGlfwWindowPtr())
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    // glClearDepth(100);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glfwSetInputMode(getGlfwWindowPtr(), GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(getGlfwWindowPtr(), GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();

    const glm::vec4 wallsColor(0.6, 0.6, 0.6, 1.0); 
#if RENDER_FLOOR
    floor->getScale().y = 30;
    floor->getScale().x = 30;
    floor->getColor() = wallsColor;
#endif
#if RENDER_WALL
    wall.getScale().x = floor->getScale().x; 
    wall.getScale().y *= 15; 
    wall.getScale().z *= 3; 
    wall.getPosition().y = -floor->getScale().y - wall.getScale().y; 
    wall.getColor() = wallsColor;
#endif
#if RENDER_CUBES
    // oglu::Cube::staticRegistryPointer->reserve(1000);
    cubes.reserve(1000);
    for(int i = 0; i < cubes.capacity(); i++)
    {
        cubes.emplace_back(std::move(oglu::Cube()));
        oglu::Cube& cube = cubes.back();
        cube.getPosition().x = i * 0.001;
        cube.getPosition().y = i * 0.001;
        cube.getPosition().z = i * 0.001;
        cube.getScale() *= 0.5f * ((float)i) / ((float)cubes.capacity());
        cube.getRotation().x = glm::half_pi<float>() / 2 * ((float)i) / ((float)cubes.capacity());
        cube.getRotation().y = glm::half_pi<float>() / 2 * ((float)i) / ((float)cubes.capacity());
        cube.getRotation().z = glm::half_pi<float>() / 2 * ((float)i) / ((float)cubes.capacity());
        cube.getColor().r = 0.0f;
        cube.getColor().g = (1.0f - (float)i / (float) cubes.capacity());
        cube.getColor().b = (float)i / (float) cubes.capacity();
    }
#endif
#if RENDER_PLANE
    plane.getPosition().z = 4;
    plane.getColor().r = 1;
    plane.getColor().g = 0;
    plane.getColor().b = 0;
    plane.getScale() *= 3;
    plane.getRotation().x = glm::pi<float>() / 4.0f;
    plane.getRotation().z = glm::pi<float>() / 4.0f;
#endif
#if RENDER_SPHERE
    sphere.getPosition().x = -2;
    sphere.getPosition().y = -2;
    sphere.getPosition().z = 2;
    sphere.getColor() = {0.1f, 0.3f, 0.8f, 0.3f};
#endif
#if RENDER_MESH
    palmMesh.getPosition().x = 5;
    palmMesh.getScale() *= 0.02;
#endif
#if RENDER_ROBOT
    robot.getPosition().x = 2;
    robot.getPrimaryColor() = {0.2, 0.2, 0.6, 1};
    robot.updateSensorsPosition();
    float anglePosition;
    float angleOnRobot = 0;
    anglePosition = 0;
    robot.addSensor
    (
        erppm::LaserSensor
        (
            glm::vec3(
                glm::cos(anglePosition),
                glm::sin(anglePosition),
                0.2
            ),
            glm::vec3(
                0,
                0,
                anglePosition + angleOnRobot
            ),
            {anglePosition, angleOnRobot}
        )
    );
#endif

    mainCamera.position.z = 2;
    mainCamera.position.x = -2;
    mainCamera.radius = 13;
    mainCamera.minVerticalRotation = 0.0;
    mainCamera.maxVerticalRotation = glm::half_pi<float>();
}

void MainWindow::mainFunction()
{
    if(active)
    {
        if (keyTracker.isJustPressed(GLFW_KEY_ESCAPE))
        {
            glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            active = false;
        }
        if(keyTracker.isJustPressed(GLFW_KEY_F))
        {
            std::cout << "Respect paid!\n";
        }
        if(keyTracker.isJustPressed(GLFW_KEY_F11))
        {
            switchFullScreen();
        }
        if(keyTracker.isJustReleased(oglu::KeyBundle{GLFW_KEY_F, GLFW_KEY_F11}))
        {
            std::cout << "Wait, that's illegal!\n";
        }
#if RENDER_ROBOT
        robot.getRotation().z += 0.01;
        robot.updateSensorsPosition();
#endif
    }
    else if(glfwGetMouseButton( getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS)
    {
		glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        active = true;
	}

    moveCamera();
}

void MainWindow::drawFunction()
{
    mainCamera.computeMatrices();
#if RENDER_FLOOR
    erppm::Floor::drawInstances(mainCamera.getMVP(), mainCamera.position);
#endif
#if RENDER_WALL
    erppm::Wall::drawInstances(mainCamera.getMVP(), mainCamera.position);
#endif
#if RENDER_CUBES
    oglu::Cube::drawInstances(mainCamera.getMVP(), mainCamera.position);
#endif
#if RENDER_PLANE
    oglu::Plane::drawInstances(mainCamera.getMVP(), mainCamera.position);
#endif
#if RENDER_SPHERE
    oglu::Sphere::drawInstances(mainCamera.getMVP(), mainCamera.position);
#endif
#if RENDER_MESH
    oglu::Mesh::drawInstances(std::filesystem::path() / ".." / "res" / "obj_models" / "palm_tree", mainCamera.getMVP(), mainCamera.position);
#endif
#if RENDER_ROBOT
    erppm::RoundRobot::draw(mainCamera.getMVP(), mainCamera.position);
    erppm::LaserSensor::draw(mainCamera.getMVP(), mainCamera.position);
#endif
}

void MainWindow::moveCamera()
{
    constexpr float cameraMouseSensitivity = 20;
    static double time0 = glfwGetTime();
    const double time = glfwGetTime();
    const double dtime = time - time0;
    if(active)
    {
        // Get mouse position
        int xSize;
        int ySize;
        glfwGetWindowSize(getGlfwWindowPtr(), &xSize, &ySize);
        double xPos;
        double yPos;
        glfwGetCursorPos(getGlfwWindowPtr(), &xPos, &yPos);
        glfwSetCursorPos(getGlfwWindowPtr(), xSize/2, ySize/2);
        mainCamera.rotate((xPos - xSize/2) * cameraMouseSensitivity * dtime, (yPos - ySize/2) * cameraMouseSensitivity * dtime);

        // camera position cant't be z = 0 
        if(keyTracker.isPressed(GLFW_KEY_W))
        {
            mainCamera.position += mainCamera.getDirection() * 0.05f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_S))
        {
            mainCamera.position -= mainCamera.getDirection() * 0.05f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_A))
        {
            mainCamera.position += mainCamera.getRightVector() * 0.05f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_D))
        {
            mainCamera.position -= mainCamera.getRightVector() * 0.05f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_E))
        {
            mainCamera.position.z += 0.05f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_Q))
        {
            mainCamera.position.z -= 0.05f * mainCamera.position.z;
        }
    }
    
    time0 = time;
}

int main(int argc, char *argv[])
{
    oglu::setupOglu();
    oglu::setupGlfw();
    MainWindow window(800, 600, "Empty window");
    oglu::setupGlew(false);
    
    window.keyTracker.addKey(GLFW_KEY_F);
    window.keyTracker.addKey(GLFW_KEY_F11);
    window.keyTracker.addKey(GLFW_KEY_ESCAPE);
    window.keyTracker.addKey(GLFW_KEY_TAB);
    window.keyTracker.addKey(GLFW_KEY_ENTER);
    window.keyTracker.addKey(GLFW_KEY_SPACE);
    window.keyTracker.addKey(GLFW_KEY_W);
    window.keyTracker.addKey(GLFW_KEY_S);
    window.keyTracker.addKey(GLFW_KEY_A);
    window.keyTracker.addKey(GLFW_KEY_D);
    window.keyTracker.addKey(GLFW_KEY_Q);
    window.keyTracker.addKey(GLFW_KEY_E);
    
    window.mainLoop();

    return 0;
}