#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <list>
#include <chrono>
#include <new>
#include <filesystem>

#include <thread>
#include <mutex>
#include <future>

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

#define ALLOW_VISUALS 1

struct Scenario{
    erppm::Floor floor;
    std::vector<erppm::Wall> walls;
};

constexpr static size_t NUMBER_OF_SURVIVING_ROBOTS = 2;
constexpr static size_t NUMBER_OF_ELIMINATED_ROBOTS =  (NUMBER_OF_SURVIVING_ROBOTS)*(NUMBER_OF_SURVIVING_ROBOTS-1)/2;
constexpr static size_t NUMBER_OF_ROBOTS = NUMBER_OF_SURVIVING_ROBOTS + NUMBER_OF_ELIMINATED_ROBOTS;
constexpr static size_t NUMBER_OF_SENSORS_PER_SIDE = 3;
constexpr static size_t NUMBER_OF_SENSORS = NUMBER_OF_SENSORS_PER_SIDE * 2 + 2;
class sim{
public:
    // int currentTypedRobotIndex; // -1 for all
    size_t numberOfGenerations;
    size_t numberOfStepsPerGeneration;
    double stepDuration;
    std::vector<erppm::RobotBase*> robots;
    std::vector<erppm::RobotBase*> predatorRobots;
    std::vector<erppm::RobotBase*> preyRobots;
    // std::vector<Jimmy::LoopingNet> predatorNetworks;
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
// , predatorNetworks()
, preyNetworks()
// , currentTypedRobotIndex(-1) // all robots
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
        // predatorNetworks.reserve(NUMBER_OF_ROBOTS);
        for(unsigned int i = 0; i < NUMBER_OF_ROBOTS; i++){
            addRobot(erppm::RoundRobot());
            erppm::RobotBase& robot = *(robots.back()); 
            predatorRobots.emplace_back(robots.back());
            robot.getPrimaryColor() = {0.6, 0.2, 0.2};
            float anglePosition = 0;
            float angleOnRobot = 0;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,(angleOnRobot + anglePosition),0), {anglePosition, angleOnRobot}));
            constexpr double INTER_SENSOR_SPACE = glm::pi<double>() / NUMBER_OF_SENSORS_PER_SIDE;
            constexpr double BEGIN_SENSOR_PLACEMENT = INTER_SENSOR_SPACE / 2.0; 
            for(int i = 0; i < NUMBER_OF_SENSORS_PER_SIDE; i++){
                for(double side : {1.0, -1.0}){
                    anglePosition = side *(BEGIN_SENSOR_PLACEMENT + INTER_SENSOR_SPACE * i);//i * 0.2 * (i%2 ? 1.0 : -1.0);//Jimmy::Misc::rand().decimal(-glm::pi<float>(), glm::pi<float>());
                    angleOnRobot = 0;//Jimmy::Misc::rand().decimal(-glm::half_pi<float>(), glm::half_pi<float>());
                    robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,(angleOnRobot + anglePosition),0), {anglePosition, angleOnRobot}));
                }
            }
            anglePosition = glm::pi<float>();
            angleOnRobot = 0;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,(angleOnRobot + anglePosition),0), {anglePosition, angleOnRobot}));
            assert(NUMBER_OF_SENSORS == robot.sensors.size());
            // angle = 0;
            // robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,0,0)));
            // angle = 0.15;
            // robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,0,0)));
            // angle = -angle;
            // robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(angle),glm::sin(angle),0.2,0), glm::vec4(0,0,0,0)));
            robot.getRotation()[2] = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
            robot.type = erppm::EObjectTypePredator;
            robot.network.addNeurons(robot.getSensorDataSize(),20,robot.getControlInputSize(), 20);
            // double randomizeLimit = 0.025/(robot.getSensorDataSize() + 20 + robot.getControlInputSize());
            // robot.network.randomize(-randomizeLimit, randomizeLimit);
        }
    }
    /* Prey robots */
    {   
        preyRobots.reserve(NUMBER_OF_ROBOTS);
        for(unsigned int i = 0; i < NUMBER_OF_ROBOTS; i++){
            addRobot(erppm::RoundRobot());
            erppm::RobotBase& robot = *(robots.back()); 
            preyRobots.emplace_back(robots.back());
            robot.getPrimaryColor() = {0.2, 0.2, 0.6};
            float anglePosition;
            float angleOnRobot = 0;
            anglePosition = 0;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,anglePosition + angleOnRobot,0), {anglePosition, angleOnRobot}));
            anglePosition = 0.15;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,anglePosition + angleOnRobot,0), {anglePosition, angleOnRobot}));
            anglePosition = -anglePosition;
            robot.addSensor(erppm::LaserSensor(glm::vec4(glm::cos(anglePosition),glm::sin(anglePosition),0.2,0), glm::vec4(0,0,anglePosition + angleOnRobot,0), {anglePosition, angleOnRobot}));
            robot.getRotation()[2] = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
            robot.type = erppm::EObjectTypePrey;
            preyNetworks.emplace_back(Jimmy::LoopingNet(robot.getSensorDataSize(),20,robot.getControlInputSize(),20,Jimmy::Methods::transFuncs::tanh, 0.01));
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

#if 1
void rotationExercise(){
    struct ExerciseConfig {
        double predatorRotation;
        std::vector<unsigned int> sensorIndexes;
    };
    
    unsigned int generationNumber = 0;
    for(; generationNumber< sim::get().numberOfGenerations; generationNumber++){
        constexpr unsigned int NUMBER_OF_EXERCISES = 25;
        std::vector<ExerciseConfig> generationData;
        generationData.reserve(NUMBER_OF_EXERCISES);
        for(unsigned int exerciseNumber = generationData.size(); exerciseNumber < generationData.capacity(); exerciseNumber++){
            generationData.emplace_back(ExerciseConfig{Jimmy::Misc::rand().decimal(-glm::pi<float>(),glm::pi<float>())});
            generationData.back().sensorIndexes.reserve(Jimmy::Misc::rand().randInt(1, 4));
            generationData.back().sensorIndexes.emplace_back(Jimmy::Misc::rand().randInt(sim::get().robots[0]->sensors.size()));
            for(unsigned int repetition =  generationData.back().sensorIndexes.size(); repetition < generationData.back().sensorIndexes.capacity(); repetition++){
                if(generationData.back().sensorIndexes.back() == 0 || generationData.back().sensorIndexes.back() == sim::get().robots[0]->sensors.size()-1){
                    generationData.back().sensorIndexes.emplace_back(Jimmy::Misc::rand().randInt(sim::get().robots[0]->sensors.size()));
                }
                else{
                    generationData.back().sensorIndexes.emplace_back(generationData.back().sensorIndexes.back() % 2 ? generationData.back().sensorIndexes.back()+1 : generationData.back().sensorIndexes.back()-1);
                }
            }
        } 
        for(unsigned int robotIndex = 0; robotIndex < sim::get().predatorRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().predatorRobots[robotIndex]);
            robot.network.score = 0.0;
            for(unsigned int exerciseNumber = 0; exerciseNumber < generationData.size(); exerciseNumber++){
                const ExerciseConfig& exerciseData = generationData[exerciseNumber];
                bool alive = true;
                double controlInputError = 0.0;
                
                robot.clearVelocity();
                robot.network.clearValues();
                robot.getPosition() = {0,0,0,0};
                robot.getRotation()[2] = exerciseData.predatorRotation;
                robot.updateSensorsPosition();
                robot.measure(std::vector<erppm::RobotBase*>(1,&robot), sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                std::vector<double> robotSensorInput = robot.getSensorData();
                std::vector<double> controlInput;

                for(unsigned int repetition = 0; repetition < exerciseData.sensorIndexes.size(); repetition++){
                    int side = -1; // 0 front, 1 back, 2 right, 3 left;
                    unsigned int sensorIndex = exerciseData.sensorIndexes[repetition];
 
                    robotSensorInput[sensorIndex * 2 + 1] = 3;
                    if(abs(sim::get().robots[0]->sensors[sensorIndex]->evolutionaryData[0]) < 0.1){
                        side = 0;
                    }
                    else if(abs(sim::get().robots[0]->sensors[sensorIndex]->evolutionaryData[0]) > glm::pi<float>() - 0.1){
                        side = 1;
                    }
                    else if(sim::get().robots[0]->sensors[sensorIndex]->evolutionaryData[0] > 0){
                        side = 2;
                    }
                    else{
                        side = 3;
                    }
                    unsigned int stepNumber = 0;
                    for(; stepNumber< sim::get().numberOfStepsPerGeneration; stepNumber++){
                    // std::cout << exerciseNumber << " " << repetition << " " << side << " " << stepNumber << "\n";

                        robot.network.run(robotSensorInput);
                        controlInput = robot.network.getOutput();

                        switch (side)
                        {
                            case 0:
                                controlInputError += (controlInput[0] + controlInput[1]);
                                if(abs(controlInput[0] - controlInput[1]) > 0.1 || controlInput[0] < 0.0 || controlInput[1] < 0.0){
                                    alive = false;
                                }
                                break;
                            
                            case 1:
                                controlInputError += abs(controlInput[0] - controlInput[1]);
                                if(controlInput[0] > controlInput[1]){
                                    side = 2;
                                }
                                else{
                                    side = 3;
                                }
                                break;
                            
                            case 2:
                                controlInputError += controlInput[1] - controlInput[0];
                                if(controlInput[0] >= controlInput[1]){
                                    alive = false;
                                }
                                break;
                            
                            case 3:
                                controlInputError += controlInput[0] - controlInput[1];
                                if(controlInput[0] <= controlInput[1]){
                                    alive = false;
                                }
                                break;
                            
                            default:
                                break;
                        }
                        robot.network.score += controlInputError;
                        if(!alive){
                            // std::cout << "break\n"; 
                            break;
                        }
                        robot.run(sim::get().stepDuration, controlInput);
                        robotSensorInput = robot.getSensorData();
                
                    } // end for( step )
                    if(!alive){
                        break;
                    }
                } // end for( repetition )

            } // end for( exercise )
        } // end for( robot )
        std::sort(sim::get().predatorRobots.begin(), sim::get().predatorRobots.end(), [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score > b->network.score;});
        if(generationNumber % 100 == 0){
            std::cout << "PREDATOR : " 
                << "generation " << generationNumber 
                << ":\tscore " << std::accumulate(sim::get().predatorRobots.begin(), sim::get().predatorRobots.end(), 0.0, [](double s, const erppm::RobotBase* a){return s + a->network.score;})/(sim::get().predatorRobots.size()) 
                << ",\tbest " << sim::get().predatorRobots[0]->network.score 
                << ",\tworst " << sim::get().predatorRobots.back()->network.score 
                << /*",\t biggest error " <<  biggestError 
                << ",\t precision " << precision/(networks.size())*100.0 << "%" 
                <<*/ "\n";
        }
        sim::get().predatorRobots[5]->network.evolveFrom((sim::get().predatorRobots[0]->network),(sim::get().predatorRobots[1]->network));
        sim::get().predatorRobots[4]->network.evolveFrom((sim::get().predatorRobots[0]->network),(sim::get().predatorRobots[2]->network));
        sim::get().predatorRobots[3]->network.evolveFrom((sim::get().predatorRobots[1]->network),(sim::get().predatorRobots[2]->network));
    } // end for( generation ) 
}
#endif

/*
*   bool OPEN_WINDOW : should open window  
*   int {TYPE}_LEARNING_MODE
*       0 : Don't run and evolve
*       1 : Run but don't evolve
*       2 : Run and evolve only networks
*       3 : Run and evolve robots
*   int {TYPE}_PLACE_ON_LASER_SENORS
        0  : Place prey anywhere
        +n : Place prey on predator n first laser sensors
        -n : place prey on it's n first laser sensors aligned with predator 
*/
template <bool OPEN_WINDOW, int PREDATOR_LEARNING_MODE, int PREY_LEARNING_MODE, int PLACE_ON_LASER_SENORS>
void stage_1(){ // learn predators to chase one of still prey
    uint8_t buffer[sizeof(MainWindow)];
    MainWindow& window = *(MainWindow*)(void*)(buffer);
    if constexpr ( OPEN_WINDOW && ALLOW_VISUALS ){
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
    }
    unsigned int generationNumber = 0;
    for(; generationNumber< sim::get().numberOfGenerations; generationNumber++){
        for(unsigned int robotIndex = 0; robotIndex < sim::get().predatorRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().predatorRobots[robotIndex]);
            robot.network.score = 0;
        }
        for(unsigned int robotIndex = 0; robotIndex < sim::get().preyRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().preyRobots[robotIndex]);
            robot.network.score = 0;
        }
        for(unsigned int repetition = 0; repetition < 25; repetition++){ // need to prepare a
        // Prepare generation
        const double predatorRotation = Jimmy::Misc::rand().decimal(0,glm::two_pi<float>());
        const double preyPositionRadius = Jimmy::Misc::rand().decimal(5,std::min(sim::get().currentScenario->floor.scale.x, sim::get().currentScenario->floor.scale.y)/2);
        const double preyRotation = Jimmy::Misc::rand().decimal(0,glm::two_pi<float>());
        double preyPositionAngle = 10;
        if constexpr (PLACE_ON_LASER_SENORS == 0){
            preyPositionAngle = Jimmy::Misc::rand().decimal(0,glm::two_pi<float>());
        }
        else if constexpr (PLACE_ON_LASER_SENORS > 0){
            int laserSensorIndex = Jimmy::Misc::rand().randInt(0, +PLACE_ON_LASER_SENORS);
            std::list<erppm::LaserSensor>::iterator begin = sim::get().predatorRobots[0]->getSensorList<erppm::LaserSensor>().begin();
            preyPositionAngle = (*std::next(begin, laserSensorIndex)).evolutionaryData[0] + predatorRotation;
        }
        else {
            int laserSensorIndex = Jimmy::Misc::rand().randInt(0, -PLACE_ON_LASER_SENORS);
        }
        for(unsigned int robotIndex = 0; robotIndex < sim::get().predatorRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().predatorRobots[robotIndex]);
            robot.getPosition() = {0,0,0,0};
            robot.getRotation()[2] = predatorRotation;
            robot.updateSensorsPosition();
            robot.clearVelocity();
        }
        glm::vec4 preyPos = {cos(preyPositionAngle) * preyPositionRadius, sin(preyPositionAngle) * preyPositionRadius, 0, 0};
        for(unsigned int robotIndex = 0; robotIndex < sim::get().preyRobots.size(); robotIndex++){
            erppm::RobotBase& robot = *(sim::get().preyRobots[robotIndex]);
            robot.getPosition() = preyPos;
            robot.getRotation()[2] = preyRotation;
            robot.updateSensorsPosition();
            robot.clearVelocity();
        }
        // run generation
        for(unsigned int robotIndex = 0; robotIndex < sim::get().predatorRobots.size(); robotIndex++){
            bool preyCaught = false;
            std::vector<erppm::RobotBase*> robotsInCurrentRun = {sim::get().predatorRobots[robotIndex], sim::get().preyRobots[robotIndex]};
            erppm::RobotBase& predatorRobot = *(sim::get().predatorRobots[robotIndex]);
            erppm::RobotBase& preyRobot = *(sim::get().preyRobots[robotIndex]);

            // predatorRobot.network.clearOutputValues();
            // preyRobot.network.clearOutputValues();

            unsigned int stepNumber = 0;
            for(; stepNumber< sim::get().numberOfStepsPerGeneration; stepNumber++){
                if constexpr ( OPEN_WINDOW && ALLOW_VISUALS ){
                    if(glfwWindowShouldClose(window.getGlfwWindowPtr()) == 0 && window.getGlfwWindowPtr() != nullptr){
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        oglu::printErrorsDbg("glClear");
                        window.keyTracker.update(window.getGlfwWindowPtr());
                    }
                }
                for(std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun.begin(); iter != robotsInCurrentRun.end(); iter = std::next(iter)){
                    (*iter)->measure(robotsInCurrentRun, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                }
                if constexpr (PREDATOR_LEARNING_MODE != 0){
                    predatorRobot.network.run(predatorRobot.getSensorData());
                    predatorRobot.run(sim::get().stepDuration, predatorRobot.network.getOutput());
                }
                if constexpr (PREY_LEARNING_MODE != 0){
                    preyRobot.network.run(preyRobot.getSensorData());
                    preyRobot.run(sim::get().stepDuration, preyRobot.network.getOutput());
                }
                if(glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) < 2.0){
                    preyCaught = true;
                    break;
                }
                for(std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun.begin(); iter != robotsInCurrentRun.end(); iter = std::next(iter)){
                    (*iter)->collide(robotsInCurrentRun, sim::get().currentScenario->walls, sim::get().currentScenario->floor);
                    (*iter)->updateSensorsPosition();
                }
                if constexpr ( OPEN_WINDOW && ALLOW_VISUALS ){
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
                        window.mainCamera.lookAt(window.cameraFocusPoint);
                        window.coordinatesCamera.setRotation(window.mainCamera.getRotation());
                        window.coordinatesCamera.lookAt(window.dir.position);
                        oglu::printErrorsDbg("mainFunction");
                        // sim::get().currentScenario->floor.draw(window.mainCamera.getMVP(),window.mainCamera.position);
                        window.drawFunction();
                        oglu::printErrorsDbg("drawFunction");
                        glfwSwapBuffers(window.getGlfwWindowPtr()); // Swap buffers
                        glfwPollEvents();
                        oglu::printErrorsDbg("glfwSwapBuffers or glfwPollEvents");
                    }
                }
            }

            if constexpr (PREDATOR_LEARNING_MODE != 0){
                double rotationError = abs(predatorRobot.getRotation()[2] - preyPositionAngle);
                double distanceError = (preyCaught ? 0.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) /*/ preyPositionRadius*/);
                predatorRobot.network.score += rotationError + distanceError + stepNumber;
            }
            if constexpr (PREY_LEARNING_MODE != 0){
                preyRobot.network.score = (preyCaught ? 0.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) /*/ preyPositionRadius*/) + stepNumber;
            }
        }
        }

        if constexpr (PREDATOR_LEARNING_MODE != 0){
            std::sort(sim::get().predatorRobots.begin(), sim::get().predatorRobots.end(), [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;});
            if(generationNumber % 100 == 0){
                std::cout << "PREDATOR : " 
                    << "generation " << generationNumber 
                    << ":\tpopulationError " << std::accumulate(sim::get().predatorRobots.begin(), sim::get().predatorRobots.end(), 0.0, [](double s, const erppm::RobotBase* a){return s + a->network.score;})/(sim::get().predatorRobots.size()) 
                    << ",\tbest " << sim::get().predatorRobots[0]->network.score 
                    << ",\tworst " << sim::get().predatorRobots.back()->network.score 
                    << /*",\t biggest error " <<  biggestError 
                    << ",\t precision " << precision/(networks.size())*100.0 << "%" 
                    <<*/ "\n";
            }
        }
        if constexpr (PREDATOR_LEARNING_MODE == 2){
            unsigned int eliminated_index = NUMBER_OF_SURVIVING_ROBOTS;
            for(int i = 0; i < NUMBER_OF_SURVIVING_ROBOTS; i++){
                for(int j = i+1; j < NUMBER_OF_SURVIVING_ROBOTS; j++){
                    sim::get().predatorRobots[eliminated_index]->network.evolveFrom((sim::get().predatorRobots[i]->network),(sim::get().predatorRobots[j]->network));
                    eliminated_index++;
                }
            }
            // sim::get().predatorRobots[5]->network.evolveFrom((sim::get().predatorRobots[0]->network),(sim::get().predatorRobots[1]->network));
            // sim::get().predatorRobots[4]->network.evolveFrom((sim::get().predatorRobots[0]->network),(sim::get().predatorRobots[2]->network));
            // sim::get().predatorRobots[3]->network.evolveFrom((sim::get().predatorRobots[1]->network),(sim::get().predatorRobots[2]->network));
        }
        else if(PREDATOR_LEARNING_MODE == 3){
            sim::get().predatorRobots[5]->evolveFrom(*(sim::get().predatorRobots[0]),*(sim::get().predatorRobots[1]));
            sim::get().predatorRobots[4]->evolveFrom(*(sim::get().predatorRobots[0]),*(sim::get().predatorRobots[2]));
            sim::get().predatorRobots[3]->evolveFrom(*(sim::get().predatorRobots[1]),*(sim::get().predatorRobots[2]));
        }

        if constexpr (PREY_LEARNING_MODE != 0){
            std::sort(sim::get().preyRobots.begin(), sim::get().preyRobots.end(), [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;});
            if(generationNumber % 100 == 0){
                std::cout << "PREY : " 
                    << "generation " << generationNumber 
                    << ":\tpopulationError " << std::accumulate(sim::get().preyRobots.begin(), sim::get().preyRobots.end(), 0.0, [](double s, const erppm::RobotBase* a){return s + a->network.score;})/(sim::get().preyRobots.size()) 
                    << ",\tbest " << sim::get().preyRobots[0]->network.score 
                    << ",\tworst " << sim::get().preyRobots.back()->network.score 
                    << /*",\t biggest error " <<  biggestError 
                    << ",\t precision " << precision/(networks.size())*100.0 << "%" 
                    <<*/ "\n";
            }
        }
        if constexpr (PREY_LEARNING_MODE == 2){
            sim::get().preyRobots[5]->network.evolveFrom((sim::get().preyRobots[0]->network),(sim::get().preyRobots[1]->network));
            sim::get().preyRobots[4]->network.evolveFrom((sim::get().preyRobots[0]->network),(sim::get().preyRobots[2]->network));
            sim::get().preyRobots[3]->network.evolveFrom((sim::get().preyRobots[1]->network),(sim::get().preyRobots[2]->network));
        }
        else if(PREY_LEARNING_MODE == 3){
            sim::get().preyRobots[5]->evolveFrom(*(sim::get().preyRobots[0]),*(sim::get().preyRobots[1]));
            sim::get().preyRobots[4]->evolveFrom(*(sim::get().preyRobots[0]),*(sim::get().preyRobots[2]));
            sim::get().preyRobots[3]->evolveFrom(*(sim::get().preyRobots[1]),*(sim::get().preyRobots[2]));
        }
    }
    if constexpr ( OPEN_WINDOW && ALLOW_VISUALS ){
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
        switch (stageNum)
        {
        case 1:
            stage_1<false, 2, 0, 1>();
            break;
        case 2:
            stage_1<false, 2, 0, 3>();
            break;
        case 3:
            stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
            break;
        case 4:
            stage_1<false, 2, 0, 0>();
            break;
        default:
            break;
        }
        return EAction::EActionWait;
    }
    else if(cinString == "stagew"){
        int stageNum;
        std::cin >> stageNum;
        sim::get().numberOfGenerations = 1;
        std::cout << "Number of steps per generation:\n";
        std::cin >> (sim::get().numberOfStepsPerGeneration);
                switch (stageNum)
        {
        case 1:
            stage_1<true, 1, 0, 1>();
            break;
        case 2:
            stage_1<true, 1, 0, 3>();
            break;
        case 3:
            stage_1<true, 1, 0, NUMBER_OF_SENSORS>();
            break;
        case 4:
            stage_1<true, 1, 0, 0>();
            break;
        default:
            break;
        }
        return EAction::EActionWait;
    }
    else if(cinString == "t1"){
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 10;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        sim::get().numberOfGenerations = 500;
        sim::get().numberOfStepsPerGeneration = 50;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        sim::get().numberOfGenerations = 100;
        sim::get().numberOfStepsPerGeneration = 100;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        return EAction::EActionWait;

    }
    else if(cinString == "r1"){
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 1;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 2;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 3;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 4;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 5;
        stage_1<false, 2, 0, NUMBER_OF_SENSORS>();
        return EAction::EActionWait;

    }
    else if(cinString == "r2"){
        sim::get().numberOfGenerations = 1000;
        sim::get().numberOfStepsPerGeneration = 10;
        rotationExercise();
        return EAction::EActionWait;

    }
    else if(cinString == "save"){
        std::string fileName;
        std::cout << "Save name:\n";
        std::cin >> fileName;
        std::string filePath;
        std::string fileDirectory = "saves/" + fileName + "/";
        if(! std::filesystem::exists("saves/" + fileName + "/")){
            std::filesystem::create_directories("saves/" + fileName + "/");
        }
        else{    
            std::string confirm = "";
            while(!(confirm == "y" || confirm == "yes")){
                std::cout << "You're about to override existing files. Are you sure, you want to continue? (y/n):\n";
                std::cin >> confirm;
                if(confirm == "n" || confirm == "no"){
                    std::cout << "Save canceled\n";
                    return EAction::EActionWait;
                }
            }
            std::cout << "Saving!\n";
        }      
        for(int i = 0; i < sim::get().predatorRobots.size(); i++){
            sim::get().predatorRobots[i]->network.saveToFile("saves/" + fileName + "/predator_" + std::to_string(i) + ".jn");
        } 
        for(int i = 0; i < sim::get().preyRobots.size(); i++){
            sim::get().preyRobots[i]->network.saveToFile("saves/" + fileName + "/prey_" + std::to_string(i) + ".jn");
        } 
        return EAction::EActionWait;

    }
    else if(cinString == "load"){
        std::string fileName;
        std::cout << "Save name:\n";
        std::cin >> fileName;
        if(! std::filesystem::exists("saves/" + fileName + "/")){
            std::cout << "Not found!\n";
            return EAction::EActionWait;
        }
        for(int i = 0; i < sim::get().predatorRobots.size(); i++){
            sim::get().predatorRobots[i]->network.loadFromFile("saves/" + fileName + "/predator_" + std::to_string(i) + ".jn");
        } 
        for(int i = 0; i < sim::get().preyRobots.size(); i++){
            sim::get().preyRobots[i]->network.loadFromFile("saves/" + fileName + "/prey_" + std::to_string(i) + ".jn");
        } 
        return EAction::EActionWait;

    }
    std::cout << "Command \"" << cinString << "\" not recognized\n";
    return EAction::EActionWait;

}

int main(int argc, char *argv[]) {
    oglu::setupOglu();
    oglu::setupGlfw();
    if constexpr (ALLOW_VISUALS){   // Open window to initialize GLEW and nitialize drawables;
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