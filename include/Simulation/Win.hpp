#pragma once

#include <optional>
#include <new>

#include "Robotics/RobotBase.hpp"
#include "oglUtil/tools/Window.hpp"
#include "oglUtil/tools/Camera.hpp"

namespace erppm
{
    class win : public oglu::Window 
    {
    public:
        static void open();
        static void destroy();
        static void run();
        static bool isAlive();
        ~win();

    private:
        static const std::vector<uint8_t> winBuffer;
        static erppm::win* winPtr;
        // static std::optional<erppm::win> instance;
        bool active;
        bool freeCamera;
        glm::vec3 cameraFocusPoint;
        oglu::Camera mainCamera;

        win(const int _width, const int _height, const std::string& _name);
        void drawFunction() override;
        void mainFunction() override;
        void afterResize(const int width, const int height) override;
        void moveObjects();
        void collisions();
        void moveCamera();
    };
}