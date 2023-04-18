#pragma once

#include <vector>
#include <list>

#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/LaserSensor.hpp"
#include "Robotics/RobotContainer.hpp"

namespace erppm
{
    class env
    {
    public:
        static erppm::env& get();

        erppm::Floor floor;
        erppm::Wall wallPlusY; 
        erppm::Wall wallMinusY;
        erppm::Wall wallPlusX; 
        erppm::Wall wallMinusX;
        std::vector<erppm::Wall> walls;
        erppm::RobotContainer robots;
        // std::vector<erppm::RobotBase*> robots;
        // std::vector<erppm::RobotBase*> predatorRobots;
        // std::vector<erppm::RobotBase*> preyRobots;
        // template <typename T>
        // void addRobot(T&& robot);
        // template <typename T>
        // std::list<T>& getRobotList();
        void setRobotsCenter(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsCenterSame(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandom(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandomSame(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandomConcentric(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandomConcentricSame(const std::vector<erppm::RobotBase*>& robots);
        float setRobotsOnLaser(const std::vector<erppm::RobotBase*>& robots, erppm::RobotBase*const referenceRobot, const int laserOnSensor);
        float setRobotsOnLaserSame(const std::vector<erppm::RobotBase*>& robots, erppm::RobotBase*const referenceRobot, const int laserOnSensor);
        void useScenario_demo();
        void useScenario_empty();
        void useScenario_0();
        void useScenario_1();
    
    private:
        void setupField(const float& xSize, const float& ySize);
        void initRobots
        (
            const erppm::EObjectType robotType,
            const glm::vec4& color
        );
        env();

    };
}