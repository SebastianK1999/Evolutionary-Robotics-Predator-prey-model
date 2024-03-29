#include <utility>

#include "glm/gtc/constants.hpp"

#include "JimmyNeuron/Misc/Misc.hpp"

#include "Simulation/Cfg.hpp"

#include "Simulation/Env.hpp"

// template <typename T>
// std::list<T>& erppm::env::getRobotList(){
//     static std::list<T> robots_T;              // it has to be a list, so the pointers won't change when vector relocates data
//     return robots_T;
// }

// template <typename T>
// void erppm::env::addRobot(T&& robot){
//     std::list<T>& robots_T = getRobotList<T>();
//     robots_T.push_back(std::move(robot));
//     erppm::RobotBase* rp =  &(robots_T.back());
//     robots.push_back( rp );
// }

void erppm::env::setRobotsCenter(const std::vector<erppm::RobotBase*>& robots)
{
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        const float rotation = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
        const glm::vec3 position = {0.0f, 0.0f, 0.0f};
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
}

void erppm::env::setRobotsCenterSame(const std::vector<erppm::RobotBase*>& robots)
{
    const float rotation = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
    const glm::vec3 position = {0.0f, 0.0f, 0.0f};
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
}

void erppm::env::setRobotsRandom(const std::vector<erppm::RobotBase*>& robots)
{
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        const float rotation = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
        const glm::vec3 position 
        = {
            Jimmy::Misc::rand().decimal(-floor.getScale().x, floor.getScale().x),
            Jimmy::Misc::rand().decimal(-floor.getScale().y, floor.getScale().y),
            0.0f
        };
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
}

void erppm::env::setRobotsRandomSame(const std::vector<erppm::RobotBase*>& robots)
{
    const float rotation = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
    const glm::vec3 position 
    = {
        Jimmy::Misc::rand().decimal(-floor.getScale().x, floor.getScale().x),
        Jimmy::Misc::rand().decimal(-floor.getScale().y, floor.getScale().y),
        0.0f
    };
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
    // Loop robots to update Color
    
}

void erppm::env::setRobotsRandomConcentric(const std::vector<erppm::RobotBase*>& robots)
{
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        const float rotation = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
        const float positionAngle = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
        const float positionRadius 
        = Jimmy::Misc::rand().decimal ( glm::min (
            floor.getScale().x,
            floor.getScale().y
        ));
        const glm::vec3 position 
        = { 
            positionRadius * glm::cos(positionAngle),
            positionRadius * glm::sin(positionAngle),
            0.0f
        };
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
}

void erppm::env::setRobotsRandomConcentricSame(const std::vector<erppm::RobotBase*>& robots)
{
    const float rotation = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
    const float positionAngle = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
    const float positionRadius 
    = Jimmy::Misc::rand().decimal ( glm::min (
        floor.getScale().x,
        floor.getScale().y
    ));
    const glm::vec3 position 
    = { 
        positionRadius * glm::cos(positionAngle),
        positionRadius * glm::sin(positionAngle),
        0.0f
    };
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
}


float erppm::env::setRobotsOnLaser(const std::vector<erppm::RobotBase*>& robots, erppm::RobotBase*const referenceRobot, const int laserOnSensor)
{
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        const float rotation = Jimmy::Misc::rand().decimal(0, glm::two_pi<float>());
        std::list<erppm::LaserSensor>::iterator begin = referenceRobot->getSensorList<erppm::LaserSensor>().begin();
        int laserSensorIndex = Jimmy::Misc::rand().randInt(0, laserOnSensor);
        const float positionAngle = (*std::next(begin, laserSensorIndex)).evolutionaryData[0] + referenceRobot->getRotation().z;
        const float positionRadius 
        = Jimmy::Misc::rand().decimal (
            5,
            std::min(floor.getScale().x, floor.getScale().y)/2
        );
        const glm::vec3 position 
        = { 
            positionRadius * glm::cos(positionAngle),
            positionRadius * glm::sin(positionAngle),
            0.0f
        };
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
    return 0;
}

float erppm::env::setRobotsOnLaserSame(const std::vector<erppm::RobotBase*>& robots, erppm::RobotBase*const referenceRobot, const int laserOnSensor)
{
    std::list<erppm::LaserSensor>::iterator begin = referenceRobot->getSensorList<erppm::LaserSensor>().begin();
    int laserSensorIndex = Jimmy::Misc::rand().randInt(0, laserOnSensor);
    const float positionAngle = (*std::next(begin, laserSensorIndex)).evolutionaryData[0] + referenceRobot->getRotation().z;
    const float positionRadius 
    = Jimmy::Misc::rand().decimal (
        5,
        std::min(floor.getScale().x, floor.getScale().y)/2
    );
    const glm::vec3 position 
    = { 
        positionRadius * glm::cos(positionAngle),
        positionRadius * glm::sin(positionAngle),
        0.0f
    };
    begin = robots[0]->getSensorList<erppm::LaserSensor>().begin();
    laserSensorIndex = Jimmy::Misc::rand().randInt(0, laserOnSensor);
    const float rotation = glm::pi<float>() - (*std::next(begin, laserSensorIndex)).evolutionaryData[0] + referenceRobot->getRotation().z;
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++)
    {
        robots[robotIndex]->getPosition() = position;
        robots[robotIndex]->getRotation().z = rotation;
        robots[robotIndex]->updateSensorsPosition();
        robots[robotIndex]->clearVelocity();
    }
    return positionAngle;
}

void erppm::env::hideWalls()
{
    for (auto& wall : *currentWallSet)
    {
        wall.getPosition().z = -100;
    }
}

void erppm::env::showWalls()
{
    for (auto& wall: *currentWallSet)
    {
        wall.getPosition().z = 0;
    }
}


void erppm::env::useScenario_random()
{
    unsigned int scenario = Jimmy::Misc::rand().randInt(0, 5);
    switch (scenario)
    {
    case 0:
        useScenario_0();
        break;
    case 1:
        useScenario_1();
        break;
    case 2:
        useScenario_2();
        break;
    case 3:
        useScenario_3();
        break;
    case 4:
        useScenario_4();
        break;
    
    default:
        break;
    }
}

void erppm::env::initRobots(
    const erppm::EObjectType robotType,
    const glm::vec4 &color)
{
    if (robots.getBase(robotType).size() != 0)
    {
        return;
    }
    robots.reserveRound(erppm::cfg::numberOfRobots, robotType);
    // predatorNetworks.reserve(erppm::cfg::numberOfRobots);
    for(unsigned int i = 0; i < erppm::cfg::numberOfRobots; i++){
        robots.emplaceBackRound(erppm::RoundRobot(), robotType);
        erppm::RobotBase& robot = *(robots.getBase().back()); 
        if(color == glm::vec4(-1,-1,-1,-1))
        {
            robots.getBase().back()->getPrimaryColor() = glm::vec4
            (
                Jimmy::Misc::rand().decimal(1.0f),
                Jimmy::Misc::rand().decimal(1.0f),
                Jimmy::Misc::rand().decimal(1.0f),
                1
            );
        }
        else    
        {
            robot.getPrimaryColor() = color;
        }
        float anglePosition = 0;
        float angleOnRobot = 0;
        robot.addSensor ( erppm::LaserSensor
        (
            glm::vec3(glm::cos(anglePosition), glm::sin(anglePosition), 0.2),
            glm::vec3(0, 0, (angleOnRobot + anglePosition)),
            {anglePosition, angleOnRobot}
        ));
        constexpr double INTER_SENSOR_SPACE = glm::pi<double>() / (erppm::cfg::numberOfSensorsPerSide + 1);
        constexpr double BEGIN_SENSOR_PLACEMENT = INTER_SENSOR_SPACE ; 
        // constexpr double INTER_SENSOR_SPACE = glm::pi<double>() / erppm::cfg::numberOfSensorsPerSide;
        // constexpr double BEGIN_SENSOR_PLACEMENT = INTER_SENSOR_SPACE / 2.0; 
        for(int i = 0; i < erppm::cfg::numberOfSensorsPerSide; i++){
            for(double side : {1.0, -1.0}){
                anglePosition = side *(BEGIN_SENSOR_PLACEMENT + INTER_SENSOR_SPACE * i);
                angleOnRobot = 0;
                robot.addSensor ( erppm::LaserSensor
                (
                    glm::vec3(glm::cos(anglePosition), glm::sin(anglePosition), 0.2),
                    glm::vec3(0, 0, (angleOnRobot + anglePosition)),
                    {anglePosition, angleOnRobot}
                ));
            }
        }
        anglePosition = -glm::pi<float>();
        angleOnRobot = 0;
        robot.addSensor ( erppm::LaserSensor
        (
            glm::vec3(glm::cos(anglePosition), glm::sin(anglePosition), 0.2),
            glm::vec3(0, 0, (angleOnRobot + anglePosition)),
            {anglePosition, angleOnRobot}
        ));
        assert(erppm::cfg::numberOfSensors == robot.sensors.size());
        robot.getRotation()[2] = Jimmy::Misc::rand().decimal(glm::two_pi<float>());
        robot.type = robotType;
        robot.network.addNeurons
        (
            robot.getSensorDataSize(),
            erppm::cfg::numberOfHiddenNeurons,
            robot.getControlInputSize(), 
            erppm::cfg::numberOfMemoryNeurons
        );
        robot.network.mutate();
    }
}

erppm::env& erppm::env::get()
{
    static erppm::env singleton;
    return singleton;
}

void erppm::env::useScenario_demo()
{
    const std::string key = "demo";
    currentWallSetName = key;
    setupField(30.0f, 30.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
    for(int i = 0; i < 5; i++)
    {
        placeWall(10 + 2*i, 2*i, 2);
    }
}

void erppm::env::useScenario_empty()
{
    std::string key = "empty";
    currentWallSetName = key;
    setupField(30.0f, 30.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
}

void erppm::env::useScenario_0()
{
    std::string key = "scenario_0";
    currentWallSetName = key;
    setupField(20.0f, 20.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
    currentWallSet->reserve(4);
    placeWall(-8,-8, 2);
    placeWall(-8, 8, 2);
    placeWall( 8,-8, 2);
    placeWall( 8, 8, 2);

}

void erppm::env::useScenario_1()
{
    std::string key = "scenario_1";
    currentWallSetName = key;
    setupField(20.0f, 20.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
    currentWallSet->reserve(4);
    placeWall(-10,  0, 2);
    placeWall( 10,  0, 2);
    placeWall(  0,-10, 2);
    placeWall(  0, 10, 2);
}

void erppm::env::useScenario_2()
{
    std::string key = "scenario_2";
    currentWallSetName = key;
    setupField(20.0f, 20.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
    currentWallSet->reserve(3);
    placeWall(  0, 0, 2);
    placeWall(-10, 0, 2);
    placeWall( 10, 0, 2);
}

void erppm::env::useScenario_3()
{
    std::string key = "scenario_3";
    currentWallSetName = key;
    setupField(20.0f, 20.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
    currentWallSet->reserve(5);
    placeWall(  0,  0, 4);
    placeWall(-18,-18, 2);
    placeWall(-18, 18, 2);
    placeWall( 18,-18, 2);
    placeWall( 18, 18, 2);
    // placeWall( -4,  0);
    // placeWall(  4,  0);
    // placeWall(  0, -4);
    // placeWall(  0,  4);
}

void erppm::env::useScenario_4()
{
    std::string key = "scenario_4";
    currentWallSetName = key;
    setupField(20.0f, 20.0f);
    hideWalls();
    auto wallSetIterator = allWallSets.find(key);
    if (wallSetIterator != allWallSets.end())
    {
        currentWallSet = &(wallSetIterator->second);
        showWalls();
        return;
    }
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &(allWallSets[key]);
    currentWallSet->reserve(8);
    placeWall( 18,  0, 2);
    placeWall( 16,  0, 2);
    placeWall(-18,  0, 2);
    placeWall(-16,  0, 2);
    placeWall(  0, 18, 2);
    placeWall(  0, 16, 2);
    placeWall(  0,-18, 2);
    placeWall(  0,-16, 2);
}

void erppm::env::setupRobot_demo()
{
    // TODO resize/reserve instance vectors
    robots.clear();
    initRobots
    (
        erppm::EObjectTypePredator,
        {-1.0f, -1.0f, -1.0f, -1.0f}
    );
    initRobots
    (
        erppm::EObjectTypePrey,
        {-1.0f, -1.0f, -1.0f, -1.0f}
    );
    setRobotsRandomConcentric(robots.getBase(erppm::EObjectTypePredator));
    setRobotsRandomConcentric(robots.getBase(erppm::EObjectTypePrey));
}

void erppm::env::setupRobot_roles()
{
    robots.clear();
    initRobots
    (
        erppm::EObjectTypePredator,
        {0.6f, 0.2f, 0.2f, 1.0f}
    );
    initRobots
    (
        erppm::EObjectTypePrey,
        {0.2f, 0.2f, 0.6f, 1.0f}
    );
    setRobotsCenterSame(robots.getBase(erppm::EObjectTypePredator));
    setRobotsRandomConcentricSame(robots.getBase(erppm::EObjectTypePrey));
}

void erppm::env::placeWall(const float x, const float y, const float size)
{
        currentWallSet->emplace_back(std::move(erppm::Wall()));
        currentWallSet->back().getPosition().x = x;
        currentWallSet->back().getPosition().y = y;
        currentWallSet->back().getScale() *= size;
        currentWallSet->back().getColor() = {0.4, 0.3, 0.7, 1.0};
}

void erppm::env::setupField(const float& xSize, const float& ySize)
{
    constexpr float wallThickness = 20; 
    constexpr float wallHeight = 3; 
    const glm::vec4 wallColor = {0.6, 0.6, 0.7, 1.0}; 
    
    floor.getScale().x = xSize;
    floor.getScale().y = ySize;
    floor.getColor() = {0.6, 0.6, 0.6, 1.0};

    wallPlusY.getScale().x = floor.getScale().x; 
    wallPlusY.getScale().y = wallThickness; 
    wallPlusY.getScale().z = wallHeight; 
    wallPlusY.getPosition().y = -floor.getScale().y - wallPlusY.getScale().y; 
    wallPlusY.getColor() = wallColor; 
    wallMinusY.getScale().x = floor.getScale().x;
    wallMinusY.getScale().y = wallThickness; 
    wallMinusY.getScale().z = wallHeight; 
    wallMinusY.getPosition().y = floor.getScale().y + wallPlusY.getScale().y; 
    wallMinusY.getColor() = wallColor; 
    wallPlusX.getScale().x = wallThickness; 
    wallPlusX.getScale().y = floor.getScale().y + wallThickness*2; 
    wallPlusX.getScale().z = wallHeight; 
    wallPlusX.getPosition().x = -floor.getScale().x - wallPlusX.getScale().x; 
    wallPlusX.getColor() = wallColor; 
    wallMinusX.getScale().x = wallThickness; 
    wallMinusX.getScale().y = floor.getScale().y + wallThickness*2; 
    wallMinusX.getScale().z = wallHeight; 
    wallMinusX.getPosition().x = floor.getScale().x + wallPlusX.getScale().x; 
    wallMinusX.getColor() = wallColor;

}

erppm::env::env()
    : floor()
    , wallPlusY()
    , wallMinusY()
    , wallPlusX()
    , wallMinusX()
    , currentWallSet()
    , allWallSets()
{
    std::string key = "empty";
    setupField(30.0f, 30.0f);
    allWallSets.emplace(key, std::vector<erppm::Wall>());
    currentWallSet = &allWallSets[key];
    robots.clear();

}
