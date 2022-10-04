#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <list>
#include <chrono>
#include <new>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "whereami.h"
#include "oglUtil/oglu.hpp"
#include "oglUtil_OLD/Cube.hpp"
#include "oglUtil_OLD/Directions.hpp"
#include "oglUtil_OLD/Mesh.hpp"
#include "oglUtil_OLD/Misc.hpp"
#include "JimmyNeuron/Jimmy.hpp"

#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/LaserSensor.hpp"
#include "Robotics/ObjectTypes.hpp"

struct Scenario{
    erppm::Floor floor;
    std::vector<erppm::Wall> walls;
};

class sim{
public:
    constexpr static unsigned int NUMBER_OF_ROBOTS = 6;    
    int currentTypedRobotIndex; // -1 for all
    unsigned int numberOfGenerations;
    unsigned int numberOfStepsPerGeneration;
    double stepDuration;
    std::vector<erppm::RobotBase*> robots;
    std::vector<erppm::RobotBase*> predatorRobots;
    std::vector<erppm::RobotBase*> preyRobots;
    std::vector<Jimmy::LoopingNet> predatorNetworks;
    std::vector<Jimmy::LoopingNet> preyNetworks;
    
    Scenario scenario_0; // just a floor
    Scenario scenario_1; // ? 
    Scenario scenario_test; // test 
    Scenario* currentScenario;

    static sim& get();
    template <typename T>
    void addRobot(T&& robot);
    template <typename T>
    std::list<T>& getRobotList();
    void reinitializeDrawables();

private:
    sim();
};

sim& sim::get(){
    static sim singleton;
    return singleton;
}

sim::sim()
: predatorRobots()
, preyRobots()
, predatorNetworks()
, preyNetworks()
, currentTypedRobotIndex(-1) // all robots
, numberOfGenerations(1000)
, numberOfStepsPerGeneration(500)
, stepDuration(0.01)
, scenario_0()
, scenario_1()
, scenario_test()
, currentScenario(&scenario_0)
{   
    /* Scenario_0 - just a floor */
    {
        const glm::vec3 wallsColor(0.6); 
        scenario_0.floor.scale.y = 60;
        scenario_0.floor.scale.x = 60;
        scenario_0.floor.primaryColor = wallsColor;
    }
    /* Scenario_1 */
    {   
        // TODO
    }
    /* Scenario_test */
    {   
        const glm::vec3 wallsColor(0.6); 
        scenario_test.floor.scale.y = 30;
        scenario_test.floor.scale.x = 30;
        scenario_test.floor.primaryColor = wallsColor;
        for(int i =0;i < 5; i++){
            scenario_test.walls.emplace_back(erppm::Wall());
            scenario_test.walls.back().position.x = 10 + 2*i;
            scenario_test.walls.back().position.y = 2*i;
            scenario_test.walls.back().scale *= 2;
            scenario_test.walls.back().primaryColor = glm::vec3(0.4,0.3,0.7);
        }
    }
    /* Predator robots */
    {   
        
        predatorRobots.reserve(NUMBER_OF_ROBOTS);
        predatorNetworks.reserve(NUMBER_OF_ROBOTS);
        for(unsigned int i = 0; i < NUMBER_OF_ROBOTS; i++){
            addRobot(erppm::RoundRobot());
            erppm::RobotBase& robot = *(robots.back()); 
            predatorRobots.emplace_back(robots.back());
            robot.getPrimaryColor() = {0.6, 0.2, 0.2};
            float angle;
            angle = 0;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,0,0)));
            angle = 0.15;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,0,0)));
            angle = -angle;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,0,0)));
            robot.getRotation()[2] = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
            robot.type = erppm::EObjectTypePredator;
            predatorNetworks.emplace_back(Jimmy::LoopingNet(robot.getSensorDataSize(),20,robot.getControlInputSize(),Jimmy::Methods::transFuncs::tanh, 0.01));
            double randomizeLimit = 0.025/(robot.getSensorDataSize() + 20 + robot.getControlInputSize());
            predatorNetworks.back().randomize(-randomizeLimit, randomizeLimit);
            // predatorNetworks.back().mutate();
        }
    }
    /* Prey robots */
    {   
        preyRobots.reserve(NUMBER_OF_ROBOTS);
        preyNetworks.reserve(NUMBER_OF_ROBOTS);
        for(unsigned int i = 0; i < NUMBER_OF_ROBOTS; i++){
            addRobot(erppm::RoundRobot());
            erppm::RobotBase& robot = *(robots.back()); 
            preyRobots.emplace_back(robots.back());
            robot.getPrimaryColor() = {0.2, 0.2, 0.6};
            float angle;
            angle = 0;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,angle,0)));
            angle = 0.15;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,angle,0)));
            angle = -angle;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,angle,0)));
            robot.getRotation()[2] = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
            robot.type = erppm::EObjectTypePrey;
            preyNetworks.emplace_back(Jimmy::LoopingNet(robot.getSensorDataSize(),20,robot.getControlInputSize(),Jimmy::Methods::transFuncs::tanh, 0.01));
            double randomizeLimit = 0.025/(robot.getSensorDataSize() + 20 + robot.getControlInputSize());
            preyNetworks.back().randomize(-randomizeLimit, randomizeLimit);
            // preyNetworks.back().mutate();
        }
    }
}

template <typename T>
std::list<T>& sim::getRobotList(){
    static std::list<T> robots_T;              // it has to be a list, so the pointers won't change when vector relocates data
    return robots_T;
}

template <typename T>
void sim::addRobot(T&& robot){
    std::list<T>& robots_T = getRobotList<T>();
    robots_T.push_back(std::move(robot));
    robots.push_back( &(robots_T.back()) );
}

void sim::reinitializeDrawables(){
    for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end() ; iter = std::next(iter)){
        (*iter)->reinitializeDrawables();
    }
    std::vector<Scenario*> scenarios = {&scenario_0, &scenario_1, &scenario_test};
    for(std::vector<Scenario*>::iterator scenarioIter = scenarios.begin(); scenarioIter != scenarios.end(); scenarioIter = std::next(scenarioIter)){
        Scenario& scenario = (**scenarioIter);
        for(std::vector<erppm::Wall>::iterator wallIter = scenario.walls.begin(); wallIter != scenario.walls.end() ; wallIter = std::next(wallIter)){
            (*wallIter).reinitialize();
        }
        scenario.floor.reinitialize();
    }
}

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
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    Directions dir;
    erppm::Wall wallPlusY; 
    erppm::Wall wallMinusY;
    erppm::Wall wallPlusX; 
    erppm::Wall wallMinusX;
    std::vector<double> mainRobotInput;
    oglu::Camera mainCamera;
    oglu::Camera coordinatesCamera;
    std::chrono::_V2::system_clock::time_point time0;
    glm::vec3 cameraFocusPoint;
    bool active;
    bool freeCamera;
};

MainWindow::~MainWindow(){

}

MainWindow::MainWindow(const int _width, const int _height, const std::string& _name) 
: oglu::Window(_width, _height, _name, false)
, dir()
, wallPlusY() 
, wallMinusY()
, wallPlusX() 
, wallMinusX()
, mainRobotInput()
, mainCamera(getGlfwWindowPtr())
, coordinatesCamera(getGlfwWindowPtr())
, time0(std::chrono::system_clock::now())
, cameraFocusPoint(0,0,0)
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
    glfwSetScrollCallback(getGlfwWindowPtr(), scroll_callback);
    glfwSetWindowUserPointer(getGlfwWindowPtr(),this);
    glfwPollEvents();
    const glm::vec3 wallsColor(0.6);
    const double borderWallsThickness = 100;
    wallPlusY.scale.x = sim::get().currentScenario->floor.scale.x; 
    wallPlusY.scale.y *= borderWallsThickness; 
    wallPlusY.scale.z *= 3; 
    wallPlusY.position.y = -sim::get().currentScenario->floor.scale.y - wallPlusY.scale.y; 
    wallPlusY.primaryColor = wallsColor; 
    wallMinusY.scale.x = sim::get().currentScenario->floor.scale.x;
    wallMinusY.scale.y *= borderWallsThickness; 
    wallMinusY.scale.z *= 3; 
    wallMinusY.position.y = sim::get().currentScenario->floor.scale.y + wallPlusY.scale.y; 
    wallMinusY.primaryColor = wallsColor; 
    wallPlusX.scale.x *= borderWallsThickness; 
    wallPlusX.scale.y = sim::get().currentScenario->floor.scale.y + borderWallsThickness*2; 
    wallPlusX.scale.z *= 3; 
    wallPlusX.position.x = -sim::get().currentScenario->floor.scale.x - wallPlusX.scale.x; 
    wallPlusX.primaryColor = wallsColor; 
    wallMinusX.scale.x *= borderWallsThickness; 
    wallMinusX.scale.y = sim::get().currentScenario->floor.scale.y + borderWallsThickness*2; 
    wallMinusX.scale.z *= 3; 
    wallMinusX.position.x = sim::get().currentScenario->floor.scale.x + wallPlusX.scale.x; 
    wallMinusX.primaryColor = wallsColor;
    mainCamera.radius = 30;
    mainCamera.setRotation({0,1,0});
    mainCamera.minVerticalRotation = 0.5;
    mainCamera.maxVerticalRotation = glm::half_pi<float>();
    coordinatesCamera.radius = 2;
    coordinatesCamera.aspect = 1.0;
    mainRobotInput.reserve(sim::get().robots[0]->getControlInputSize());
    
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
            mainCamera.lookAt(cameraFocusPoint);
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
    sim::get().currentScenario->floor.draw(mainCamera.getMVP(), mainCamera.position);
    for(std::vector<erppm::RobotBase*>::iterator iter = sim::get().robots.begin(); iter != sim::get().robots.end(); iter = std::next(iter)){
        (*iter)->draw(mainCamera.getMVP(), mainCamera.position);
    }
    for(std::vector<erppm::Wall>::iterator iter = sim::get().currentScenario->walls.begin(); iter != sim::get().currentScenario->walls.end(); iter = std::next(iter)){
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
    std::chrono::_V2::system_clock::time_point timeNow = std::chrono::system_clock::now();
    double dtime = (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow-time0).count());
    dtime /= 1000.0;
    // std::cout << "time:" << dtime << " " << std::chrono::duration_cast<std::chrono::milliseconds>(timeNow-time0).count() / 1000.0 << "\n";
    // Get mouse position
    int xSize;
    int ySize;
    glfwGetWindowSize(getGlfwWindowPtr(), &xSize, &ySize);
    double xPos;
    double yPos;
    glfwGetCursorPos(getGlfwWindowPtr(), &xPos, &yPos);
    glfwSetCursorPos(getGlfwWindowPtr(), xSize/2, ySize/2);
    int mouseState;
    mouseState = glfwGetMouseButton(getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_RIGHT);
    if (mouseState == GLFW_PRESS)
    {
        mainCamera.rotate(-(xPos - xSize/2) * cameraMouseSensitivity * dtime, -(yPos - ySize/2) * cameraMouseSensitivity * dtime);
    }
    mouseState = glfwGetMouseButton(getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_LEFT);
    if (mouseState == GLFW_PRESS)
    {
        glm::vec2 posDiff;       
        posDiff.x = (yPos - ySize/2) * dtime;
        posDiff.y = (xPos - xSize/2) * dtime;
        // std::cout << "posDiff" << posDiff.x << " " << posDiff.y << " " << mainCamera.getRotation()[0] << "\n";
        cameraFocusPoint.x += posDiff.x * cos(mainCamera.getRotation()[0]) - posDiff.y * sin(mainCamera.getRotation()[0]);
        cameraFocusPoint.y += posDiff.x * sin(mainCamera.getRotation()[0]) + posDiff.y * cos(mainCamera.getRotation()[0]);
        if(cameraFocusPoint.x > sim::get().currentScenario->floor.scale.x){
            cameraFocusPoint.x = sim::get().currentScenario->floor.scale.x;
        }
        else if(cameraFocusPoint.x < -sim::get().currentScenario->floor.scale.x){
            cameraFocusPoint.x = -sim::get().currentScenario->floor.scale.x;
        }
        if(cameraFocusPoint.y > sim::get().currentScenario->floor.scale.y){
            cameraFocusPoint.y = sim::get().currentScenario->floor.scale.y;
        }
        else if(cameraFocusPoint.y < -sim::get().currentScenario->floor.scale.y){
            cameraFocusPoint.y = -sim::get().currentScenario->floor.scale.y;
        }
    }
    mouseState = glfwGetMouseButton(getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_MIDDLE);
    if (mouseState == GLFW_PRESS)
    {
        mainCamera.radius += (yPos - ySize/2) * dtime;
    }

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
            sim::get().robots[0]->run(dtime, mainRobotInput);

            // for(std::vector<erppm::RobotBase*>::iterator iter = sim::get().robots.begin(); iter != sim::get().robots.end(); iter = std::next(iter)){
            //     (*iter)->run(dtime, mainRobotInput);
            // }
            for(std::vector<erppm::RobotBase*>::iterator iter = sim::get().robots.begin(); iter != sim::get().robots.end(); iter = std::next(iter)){
                (*iter)->collide(sim::get().robots, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                (*iter)->updateSensorsPosition();
            }
            for(std::vector<erppm::RobotBase*>::iterator iter = sim::get().robots.begin(); iter != sim::get().robots.end(); iter = std::next(iter)){
                (*iter)->measure(sim::get().robots, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
            }

            static bool followDirection = false;
            if(keyTracker.isJustPressed(GLFW_KEY_SPACE)){
                followDirection = !followDirection;
            }
            if (followDirection) {
                mainCamera.rotate(( mainCamera.getRotation()[0] - sim::get().robots[0]->getRotation()[2]) * 50, 0);
            }
        }
    }
    time0 = std::chrono::system_clock::now();
}

void MainWindow::collisions(){
}

void MainWindow::scroll_callback(GLFWwindow* window, double xOffset, double yOffset){
    MainWindow* myWindowPointer = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    myWindowPointer->mainCamera.radius -= yOffset;
    // std::cout << "scroll " << myWindowPointer->mainCamera.radius << "\n";
}

enum EAction{
    EActionWait,
    EActionTerminate,
    EActionOpenWindow,
    EActionTrain,
};

template <bool OPEN_WINDOW>
void stage_1(){ // learn predators to chase one of still prey
        uint8_t buffer[sizeof(MainWindow)];
        MainWindow& window = *(MainWindow*)(void*)(buffer);
        if constexpr ( OPEN_WINDOW ){
            std::cout << "Opening window\n";
            new(&window) MainWindow(800, 800, "Robot demo");
            sim::get().reinitializeDrawables();
            window.keyTracker.addKey(GLFW_KEY_F);
            window.keyTracker.addKey(GLFW_KEY_F11);
            window.keyTracker.addKey(GLFW_KEY_ESCAPE);
            window.keyTracker.addKey(GLFW_KEY_TAB);
            window.keyTracker.addKey(GLFW_KEY_SPACE);
            window.keyTracker.addKey(GLFW_KEY_W);
            window.keyTracker.addKey(GLFW_KEY_S);
            window.keyTracker.addKey(GLFW_KEY_A);
            window.keyTracker.addKey(GLFW_KEY_D);
            window.keyTracker.addKey(GLFW_KEY_Q);
            window.keyTracker.addKey(GLFW_KEY_E);
            window.mainCamera.position.z = 120;
            // window.mainCamera.setRotation({0,0,0});
            window.mainCamera.position.x = -100;
        }
        else{
            // window.destroy();
        }

    unsigned int generationNumber = 0;
    for(; generationNumber< sim::get().numberOfGenerations; generationNumber++){
        // Prepare generation
        const double preyPositionRadius = Jimmy::Misc::rand().decimal(3,std::min(sim::get().currentScenario->floor.scale.x, sim::get().currentScenario->floor.scale.y));
        const double preyPositionAngle = Jimmy::Misc::rand().decimal(0,glm::two_pi<float>());
        const double preyRotation = Jimmy::Misc::rand().decimal(0,glm::two_pi<float>());
        const double predatorRotation = Jimmy::Misc::rand().decimal(0,glm::two_pi<float>());
        glm::vec4 preyPos = {cos(preyPositionAngle) * preyPositionRadius, sin(preyPositionAngle) * preyPositionRadius, 0, 0};
        for(unsigned int robotIndex = 0; robotIndex < sim::get().predatorRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().predatorRobots[robotIndex]);
            robot.getPosition() = {0,0,0,0};
            robot.getRotation()[2] = predatorRotation;
            robot.updateSensorsPosition();
            robot.clearVelocity();
        }
        for(unsigned int robotIndex = 0; robotIndex < sim::get().preyRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().preyRobots[robotIndex]);
            robot.getPosition() = preyPos;
            robot.getRotation()[2] = preyRotation;
            robot.updateSensorsPosition();
            robot.clearVelocity();
        }
        bool isWin = false;
        // run generation
        for(unsigned int robotIndex = 0; robotIndex < sim::get().predatorRobots.size(); robotIndex++){
            // std::cout << "new robot " << robotIndex << "\n";
            
            // std::cout << robotIndex << sim::get().predatorRobots.size() << sim::get().preyRobots.size() << "\n"; 
            std::vector<erppm::RobotBase*> robotsInCurrentRun = {sim::get().predatorRobots[robotIndex], sim::get().preyRobots[robotIndex]};
            erppm::RobotBase& predatorRobot = *(sim::get().predatorRobots[robotIndex]);
            Jimmy::LoopingNet& predatorNetwork = sim::get().predatorNetworks[robotIndex];
            erppm::RobotBase& preyRobot = *(sim::get().preyRobots[robotIndex]);
            // Jimmy::LoopingNet& preyNetwork = sim::get().preyNetworks[robotIndex];

            predatorNetwork.clearOutputValues();

            for(unsigned int stepNumber = 0; stepNumber< sim::get().numberOfStepsPerGeneration; stepNumber++){
                if constexpr ( OPEN_WINDOW ){
                    if(glfwWindowShouldClose(window.getGlfwWindowPtr()) == 0 && window.getGlfwWindowPtr() != nullptr){
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        oglu::printErrorsDbg("glClear");
                        window.keyTracker.update(window.getGlfwWindowPtr());
                    }
                }
                for(std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun.begin(); iter != robotsInCurrentRun.end(); iter = std::next(iter)){
                    (*iter)->measure(robotsInCurrentRun, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                }
                predatorNetwork.run(predatorRobot.getSensorData());
                predatorRobot.run(0.01, predatorNetwork.getOutput());
                if(glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) < 2.0){
                    isWin = true;
                    break;
                }
                for(std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun.begin(); iter != robotsInCurrentRun.end(); iter = std::next(iter)){
                    (*iter)->collide(robotsInCurrentRun, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                    (*iter)->updateSensorsPosition();
                }
                if constexpr ( OPEN_WINDOW ){
                    if(glfwWindowShouldClose(window.getGlfwWindowPtr()) == 0 && window.getGlfwWindowPtr() != nullptr){
    int xSize;
    int ySize;
    glfwGetWindowSize(window.getGlfwWindowPtr(), &xSize, &ySize);
    double xPos;
    double yPos;
    glfwGetCursorPos(window.getGlfwWindowPtr(), &xPos, &yPos);
    glfwSetCursorPos(window.getGlfwWindowPtr(), xSize/2, ySize/2);
    int mouseState;
    mouseState = glfwGetMouseButton(window.getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_RIGHT);
    if (mouseState == GLFW_PRESS)
    {
        window.mainCamera.rotate(-(xPos - xSize/2) * 20 * 0.01, -(yPos - ySize/2) * 20 * 0.01);
    }
    mouseState = glfwGetMouseButton(window.getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_LEFT);
    if (mouseState == GLFW_PRESS)
    {
        glm::vec2 posDiff;       
        posDiff.x = (yPos - ySize/2) * 0.01;
        posDiff.y = (xPos - xSize/2) * 0.01;
        // std::cout << "posDiff" << posDiff.x << " " << posDiff.y << " " << window.mainCamera.getRotation()[0] << "\n";
        window.cameraFocusPoint.x += posDiff.x * cos(window.mainCamera.getRotation()[0]) - posDiff.y * sin(window.mainCamera.getRotation()[0]);
        window.cameraFocusPoint.y += posDiff.x * sin(window.mainCamera.getRotation()[0]) + posDiff.y * cos(window.mainCamera.getRotation()[0]);
        if(window.cameraFocusPoint.x > sim::get().currentScenario->floor.scale.x){
            window.cameraFocusPoint.x = sim::get().currentScenario->floor.scale.x;
        }
        else if(window.cameraFocusPoint.x < -sim::get().currentScenario->floor.scale.x){
            window.cameraFocusPoint.x = -sim::get().currentScenario->floor.scale.x;
        }
        if(window.cameraFocusPoint.y > sim::get().currentScenario->floor.scale.y){
            window.cameraFocusPoint.y = sim::get().currentScenario->floor.scale.y;
        }
        else if(window.cameraFocusPoint.y < -sim::get().currentScenario->floor.scale.y){
            window.cameraFocusPoint.y = -sim::get().currentScenario->floor.scale.y;
        }
    }
    mouseState = glfwGetMouseButton(window.getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_MIDDLE);
    if (mouseState == GLFW_PRESS)
    {
        window.mainCamera.radius += (yPos - ySize/2) * 0.01;
    }
                        sim::get().currentScenario->floor.draw(window.mainCamera.getMVP(),window.mainCamera.position);
                        oglu::printErrorsDbg("mainFunction");
                        window.drawFunction();
                        oglu::printErrorsDbg("drawFunction");
                        glfwSwapBuffers(window.getGlfwWindowPtr()); // Swap buffers
                        glfwPollEvents();
                        oglu::printErrorsDbg("glfwSwapBuffers or glfwPollEvents");
                    }
                }
            }

            predatorNetwork.score = (isWin ? 0.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) / preyPositionRadius);
        }
        // get the best
        std::sort(sim::get().predatorNetworks.begin(), sim::get().predatorNetworks.end(), [](const Jimmy::LoopingNet& a, const Jimmy::LoopingNet& b){return a.score < b.score;});
        if(generationNumber % 100 == 0){
            std::cout << "generation " << generationNumber << ":\tpopulationError " << std::accumulate(sim::get().predatorNetworks.begin(), sim::get().predatorNetworks.end(), 0.0, [](double s, const Jimmy::LoopingNet& a){return s + a.score;})/(sim::get().predatorNetworks.size()) << ",\tbest " << sim::get().predatorNetworks[0].score << ",\tworst " << sim::get().predatorNetworks.back().score << /*",\t biggest error " <<  biggestError << ",\t precision " << precision/(networks.size())*100.0 << "%" <<*/ "\n";
            // if(generationNumber % 10000 == 0){
            //     for(std::vector<Jimmy::LoopingNet>::iterator iter = networks.begin();  iter != networks.end(); iter = std::next(iter)){
            //         Jimmy::LoopingNet& network = (*iter);
            //         network.susceptibilityToMutations *= 0.5;
            //     }
            //     std::cout << "change susceptibilityToMutations to " << networks[0].susceptibilityToMutations << "\n";
            // }
        }
        sim::get().predatorNetworks[5].evolveFrom(sim::get().predatorNetworks[0],sim::get().predatorNetworks[1]);
        sim::get().predatorNetworks[4].evolveFrom(sim::get().predatorNetworks[0],sim::get().predatorNetworks[2]);
        sim::get().predatorNetworks[3].evolveFrom(sim::get().predatorNetworks[1],sim::get().predatorNetworks[2]);
    }
    if constexpr( OPEN_WINDOW ){
        window.~MainWindow();
    }
}

void stage_2(){ // learn prey ro run from one of still predators
    
}

void stage_3(){ // learn predators and prey at the same time in pairs
    
}

void stage_4(){ // (optional) learn all predators and prey at the same time 
    
}

void stage_5(){ // (optional) learn predators and prey at the same time in pairs at all other scenarios
    
}

void stage_6(){ // learn all predators and prey at the same time at all other scenarios
    
}

EAction getAction(){
    std::string cinString;
    std::cout <<
        "Available commands:\n" << 
        "\t exit/quit - Terminates program\n" <<
        "\t run [scenario] - Opens window with simulation\n" <<
        // "\t train [stage] - Trains neural network in the background\n" <<
        "\t stage [n] - Trains neural network in the background\n" <<
    "";
    std::cin >> cinString;
    if(cinString == "exit" || cinString == "quit"){
        return EAction::EActionTerminate;
    }
    else if(cinString == "run"){
        return EAction::EActionOpenWindow;
    }
    else if(cinString == "train"){
        std::cout << "Number of generations:\n";
        std::cin >> (sim::get().numberOfGenerations);
        return EAction::EActionTrain;
    }
    else if(cinString == "stage"){
        int stageNum;
        std::cin >> stageNum;
        std::cout << "Number of generations:\n";
        std::cin >> (sim::get().numberOfGenerations);
        std::cout << "Number of steps per generation:\n";
        std::cin >> (sim::get().numberOfStepsPerGeneration);
        stage_1<false>();
        return EAction::EActionWait;
    }
    else if(cinString == "stagew"){
        int stageNum;
        std::cin >> stageNum;
        std::cout << "Number of generations:\n";
        std::cin >> (sim::get().numberOfGenerations);
        std::cout << "Number of steps per generation:\n";
        std::cin >> (sim::get().numberOfStepsPerGeneration);
        stage_1<true>();
        return EAction::EActionWait;
    }
    std::cout << "Command \"" << cinString << "\" not recognized\n";
    return EAction::EActionWait;

}

int main(int argc, char *argv[]) {
    oglu::setupOglu();
    oglu::setupGlfw();
    {   // Open window to initialize GLEW and initialize drawables;
        oglu::Window wInitGlew(800, 800, "Init GLEW");
        sim::get();
    }

    // std::cout << "hmm?: " << sim::get().numberOfGenerations << " ";
    EAction action = getAction(); 
    while(action != EAction::EActionTerminate){
        switch(action){
            case EAction::EActionOpenWindow:{
                std::cout << "Opening window\n";
                MainWindow window(800, 800, "Robot demo");
                sim::get().reinitializeDrawables();
                window.keyTracker.addKey(GLFW_KEY_F);
                window.keyTracker.addKey(GLFW_KEY_F11);
                window.keyTracker.addKey(GLFW_KEY_ESCAPE);
                window.keyTracker.addKey(GLFW_KEY_TAB);
                window.keyTracker.addKey(GLFW_KEY_SPACE);
                window.keyTracker.addKey(GLFW_KEY_W);
                window.keyTracker.addKey(GLFW_KEY_S);
                window.keyTracker.addKey(GLFW_KEY_A);
                window.keyTracker.addKey(GLFW_KEY_D);
                window.keyTracker.addKey(GLFW_KEY_Q);
                window.keyTracker.addKey(GLFW_KEY_E);
                // window.mainLoop();
                do {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    oglu::printErrorsDbg("glClear");
                    window.keyTracker.update(window.getGlfwWindowPtr());
                    window.mainFunction();
                    oglu::printErrorsDbg("mainFunction");
                    window.drawFunction();
                    oglu::printErrorsDbg("drawFunction");
                    glfwSwapBuffers(window.getGlfwWindowPtr()); // Swap buffers
                    glfwPollEvents();
                    oglu::printErrorsDbg("glfwSwapBuffers or glfwPollEvents");
                } while( glfwWindowShouldClose(window.getGlfwWindowPtr()) == 0 && window.getGlfwWindowPtr() != nullptr);
                break;
            }
            case EAction::EActionTrain:{
                std::cout << "Opening window\n";
                MainWindow window(800, 800, "Robot demo");
                sim::get().reinitializeDrawables();
                window.keyTracker.addKey(GLFW_KEY_F);
                window.keyTracker.addKey(GLFW_KEY_F11);
                window.keyTracker.addKey(GLFW_KEY_ESCAPE);
                window.keyTracker.addKey(GLFW_KEY_TAB);
                window.keyTracker.addKey(GLFW_KEY_SPACE);
                window.keyTracker.addKey(GLFW_KEY_W);
                window.keyTracker.addKey(GLFW_KEY_S);
                window.keyTracker.addKey(GLFW_KEY_A);
                window.keyTracker.addKey(GLFW_KEY_D);
                window.keyTracker.addKey(GLFW_KEY_Q);
                window.keyTracker.addKey(GLFW_KEY_E);
                window.mainCamera.position.z = 60;
                window.mainCamera.position.x = -60;

                
                std::vector<double> input(sim::get().robots[0]->getControlInputSize());
                input[0] = 0.5;
                input[1] = 0.0;
                for(unsigned int i = 0; i< sim::get().numberOfGenerations && glfwWindowShouldClose(window.getGlfwWindowPtr()) == 0 && window.getGlfwWindowPtr() != nullptr; i++){
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    oglu::printErrorsDbg("glClear");
                    window.keyTracker.update(window.getGlfwWindowPtr());


                    sim::get().robots[0]->run(0.01, input);
                    for(std::vector<erppm::RobotBase*>::iterator iter = sim::get().robots.begin(); iter != sim::get().robots.end(); iter = std::next(iter)){
                        (*iter)->collide(sim::get().robots, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                        (*iter)->updateSensorsPosition();
                    }
                    for(std::vector<erppm::RobotBase*>::iterator iter = sim::get().robots.begin(); iter != sim::get().robots.end(); iter = std::next(iter)){
                        (*iter)->measure(sim::get().robots, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                        (*iter)->draw(window.mainCamera.getMVP(),window.mainCamera.position);
                    }
                    sim::get().currentScenario->floor.draw(window.mainCamera.getMVP(),window.mainCamera.position);


                    oglu::printErrorsDbg("mainFunction");
                    window.drawFunction();
                    oglu::printErrorsDbg("drawFunction");
                    glfwSwapBuffers(window.getGlfwWindowPtr()); // Swap buffers
                    glfwPollEvents();
                    oglu::printErrorsDbg("glfwSwapBuffers or glfwPollEvents");
                }
            }
            case EAction::EActionWait:
            case EAction::EActionTerminate:
                break;
        }
        action = getAction();
    }
    return 0;
}