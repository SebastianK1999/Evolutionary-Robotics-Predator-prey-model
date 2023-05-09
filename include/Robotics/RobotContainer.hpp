#pragma once

#include <list>
#include <vector>
#include <algorithm>

#include "Robotics/RobotBase.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/ObjectTypes.hpp"

namespace erppm
{
    class RobotContainer
    {
    public:
        void clear();
        void sortRound(const erppm::EObjectType type = erppm::EObjectTypeNone);
        void sortBaseIncremental(const erppm::EObjectType type = erppm::EObjectTypeNone);
        void sortBaseDecremental(const erppm::EObjectType type = erppm::EObjectTypeNone);
        void reserveRound(const size_t capacity, const erppm::EObjectType type);
        void emplaceBackRound(erppm::RoundRobot&& robot, const erppm::EObjectType type);
        const std::vector<erppm::RoundRobot*>& getRound(const erppm::EObjectType type = erppm::EObjectTypeNone) const;
        const std::vector<erppm::RobotBase*>& getBase(const erppm::EObjectType type = erppm::EObjectTypeNone) const;

    private:
        struct RobotContainerGroup
        {
            std::list<erppm::RoundRobot> roundRobotsList;
            std::vector<erppm::RoundRobot*> roundRobots;
            std::vector<erppm::RobotBase*> robots;
        };
        RobotContainerGroup preyRobots;
        RobotContainerGroup predatorRobots;
        std::vector<erppm::RobotBase*> allRobots;
    };
}
