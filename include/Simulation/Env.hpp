#pragma once

#include <vector>
#include <unordered_map>
#include <string>
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
        std::vector<erppm::Wall>* currentWallSet;
        std::unordered_map<std::string, std::vector<erppm::Wall>> allWallSets;
        erppm::RobotContainer robots;
        void setRobotsCenter(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsCenterSame(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandom(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandomSame(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandomConcentric(const std::vector<erppm::RobotBase*>& robots);
        void setRobotsRandomConcentricSame(const std::vector<erppm::RobotBase*>& robots);
        float setRobotsOnLaser(const std::vector<erppm::RobotBase*>& robots, erppm::RobotBase*const referenceRobot, const int laserOnSensor);
        float setRobotsOnLaserSame(const std::vector<erppm::RobotBase*>& robots, erppm::RobotBase*const referenceRobot, const int laserOnSensor);
        void hideWalls();
        void showWalls();
        void useScenario_random();
        void useScenario_demo();
        void useScenario_empty();
        void useScenario_0();
        void useScenario_1();
        void useScenario_2();
        void useScenario_3();
        void useScenario_4();
        void setupRobot_demo();
        void setupRobot_roles();
    
    private:
        void setupField(const float& xSize, const float& ySize);
        void placeWall(const float x, const float y, const float size);
        void initRobots
        (
            const erppm::EObjectType robotType,
            const glm::vec4& color
        );
        env();

    };
}