#include <iostream>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "oglUtil_OLD/Misc.hpp"

#include "oglUtil/ogluTools.hpp"

#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/LaserSensor.hpp"
#include "Simulation/Env.hpp"


class MainWindow 
: public oglu::Window 
{
public:
    bool active;
    bool freeCamera;
    unsigned int mainRobotIndex;

    std::vector<erppm::RobotBase*> robots;
    std::vector<double> mainRobotInput;
    oglu::Camera mainCamera;

    template <typename T>
    void addRobot(T&& robot);
    template <typename T>
    std::list<T>& getRobotList();
    MainWindow(const int _width, const int _height, const std::string& _name);
    ~MainWindow();
    void mainFunction() override;
    void drawFunction() override;
    void afterResize(const int width, const int height) override;
    void moveObjects();
    void collisions();
    void moveCamera();
};

template <typename T>
std::list<T>& MainWindow::getRobotList(){
    static std::list<T> robots_T;              // it has to be a list, so the pointers won't change when vector relocates data
    return robots_T;
}

template <typename T>
void MainWindow::addRobot(T&& robot){
    std::list<T>& robots_T = getRobotList<T>();
    robots_T.push_back(std::move(robot));
    robots.push_back( &(robots_T.back()) );
}

MainWindow::MainWindow(const int _width, const int _height, const std::string& _name) 
    : oglu::Window(_width, _height, _name, false)
    , active(true)
    , freeCamera(false)
    , mainRobotIndex(0)
    , robots()
    , mainRobotInput()
    , mainCamera(getGlfwWindowPtr())
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glfwSetInputMode(getGlfwWindowPtr(), GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(getGlfwWindowPtr(), GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();

    erppm::env::get().useScenario_demo();

    mainRobotInput.resize(erppm::env::get().robots.getBase()[mainRobotIndex]->getControlInputSize());    

    mainCamera.position.z = 2;
    mainCamera.position.x = -2;
    mainCamera.radius = 13;
    mainCamera.minVerticalRotation = 0.0;
    mainCamera.maxVerticalRotation = glm::half_pi<float>();
}

MainWindow::~MainWindow()
{}

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
        if(keyTracker.isJustPressed(GLFW_KEY_TAB))
        {
            freeCamera =! freeCamera;
        }
        moveObjects();
        collisions();
        moveCamera();
    }
    else if(glfwGetMouseButton( getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS)
    {
		glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        active = true;
	}

}

void MainWindow::drawFunction()
{
    mainCamera.computeMatrices();
    erppm::Floor::drawInstances(mainCamera.getMVP(), mainCamera.position);
    erppm::Wall::drawInstances(mainCamera.getMVP(), mainCamera.position);
    erppm::RoundRobot::draw(mainCamera.getMVP(), mainCamera.position);
    erppm::LaserSensor::draw(mainCamera.getMVP(), mainCamera.position); 
}

void MainWindow::moveObjects(){
    static double time0 = glfwGetTime();
    const double time = glfwGetTime();
    const double dtime = time - time0;
    if(active && !freeCamera)
    {
        if(keyTracker.isJustReleased(GLFW_KEY_ENTER))
        {
            mainRobotIndex = (mainRobotIndex + 1) %  erppm::env::get().robots.getBase().size();
        }
        // Gamepad data
        int countAx;
        // int countBtn;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAx);
        if(countAx > 0){
            mainRobotInput[0] = -(abs(axes[1]) > 0.1 ? axes[1] : 0.0);
            mainRobotInput[1] = -(abs(axes[4]) > 0.1 ? axes[4] : 0.0);
            erppm::env::get().robots.getBase()[mainRobotIndex]->run(dtime, mainRobotInput);
        }
    }
    time0 = time;
}

void MainWindow::collisions(){
    if(active)
    {
        for(std::vector<erppm::RobotBase*>::const_iterator iter = erppm::env::get().robots.getBase().begin(); iter != erppm::env::get().robots.getBase().end(); iter = std::next(iter)){
            (*iter)->collide(erppm::env::get().robots.getBase(), erppm::env::get().walls, erppm::env::get().floor);
            (*iter)->updateSensorsPosition();
        }
        for(std::vector<erppm::RobotBase*>::const_iterator iter = erppm::env::get().robots.getBase().begin(); iter != erppm::env::get().robots.getBase().end(); iter = std::next(iter)){
            (*iter)->measure(erppm::env::get().robots.getBase(), erppm::env::get().walls, erppm::env::get().floor);
        }
    }
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
        if(freeCamera)
        {
            // Warning! camera position cant't be z = 0 
            if(keyTracker.isPressed(GLFW_KEY_W))
            {
                mainCamera.position += mainCamera.getDirection() * 0.02f * mainCamera.position.z;
            }
            if(keyTracker.isPressed(GLFW_KEY_S))
            {
                mainCamera.position -= mainCamera.getDirection() * 0.02f * mainCamera.position.z;
            }
            if(keyTracker.isPressed(GLFW_KEY_A))
            {
                mainCamera.position += mainCamera.getRightVector() * 0.02f * mainCamera.position.z;
            }
            if(keyTracker.isPressed(GLFW_KEY_D))
            {
                mainCamera.position -= mainCamera.getRightVector() * 0.02f * mainCamera.position.z;
            }
            if(keyTracker.isPressed(GLFW_KEY_E))
            {
                mainCamera.position.z += 0.02f * mainCamera.position.z;
            }
            if(keyTracker.isPressed(GLFW_KEY_Q))
            {
                mainCamera.position.z -= 0.02f * mainCamera.position.z;
            }
        }
        else
        {
            // Gamepad data
            int countAx;
            int countBtn;
            const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAx);
            const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countBtn);
            if(countBtn > 0)
            {
                mainCamera.rotate(1000 * dtime * (float)(buttons[14] - buttons[16]), 1000 * dtime * (float)(buttons[13] - buttons[15]));
            }
            if(countAx > 0)
            {
                mainCamera.rotate(((axes[2] + 1.0) - (axes[5] + 1.0))*15, 0);
                static bool followDirection = false;
                if(keyTracker.isJustPressed(GLFW_KEY_SPACE))
                {
                    followDirection = !followDirection;
                }
                if (followDirection)
                {
                    mainCamera.rotate(( mainCamera.getRotation()[0] - erppm::env::get().robots.getBase()[mainRobotIndex]->getRotation()[2]) * 50, 0);
                }
            }
            mainCamera.lookAt(erppm::env::get().robots.getBase()[mainRobotIndex]->getPosition() + glm::vec3(0,0,mainCamera.radius/5 * ((mainCamera.maxVerticalRotation - mainCamera.getRotation().y) / (mainCamera.maxVerticalRotation - mainCamera.minVerticalRotation))));
        }
    }
    time0 = time;
}

void MainWindow::afterResize(const int width, const int height){
    mainCamera.aspect = (float)width/height;
}

int main(int argc, char *argv[])
{
    std::cout <<
        "\t" << "⮜⮞⮝⮟ - Control camera" << "\n" <<
        "\t" << "L3↕ - Control left engine" << "\n" <<
        "\t" << "R3↕ - Control left engine" << "\n" <<
        "\t" << "△ - Change robot" 
    << "\n";
    oglu::setupOglu();
    oglu::setupGlfw();
    MainWindow window(800, 800, "Robot demo");
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