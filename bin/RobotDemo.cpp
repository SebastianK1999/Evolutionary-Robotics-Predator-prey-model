#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include "oglUtil/oglu.hpp"
#include "oglUtil_OLD/Cube.hpp"
#include "oglUtil_OLD/Directions.hpp"
#include "oglUtil_OLD/Mesh.hpp"
#include "Drawables/Floor.hpp"
#include "whereami.h"

#include <glm/glm.hpp>

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
    
    void moveOjects();
    void colisions();

    Directions dir;
    erppm::Floor floor;
    Mesh mesh;
    oglu::Camera camera;
    oglu::Camera camera2;
    bool active;
    bool freeCamera;
};

MainWindow::~MainWindow(){

}

MainWindow::MainWindow(const int _width, const int _height, const std::string& _name) 
: oglu::Window(_width, _height, _name, false)
, dir()
, floor()
, mesh("../res/obj_models/round_robot/")
, camera(getGLFWwindowPtr())
, camera2(getGLFWwindowPtr())
, active(true)
, freeCamera(false)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    
    glfwSetInputMode(getGLFWwindowPtr(), GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(getGLFWwindowPtr(), GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetInputMode(getGLFWwindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();

    floor.scale.y = 300;
    floor.scale.x = 300;
    mesh.primaryColor = {0.6, 0.2, 0.2};
    camera.radius = 10;
    camera.minVerticalRotation = 0.0;
    camera.maxVerticalRotation = glm::half_pi<float>();

    camera2.radius = 2;
    camera2.aspect = 1.0;
    
}

void MainWindow::mainFunction(){
    if(active){
	    // Get mouse position
        int xsize;
        int ysize;
        glfwGetWindowSize(getGLFWwindowPtr(), &xsize, &ysize);
        double xpos;
        double ypos;
        glfwGetCursorPos(getGLFWwindowPtr(), &xpos, &ypos);
		glfwSetCursorPos(getGLFWwindowPtr(), xsize/2, ysize/2);
        camera.rotate(xpos - xsize/2, ypos - ysize/2);

        if (keyTracker.isJustPressed(GLFW_KEY_ESCAPE)){
            glfwSetInputMode(getGLFWwindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
        moveOjects();
        colisions();
    }
	else if(glfwGetMouseButton( getGLFWwindowPtr(), GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS){
		glfwSetInputMode(getGLFWwindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        active = true;
	}

}

void MainWindow::drawFunction(){
    camera.computeMatrices();
    floor.draw(camera.getMVP());
    mesh.draw(camera.getMVP(), camera.position);

    camera2.computeMatrices();
    int xsize, ysize;
    glClear(GL_DEPTH_BUFFER_BIT); 
    glfwGetWindowSize(getGLFWwindowPtr(), &xsize, &ysize);
    glViewport(xsize - ysize/7, 0, ysize/7, ysize/7);
    glScissor( xsize - ysize/7, 0, ysize/7, ysize/7);
    dir.draw(camera2.getMVP());
    glClear(GL_DEPTH_BUFFER_BIT); 
    glViewport(0, 0, xsize, ysize);
    glScissor(0, 0, xsize, ysize);

}

void MainWindow::afterResize(const int width, const int height){
    camera.aspect = (float)width/height;
    camera2.aspect = 1.0;
}

void MainWindow::moveOjects(){
    const static double friction = 2.0;
    static double torque = 0.0;
    static double momentum = 0.0;
    static double time0 = glfwGetTime();


    if(freeCamera) {
        if(keyTracker.isPressed(GLFW_KEY_W)){
            camera.position += camera.getDirection() * 0.01f * camera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_S)){
            camera.position -= camera.getDirection() * 0.01f * camera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_A)){
            camera.position += camera.getRightVector() * 0.01f * camera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_D)){
            camera.position -= camera.getRightVector() * 0.01f * camera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_E)){
            camera.position.z += 0.01f * camera.position.z;
        }
        if(keyTracker.isPressed(GLFW_KEY_Q)){
            camera.position.z -= 0.01f * camera.position.z;
        }
    }
    else{
        if(keyTracker.isPressed(GLFW_KEY_W)){
            mesh.position.x += camera.getDirection().x * 0.01f;
            mesh.position.y += camera.getDirection().y * 0.01f;
        }
    
        int countAx;
        int countBtn;
        const double time = glfwGetTime();
        const double dtime = time - time0;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAx);
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countBtn);
        if(countBtn > 0){
            camera.rotate(1000 * dtime * (float)(buttons[14] - buttons[16]), 1000 * dtime * (float)(buttons[13] - buttons[15]));
        }
        if(countAx > 0){
            camera.rotate(((axes[2] + 1.0) - (axes[5] + 1.0))*15, 0);

            const float left = -(abs(axes[1]) > 0.1 ? axes[1] : 0.0);
            const float right = -(abs(axes[4]) > 0.1 ? axes[4] : 0.0);
            mesh.rotation[2] += (right - left) * friction * dtime;
            const float forward = /*(left * right > 0 ? (abs(left) < abs(right) ? left : right) : (0) )*/ + (right + left);
            mesh.position.x += cosf(mesh.rotation[2]) * forward * friction * dtime;
            mesh.position.y += sinf(mesh.rotation[2]) * forward * friction * dtime;
                
            time0 = time;
        }
        camera.lookAt(mesh.position + glm::vec4(0,0,2.0 * ((camera.maxVerticalRotation - camera.getRotation().y) / (camera.maxVerticalRotation - camera.minVerticalRotation)),0));
    }
    camera2.setRotation(camera.getRotation());
    camera2.lookAt(dir.position);
}

void MainWindow::colisions(){
    
}

int main(int argc, char *argv[]) {
    oglu::setupOglu();
    oglu::setupGlfw();
    MainWindow window(800, 600, "Robot demo");
    
    window.keyTracker.addKey(GLFW_KEY_F);
    window.keyTracker.addKey(GLFW_KEY_F11);
    window.keyTracker.addKey(GLFW_KEY_ESCAPE);
    window.keyTracker.addKey(GLFW_KEY_TAB);
    window.keyTracker.addKey(GLFW_KEY_W);
    window.keyTracker.addKey(GLFW_KEY_S);
    window.keyTracker.addKey(GLFW_KEY_A);
    window.keyTracker.addKey(GLFW_KEY_D);
    window.keyTracker.addKey(GLFW_KEY_Q);
    window.keyTracker.addKey(GLFW_KEY_E);
    
    window.mainLoop();

    return 0;
}