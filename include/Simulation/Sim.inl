#pragma once

#include "Simulation/Sim.hpp"

erppm::sim& erppm::sim::get()
{
    static erppm::sim singleton;
    return singleton;
}

static void resetScore(const std::vector<erppm::RobotBase*>& robots)
{
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++){
        robots[robotIndex]->network.score = 0;
    }
}

static void resetRobotSetup(const std::vector<erppm::RobotBase*>& robots)
{
    for(unsigned int robotIndex = 0; robotIndex < robots.size(); robotIndex++){
        robots[robotIndex]->clearVelocity();
    }
}

template <
    bool OPEN_WINDOW,
    int PREDATOR_LEARNING_MODE,
    int PREY_LEARNING_MODE,
    bool PLACE_ON_LASER_SENORS
>
void erppm::sim::runRobot
(
    unsigned int threadId,
    float preyPositionAngle
)
{
    constexpr bool DISAPPEAR_TARGET = true;

    const std::vector<erppm::RobotBase*>& predatorRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePredator);
    const std::vector<erppm::RobotBase*>& preyRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePrey);
    const std::vector<erppm::Wall>& walls = erppm::env::get().walls;
    const erppm::Floor& floor = erppm::env::get().floor;
    
    static const unsigned int processor_count = std::thread::hardware_concurrency();
    unsigned int begin; 
    unsigned int end;
    if constexpr (OPEN_WINDOW)
    {
        begin  = 0; 
        end    = 10;
    }
    else
    {
        begin  = (unsigned int)( predatorRobots.size() * ((float)threadId / (float)processor_count)); 
        end    = (unsigned int)( predatorRobots.size() * ((float)(threadId+1) / (float)processor_count));
    }
    // std::cout << threadId << " : " << begin << " - " << end << "\n";

    for (unsigned int robotIndex = begin; robotIndex < end; robotIndex++)
    {
        bool targetGone = false;
        bool preyCaught = false;
        erppm::RobotBase& predatorRobot = *(predatorRobots[robotIndex]);
        erppm::RobotBase& preyRobot = *(preyRobots[robotIndex]);
        std::vector<erppm::RobotBase*> robotsInCurrentRun = {&predatorRobot, &preyRobot};
        predatorRobot.network.clearOutputValues();
        preyRobot.network.clearOutputValues();
        unsigned int stepNumber = 0;

        for(; stepNumber< numberOfStepsPerGeneration; stepNumber++)
        {
            for (std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun.begin(); iter != robotsInCurrentRun.end(); iter = std::next(iter))
            {
                (*iter)->measure(robotsInCurrentRun, walls, floor);
            }
            if constexpr (PREDATOR_LEARNING_MODE != 0)
            {
                predatorRobot.network.run(predatorRobot.getSensorData());
                predatorRobot.run(stepDuration, predatorRobot.network.getOutput());
            }
            if constexpr (PREY_LEARNING_MODE != 0)
            {
                preyRobot.network.run(preyRobot.getSensorData());
                preyRobot.run(stepDuration, preyRobot.network.getOutput());
            }
            if (glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) < 2.0)
            {
                preyCaught = true;
                break;
            }
            for (std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun.begin(); iter != robotsInCurrentRun.end(); iter = std::next(iter))
            {
                (*iter)->collide(robotsInCurrentRun, walls, floor);
                (*iter)->updateSensorsPosition();
            }
            if constexpr ( OPEN_WINDOW )
            {
                // std::cout << predatorRobot.network.getOutput().size() << " " << predatorRobot.network.getOutput()[0] << " " << predatorRobot.network.getOutput()[1] << "\r";
                erppm::win::run();
                if (!erppm::win::isAlive()) return;
            }
            if constexpr (DISAPPEAR_TARGET)
            {
                if (!targetGone)
                {
                    robotsInCurrentRun = {&predatorRobot};
                    targetGone = true;
                }
            }
        }
        if constexpr (PREDATOR_LEARNING_MODE != 0 && !OPEN_WINDOW)
        {
            double runError = 0;
            if constexpr (!DISAPPEAR_TARGET)
            {
                runError += stepNumber;
                runError += (preyCaught ? 0.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) /*/ preyPositionRadius*/);
            }
            // double rotationDiff = glm::pi<float>() - abs(glm::modf(abs(predatorRobot.getRotation()[2] - preyPositionAngle), glm::two_pi<float>()) - glm::pi<float>());
            runError += abs(predatorRobot.getRotation()[2] - preyPositionAngle);
            predatorRobot.network.score += runError;
        }
        if constexpr (PREY_LEARNING_MODE != 0 && !OPEN_WINDOW)
        {
            preyRobot.network.score = (preyCaught ? 0.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) /*/ preyPositionRadius*/) + stepNumber;
        }
    }
}



template <
    bool OPEN_WINDOW,
    int PREDATOR_LEARNING_MODE,
    int PREY_LEARNING_MODE,
    bool PLACE_ON_LASER_SENORS
>
void erppm::sim::loop(unsigned int sensorsCount)
{
    sensorsCount = (sensorsCount == -1 ? erppm::cfg::numberOfSensors : sensorsCount); 
    unsigned int generationNumber = 0;
    const std::vector<erppm::RobotBase*>& predatorRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePredator);
    const std::vector<erppm::RobotBase*>& preyRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePrey);
    for (; generationNumber< numberOfGenerations; generationNumber++)
    {
        resetScore(predatorRobots);
        resetScore(preyRobots);
        for (unsigned int repetition = 0; repetition < 100; repetition++) // need to prepare a
        { 
            if constexpr (OPEN_WINDOW){
                if (!erppm::win::isAlive()) return;
            }
            // Prepare generation
            float preyPositionAngle = 0;
            erppm::env::get().setRobotsCenterSame(predatorRobots);
            if constexpr (PLACE_ON_LASER_SENORS)
            {
                preyPositionAngle = erppm::env::get().setRobotsOnLaserSame(preyRobots, predatorRobots[0], sensorsCount);
            }
            else
            {
                erppm::env::get().setRobotsRandomConcentricSame(preyRobots);
            }
            if constexpr ( OPEN_WINDOW )
            {
                erppm::sim::runRobot<
                    OPEN_WINDOW,
                    PREDATOR_LEARNING_MODE,
                    PREY_LEARNING_MODE,
                    PLACE_ON_LASER_SENORS
                >(0, preyPositionAngle);
            }
            else 
            {
                static const auto processor_count = std::thread::hardware_concurrency();
                std::vector<std::future<void>> futures(processor_count);
                for( int threadId = 0; threadId < futures.size(); threadId++ )
                {
                    futures[threadId] = std::async
                    (
                        std::launch::async,
                        &erppm::sim::runRobot<
                            OPEN_WINDOW,
                            PREDATOR_LEARNING_MODE,
                            PREY_LEARNING_MODE,
                            PLACE_ON_LASER_SENORS
                        >,
                        this,
                        threadId,
                        preyPositionAngle
                    );

                }
                for( auto& f : futures )
                {
                    f.get();
                }
            }
        }
        if constexpr (!OPEN_WINDOW)
        {
            if constexpr (PREDATOR_LEARNING_MODE != 0)
            {
                erppm::env::get().robots.sortBase(erppm::EObjectTypePredator);
                std::cout << "PREDATOR : " 
                    << "generation " << generationNumber 
                    << ":\tpopulationError " << std::accumulate(predatorRobots.begin(), predatorRobots.end(), 0.0, [](double s, const erppm::RobotBase* a)
                    {return s + a->network.score;})/(predatorRobots.size()) 
                    << ",\tbest " << predatorRobots[0]->network.score 
                    << ",\tworst " << predatorRobots.back()->network.score 
                    // << ",\t biggest error " <<  biggestError 
                    // << ",\t precision " << precision/(networks.size())*100.0 << "%" 
                    << "\n";
            }
            if constexpr (PREDATOR_LEARNING_MODE == 2)
            {
                unsigned int eliminated_index = erppm::cfg::numberOfSurvivingRobots;
                for(int i = 0; i < erppm::cfg::numberOfSurvivingRobots; i++)
                {
                    for(int j = i+1; j < erppm::cfg::numberOfSurvivingRobots; j++)
                    {
                        predatorRobots[eliminated_index]->network.evolveFrom((predatorRobots[i]->network),(predatorRobots[j]->network));
                        eliminated_index++;
                    }
                }
            }
            else if(PREDATOR_LEARNING_MODE == 3)
            {
                // TODO automatic
                // NOTE this evolve
                preyRobots[5]->evolveFrom(*(preyRobots[0]),*(preyRobots[1]));
                preyRobots[4]->evolveFrom(*(preyRobots[0]),*(preyRobots[2]));
                preyRobots[3]->evolveFrom(*(preyRobots[1]),*(preyRobots[2]));
            }

            if constexpr (PREY_LEARNING_MODE != 0)
            {
                erppm::env::get().robots.sortBase(erppm::EObjectTypePrey); // TODO reverse?
                std::cout << "PREY : " 
                    << "generation " << generationNumber 
                    << ":\tpopulationError " << std::accumulate(preyRobots.begin(), preyRobots.end(), 0.0, [](double s, const erppm::RobotBase* a)
                    {return s + a->network.score;})/(preyRobots.size()) 
                    << ",\tbest " << preyRobots[0]->network.score 
                    << ",\tworst " << preyRobots.back()->network.score 
                    // << ",\t biggest error " <<  biggestError 
                    // << ",\t precision " << precision/(networks.size())*100.0 << "%" 
                    << "\n";   
            }
            if constexpr (PREY_LEARNING_MODE == 2)
            {
                preyRobots[5]->network.evolveFrom((preyRobots[0]->network),(preyRobots[1]->network));
                preyRobots[4]->network.evolveFrom((preyRobots[0]->network),(preyRobots[2]->network));
                preyRobots[3]->network.evolveFrom((preyRobots[1]->network),(preyRobots[2]->network));
            }
            else if(PREY_LEARNING_MODE == 3)
            {
                preyRobots[5]->evolveFrom(*(preyRobots[0]),*(preyRobots[1]));
                preyRobots[4]->evolveFrom(*(preyRobots[0]),*(preyRobots[2]));
                preyRobots[3]->evolveFrom(*(preyRobots[1]),*(preyRobots[2]));
            }
        }
    }
}

void erppm::sim::loopTemplateSwitching
(
    bool OPEN_WINDOW,
    int PREDATOR_LEARNING_MODE,
    int PREY_LEARNING_MODE,
    int PLACE_ON_LASER_SENORS
)
{
    #define GEN_SELECT() \
        SELECT_WINDOW()

    #define SELECT_WINDOW() \
        if     (OPEN_WINDOW) {SELECT_PREDATOR_LM(true)} \
        else                 {SELECT_PREDATOR_LM(false)}

    #define SELECT_PREDATOR_LM(...) \
        if     (PREDATOR_LEARNING_MODE == 0) {SELECT_PREY_LM(__VA_ARGS__, 0)} \
        else if(PREDATOR_LEARNING_MODE == 1) {SELECT_PREY_LM(__VA_ARGS__, 1)} \
        else if(PREDATOR_LEARNING_MODE == 2) {SELECT_PREY_LM(__VA_ARGS__, 2)} \
        else { throw; }

    #define SELECT_PREY_LM(...) \
        if     (PREY_LEARNING_MODE == 0) {SELECT_PLACE_LASER(__VA_ARGS__, 0)} \
        else if(PREY_LEARNING_MODE == 1) {SELECT_PLACE_LASER(__VA_ARGS__, 1)} \
        else if(PREY_LEARNING_MODE == 2) {SELECT_PLACE_LASER(__VA_ARGS__, 2)} \
        else { throw; }

    #define SELECT_PLACE_LASER(...) \
        if     (PLACE_ON_LASER_SENORS ==-1) {EXEC(__VA_ARGS__, true)} \
        else if(PLACE_ON_LASER_SENORS == 0) {EXEC(__VA_ARGS__, false)} \
        else                                {EXEC(__VA_ARGS__, true)}

    #define EXEC(v1, v2, v3, v4) \
        erppm::sim::loop<v1, v2, v3, v4>(PLACE_ON_LASER_SENORS);

    SELECT_WINDOW();
    
    #undef GEN_SELECT
    #undef SELECT_WINDOW
    #undef SELECT_PREDATOR_LM
    #undef SELECT_PREY_LM
    #undef SELECT_PLACE_LASER
    #undef EXEC
}


 