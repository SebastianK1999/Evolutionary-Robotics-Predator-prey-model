#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <list>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "whereami.h"
#include "oglUtil/oglu.hpp"
#include "oglUtil_OLD/Cube.hpp"
#include "oglUtil_OLD/Directions.hpp"
#include "oglUtil_OLD/Mesh.hpp"
#include "oglUtil_OLD/Misc.hpp"

#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/LaserSensor.hpp"


Mesh* meshPtr;
erppm::Floor* floorPtr;

class MainWindow 
: public oglu::Window 
{
public:
    MainWindow(const int _width, const int _height, const std::string& _name);
    ~MainWindow();
    void mainFunction() override;
    void drawFunction() override;
    void afterResize(const int width, const int height) override;
    void moveObjects();
    void collisions();
    template <typename T>
    void addRobot(T&& robot);
    template <typename T>
    std::list<T>& getRobotList();
    Directions dir;
    erppm::Floor floor;
    erppm::Wall wallPlusY; 
    erppm::Wall wallMinusY;
    erppm::Wall wallPlusX; 
    erppm::Wall wallMinusX;
    std::vector<erppm::RobotBase*> robots;
    std::vector<erppm::Wall> walls;
    std::vector<double> mainRobotInput;
    oglu::Camera mainCamera;
    oglu::Camera coordinatesCamera;
    bool active;
    bool freeCamera;
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

MainWindow::~MainWindow(){

}

MainWindow::MainWindow(const int _width, const int _height, const std::string& _name) 
: oglu::Window(_width, _height, _name, false)
, dir()
, floor()
, wallPlusY() 
, wallMinusY()
, wallPlusX() 
, wallMinusX()
, walls()
, mainRobotInput()
, mainCamera(getGlfwWindowPtr())
, coordinatesCamera(getGlfwWindowPtr())
, active(true)
, freeCamera(false)
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
    const glm::vec3 wallsColor(0.6); 
    floor.scale.y = 30;
    floor.scale.x = 30;
    floor.primaryColor = wallsColor;
    for(int i =0;i < 5; i++){
        walls.emplace_back(erppm::Wall());
        walls.back().position.x = 10 + 2*i;
        walls.back().position.y = 2*i;
        walls.back().scale *= 2;
        walls.back().primaryColor = glm::vec3(0.4,0.3,0.7);
    }
    wallPlusY.scale.x = floor.scale.x; 
    wallPlusY.scale.y *= 15; 
    wallPlusY.scale.z *= 3; 
    wallPlusY.position.y = -floor.scale.y - wallPlusY.scale.y; 
    wallPlusY.primaryColor = wallsColor; 
    wallMinusY.scale.x = floor.scale.x;
    wallMinusY.scale.y *= 15; 
    wallMinusY.scale.z *= 3; 
    wallMinusY.position.y = floor.scale.y + wallPlusY.scale.y; 
    wallMinusY.primaryColor = wallsColor; 
    wallPlusX.scale.x *= 15; 
    wallPlusX.scale.y = floor.scale.y + 15*2; 
    wallPlusX.scale.z *= 3; 
    wallPlusX.position.x = -floor.scale.x - wallPlusX.scale.x; 
    wallPlusX.primaryColor = wallsColor; 
    wallMinusX.scale.x *= 15; 
    wallMinusX.scale.y = floor.scale.y + 15*2; 
    wallMinusX.scale.z *= 3; 
    wallMinusX.position.x = floor.scale.x + wallPlusX.scale.x; 
    wallMinusX.primaryColor = wallsColor;
    addRobot(erppm::RoundRobot());
    robots[0]->getPrimaryColor() = {0.6, 0.2, 0.2};
    float anglePosition;
    float angleOnRobot = 0;
    anglePosition = 0;
    robots[0]->addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,anglePosition + angleOnRobot,0), {anglePosition, angleOnRobot}));
    anglePosition = 0.15;
    robots[0]->addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,anglePosition + angleOnRobot,0), {anglePosition, angleOnRobot}));
    anglePosition = -anglePosition;
    robots[0]->addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,anglePosition + angleOnRobot,0), {anglePosition, angleOnRobot}));
    // angle = 0;
    // for(int i =0;i < 5; i++){
    //     addRobot(erppm::RoundRobot());
    //     robots.back()->getPosition().x = 5;
    //     robots.back()->getPosition().y = i;
    //     // robots.back()->getPrimaryColor() = glm::vec3(0.2);
    //     robots.back()->getPrimaryColor() = glm::vec3(Misc::rand::get().decim(1.0f), Misc::rand::get().decim(1.0f), Misc::rand::get().decim(1.0f));
    //     robots.back()->getRotation()[2] = Misc::rand::get().decim(glm::two_pi<float>());
    //     robots.back()->addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,angle,0)));
    // }
    mainCamera.radius = 13;
    mainCamera.minVerticalRotation = 0.0;
    mainCamera.maxVerticalRotation = glm::half_pi<float>();
    coordinatesCamera.radius = 2;
    coordinatesCamera.aspect = 1.0;
    mainRobotInput.reserve(robots[0]->getControlInputSize());
    
}

void MainWindow::mainFunction(){
    if(active){
        if (keyTracker.isJustPressed(GLFW_KEY_ESCAPE)){
            glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            active = false;
        }
        if(keyTracker.isJustPressed(GLFW_KEY_F)){
            std::cout << "Respect paid!\n";
        }
        if(keyTracker.isJustPressed(GLFW_KEY_F11)){
            switchFullScreen();
        }
        if(keyTracker.isJustPressed(GLFW_KEY_TAB)){
            freeCamera =! freeCamera;
        }
        moveObjects();
        collisions();

        if(!freeCamera){
            mainCamera.lookAt(robots[0]->getPosition() + glm::vec4(0,0,mainCamera.radius/5 * ((mainCamera.maxVerticalRotation - mainCamera.getRotation().y) / (mainCamera.maxVerticalRotation - mainCamera.minVerticalRotation)),0));
        }
        coordinatesCamera.setRotation(mainCamera.getRotation());
        coordinatesCamera.lookAt(dir.position);
    }
	else if(glfwGetMouseButton( getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS){
		glfwSetInputMode(getGlfwWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        active = true;
	}

}

void MainWindow::drawFunction(){
    mainCamera.computeMatrices();
    floor.draw(mainCamera.getMVP(), mainCamera.position);
    for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter)){
        (*iter)->draw(mainCamera.getMVP(), mainCamera.position);
    }
    for(std::vector<erppm::Wall>::iterator iter = walls.begin(); iter != walls.end(); iter = std::next(iter)){
        (iter)->draw(mainCamera.getMVP(), mainCamera.position);
    }
    wallPlusY.draw(mainCamera.getMVP(), mainCamera.position);
    wallMinusY.draw(mainCamera.getMVP(), mainCamera.position);
    wallPlusX.draw(mainCamera.getMVP(), mainCamera.position);
    wallMinusX.draw(mainCamera.getMVP(), mainCamera.position);

    coordinatesCamera.computeMatrices();
    int xSize, ySize;
    glClear(GL_DEPTH_BUFFER_BIT); 
    glfwGetWindowSize(getGlfwWindowPtr(), &xSize, &ySize);
    glViewport(xSize - ySize/7, 0, ySize/7, ySize/7);
    glScissor( xSize - ySize/7, 0, ySize/7, ySize/7);
    dir.draw(coordinatesCamera.getMVP());
    glClear(GL_DEPTH_BUFFER_BIT); 
    glViewport(0, 0, xSize, ySize);
    glScissor(0, 0, xSize, ySize);

}

void MainWindow::afterResize(const int width, const int height){
    mainCamera.aspect = (float)width/height;
    coordinatesCamera.aspect = 1.0;
}

void MainWindow::moveObjects(){
    constexpr float cameraMouseSensitivity = 20;
    static double time0 = glfwGetTime();
    const double time = glfwGetTime();
    const double dtime = time - time0;
    // Get mouse position
    int xSize;
    int ySize;
    glfwGetWindowSize(getGlfwWindowPtr(), &xSize, &ySize);
    double xPos;
    double yPos;
    glfwGetCursorPos(getGlfwWindowPtr(), &xPos, &yPos);
    glfwSetCursorPos(getGlfwWindowPtr(), xSize/2, ySize/2);
    mainCamera.rotate((xPos - xSize/2) * cameraMouseSensitivity * dtime, (yPos - ySize/2) * cameraMouseSensitivity * dtime);
    if(freeCamera) {
        if(keyTracker.isPressed(GLFW_KEY_W)){
            mainCamera.position += mainCamera.getDirection() * 0.01f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_S)){
            mainCamera.position -= mainCamera.getDirection() * 0.01f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_A)){
            mainCamera.position += mainCamera.getRightVector() * 0.01f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_D)){
            mainCamera.position -= mainCamera.getRightVector() * 0.01f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_E)){
            mainCamera.position.z += 0.01f * mainCamera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_Q)){
            mainCamera.position.z -= 0.01f * mainCamera.position.z;
        }
    }
    else{
        if(keyTracker.isJustReleased(GLFW_KEY_ENTER)){
            std::swap(robots[0], robots[Jimmy::Misc::rand().randInt(1,robots.size())]);
        }
        // Gamepad data
        int countAx;
        int countBtn;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAx);
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countBtn);
        if(countBtn > 0){
            mainCamera.rotate(1000 * dtime * (float)(buttons[14] - buttons[16]), 1000 * dtime * (float)(buttons[13] - buttons[15]));
        }
        if(countAx > 0){
            mainCamera.rotate(((axes[2] + 1.0) - (axes[5] + 1.0))*15, 0);
            mainRobotInput[0] = -(abs(axes[1]) > 0.1 ? axes[1] : 0.0);
            mainRobotInput[1] = -(abs(axes[4]) > 0.1 ? axes[4] : 0.0);
            robots[0]->run(dtime, mainRobotInput);

            // for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter)){
            //     (*iter)->run(dtime, mainRobotInput);
            // }
            for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter)){
                (*iter)->collide(robots, walls, floor);
                (*iter)->updateSensorsPosition();
            }
            for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter)){
                (*iter)->measure(robots, walls, floor);
            }

            static bool followDirection = false;
            if(keyTracker.isJustPressed(GLFW_KEY_SPACE)){
                followDirection = !followDirection;
            }
            if (followDirection) {
                mainCamera.rotate(( mainCamera.getRotation()[0] - robots[0]->getRotation()[2]) * 50, 0);
            }
        }
    }
    time0 = time;
}

void MainWindow::collisions(){
}

int main(int argc, char *argv[]) {
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