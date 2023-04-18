#include <utility>

#include "Robotics/RobotContainer.hpp"

void erppm::RobotContainer::clear()
{

    predatorRobots.roundRobotsList.clear();
    predatorRobots.roundRobots.clear();
    predatorRobots.robots.clear();
    preyRobots.roundRobotsList.clear();
    preyRobots.roundRobots.clear();
    preyRobots.robots.clear();
    allRobots.clear();
}

void erppm::RobotContainer::sortRound(const erppm::EObjectType type)
{
    switch(type)
    {
    case erppm::EObjectTypePredator:
        std::sort
        (
            predatorRobots.roundRobots.begin(),
            predatorRobots.roundRobots.end(),
            [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;}
        );
        break;
    case erppm::EObjectTypePrey:
        std::sort
        (
            preyRobots.roundRobots.begin(),
            preyRobots.roundRobots.end(),
            [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;}
        );
        break;
    default:
        throw;
    }
}

void erppm::RobotContainer::sortBase(const erppm::EObjectType type)
{
    switch(type)
    {
    case erppm::EObjectTypePredator:
        std::sort
        (
            predatorRobots.robots.begin(),
            predatorRobots.robots.end(),
            [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;}
        );
        break;
    case erppm::EObjectTypePrey:
        std::sort
        (
            preyRobots.robots.begin(),
            preyRobots.robots.end(),
            [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;}
        );
        break;
    case erppm::EObjectTypeNone:
        std::sort
        (
            allRobots.begin(),
            allRobots.end(),
            [](const erppm::RobotBase* a, const erppm::RobotBase* b){return a->network.score < b->network.score;}
        );
        break;
    default:
        throw;
    }
}


void erppm::RobotContainer::reserveRound(const size_t capacity, const erppm::EObjectType type)
{
    switch(type)
    {
    case erppm::EObjectTypePredator:
        allRobots.reserve(allRobots.size() - predatorRobots.roundRobots.size() + capacity);
        predatorRobots.robots.reserve(predatorRobots.robots.size() - predatorRobots.roundRobots.size() + capacity);
        predatorRobots.roundRobots.reserve(capacity);
        break;
    case erppm::EObjectTypePrey:
        allRobots.reserve(allRobots.size() - preyRobots.roundRobots.size() + capacity);
        preyRobots.robots.reserve(preyRobots.robots.size() - preyRobots.roundRobots.size() + capacity);
        preyRobots.roundRobots.reserve(capacity);
        break;
    default:
        throw;
    }
}

const std::vector<erppm::RoundRobot*>& erppm::RobotContainer::getRound(const erppm::EObjectType type) const
{
    switch(type)
    {
    case erppm::EObjectTypePredator:
        return predatorRobots.roundRobots;
    case erppm::EObjectTypePrey:
        return preyRobots.roundRobots;
    default:
        throw;
    }
}

const std::vector<erppm::RobotBase*>& erppm::RobotContainer::getBase(const erppm::EObjectType type) const
{
    switch(type)
    {
    case erppm::EObjectTypePredator:
        return predatorRobots.robots;
    case erppm::EObjectTypePrey:
        return preyRobots.robots;
    case erppm::EObjectTypeNone:
        return allRobots;
    default:
        throw;
    }
}

void erppm::RobotContainer::emplaceBackRound(erppm::RoundRobot&& robot, const erppm::EObjectType type)
{
    switch(type)
    {
    case erppm::EObjectTypePredator:
        predatorRobots.roundRobotsList.push_back(std::move(robot));
        predatorRobots.roundRobots.emplace_back(&predatorRobots.roundRobotsList.back());
        predatorRobots.robots.emplace_back(&predatorRobots.roundRobotsList.back());
        allRobots.emplace_back(&predatorRobots.roundRobotsList.back());
        break;
    case erppm::EObjectTypePrey:
        preyRobots.roundRobotsList.push_back(std::move(robot));
        preyRobots.roundRobots.emplace_back(&preyRobots.roundRobotsList.back());
        preyRobots.robots.emplace_back(&preyRobots.roundRobotsList.back());
        allRobots.emplace_back(&preyRobots.roundRobotsList.back());
        break;
    default:
        throw;
    }
}