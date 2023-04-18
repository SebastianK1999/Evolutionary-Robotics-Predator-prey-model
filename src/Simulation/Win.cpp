#include <new>

#include "oglUtil_OLD/Misc.hpp"

#include "Robotics/LaserSensor.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Simulation/Env.hpp"

#include "Simulation/Win.hpp"

const std::vector<uint8_t> erppm::win::winBuffer(sizeof(erppm::win), 0);
erppm::win* erppm::win::winPtr = nullptr;

void erppm::win::open()
{
    if (winPtr == nullptr)
    {
        winPtr = reinterpret_cast<erppm::win*>(const_cast<uint8_t*>(winBuffer.data()));
        new(winPtr) erppm::win(800, 800, "Evolutionary Robotics");
    }
}

void erppm::win::destroy()
{
    if (winPtr!=nullptr)
    {
        winPtr->~win();
        winPtr = nullptr;
    }
}

void erppm::win::run()
{
    if (winPtr!=nullptr)
    {
        if(glfwWindowShouldClose(winPtr->getGlfwWindowPtr()) == 0 && winPtr->getGlfwWindowPtr() != nullptr)
        {
            winPtr->renderWindow();
        }
        else
        {
            erppm::win::destroy();
        }
    }
}

bool erppm::win::isAlive()
{
    return winPtr!=nullptr;
}
erppm::win::win(const int _width, const int _height, const std::string &_name)
    : oglu::Window(_width, _height, _name, false), active(true), freeCamera(false), cameraFocusPoint(0, 0, 0), mainCamera(getGlfwWindowPtr())
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

    keyTracker.addKey(GLFW_KEY_F);
    keyTracker.addKey(GLFW_KEY_F11);
    keyTracker.addKey(GLFW_KEY_ESCAPE);
    keyTracker.addKey(GLFW_KEY_TAB);
    keyTracker.addKey(GLFW_KEY_ENTER);
    keyTracker.addKey(GLFW_KEY_SPACE);
    keyTracker.addKey(GLFW_KEY_W);
    keyTracker.addKey(GLFW_KEY_S);
    keyTracker.addKey(GLFW_KEY_A);
    keyTracker.addKey(GLFW_KEY_D);
    keyTracker.addKey(GLFW_KEY_Q);
    keyTracker.addKey(GLFW_KEY_E);
    
    erppm::env::get().useScenario_empty(); // TODO - move to cmd

    mainCamera.radius = 30;
    mainCamera.setRotation({0,1,0});
    mainCamera.minVerticalRotation = 0.5;
    mainCamera.maxVerticalRotation = glm::half_pi<float>();
    
    // TODO move somewhere else
    // mainRobotInput.resize(robots[mainRobotIndex]->getControlInputSize());
}

erppm::win::~win()
{
    
}

void erppm::win::mainFunction()
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

void erppm::win::drawFunction()
{
    mainCamera.computeMatrices();
    erppm::Floor::drawInstances(mainCamera.getMVP(), mainCamera.position);
    erppm::Wall::drawInstances(mainCamera.getMVP(), mainCamera.position);
    erppm::RoundRobot::draw(mainCamera.getMVP(), mainCamera.position);
    erppm::LaserSensor::draw(mainCamera.getMVP(), mainCamera.position); 
}

void erppm::win::moveObjects() // TODO Move to env, or cmd in a custom  
{
    // static double time0 = glfwGetTime();
    // const double time = glfwGetTime();
    // const double dtime = time - time0;
    if(active && !freeCamera)
    {
        // if(keyTracker.isJustReleased(GLFW_KEY_ENTER))
        // {
        //     mainRobotIndex = (mainRobotIndex + 1) %  robots.size();
        // }
        // // Gamepad data
        // int countAx;
        // // int countBtn;
        // const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAx);
        // if(countAx > 0){
        //     mainRobotInput[0] = -(abs(axes[1]) > 0.1 ? axes[1] : 0.0);
        //     mainRobotInput[1] = -(abs(axes[4]) > 0.1 ? axes[4] : 0.0);
        //     robots[mainRobotIndex]->run(dtime, mainRobotInput);
        // }
    }
    // time0 = time;
}

void erppm::win::collisions() // TODO Move to env, or cmd in a custom  
{
    if(active)
    {
        // for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter))
        // {
        //     (*iter)->collide(robots, erppm::env::get().walls, erppm::env::get().floor);
        //     (*iter)->updateSensorsPosition();
        // }
        // for(std::vector<erppm::RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter))
        // {
        //     (*iter)->measure(robots, erppm::env::get().walls, erppm::env::get().floor);
        // }
    }
}

void erppm::win::moveCamera()
{
    // TODO camera with own focus point, not a robot
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
        if(freeCamera)
        {
            // Warning! camera position cant't be z = 0 
            mainCamera.rotate((xPos - xSize/2) * cameraMouseSensitivity * dtime, (yPos - ySize/2) * cameraMouseSensitivity * dtime);
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
                if(cameraFocusPoint.x > erppm::env::get().floor.getScale().x){
                    cameraFocusPoint.x = erppm::env::get().floor.getScale().x;
                }
                else if(cameraFocusPoint.x < -erppm::env::get().floor.getScale().x){
                    cameraFocusPoint.x = -erppm::env::get().floor.getScale().x;
                }
                if(cameraFocusPoint.y > erppm::env::get().floor.getScale().y){
                    cameraFocusPoint.y = erppm::env::get().floor.getScale().y;
                }
                else if(cameraFocusPoint.y < -erppm::env::get().floor.getScale().y){
                    cameraFocusPoint.y = -erppm::env::get().floor.getScale().y;
                }
            }
            mouseState = glfwGetMouseButton(getGlfwWindowPtr(), GLFW_MOUSE_BUTTON_MIDDLE);
            if (mouseState == GLFW_PRESS)
            {
                mainCamera.radius += (yPos - ySize/2) * dtime;
            }
            mainCamera.lookAt(cameraFocusPoint);
        }
    }
    time0 = time;
}

void erppm::win::afterResize(const int width, const int height)
{
    mainCamera.aspect = (float)width/height;
}