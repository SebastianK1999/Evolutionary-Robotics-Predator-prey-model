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
    bool PLACE_ON_LASER_SENORS,
    bool TARGET_DISAPPEAR
>
void erppm::sim::runRobot
(
    unsigned int threadId,
    float preyPositionAngle
)
{
    constexpr bool IS_TOURNAMENT = PREDATOR_LEARNING_MODE >=1 && PREY_LEARNING_MODE >= 1;
    constexpr bool PREDATOR_IS_PRIMARY = PREDATOR_LEARNING_MODE > PREY_LEARNING_MODE;

    const std::vector<erppm::RobotBase*>* primaryRobots;
    const std::vector<erppm::RobotBase*>* secondaryRobots;
    if constexpr (PREDATOR_IS_PRIMARY)
    {
        primaryRobots = &(erppm::env::get().robots.getBase(erppm::EObjectTypePredator));
        secondaryRobots = &(erppm::env::get().robots.getBase(erppm::EObjectTypePrey));
    }
    else
    {
        primaryRobots = &(erppm::env::get().robots.getBase(erppm::EObjectTypePrey));
        secondaryRobots = &(erppm::env::get().robots.getBase(erppm::EObjectTypePredator));
    }
    const std::vector<erppm::Wall>& walls = *erppm::env::get().currentWallSet;
    const erppm::Floor& floor = erppm::env::get().floor;

    static const unsigned int processor_count = std::thread::hardware_concurrency();
    unsigned int begin; 
    unsigned int end;
    if constexpr (OPEN_WINDOW)
    {
        begin  = 0; 
        end    = 1;
    }
    else
    {
        begin  = (unsigned int)( primaryRobots->size() * ((float)threadId / (float)processor_count)); 
        end    = (unsigned int)( primaryRobots->size() * ((float)(threadId+1) / (float)processor_count));
    }
    auto secondaryNetwork = (*secondaryRobots)[0]->network;
    for (unsigned int robotIndex = begin; robotIndex < end; robotIndex++)
    {
        bool preyCaught = false;
        erppm::RobotBase& primaryRobot = *(*primaryRobots)[robotIndex];
        erppm::RobotBase& secondaryRobot = *(*secondaryRobots)[threadId];
        std::vector<erppm::RobotBase*> robotsInCurrentRun = {&primaryRobot, &secondaryRobot};
        primaryRobot.network.clearValues();
        secondaryNetwork.clearValues();

        double lastOutput[2] = {0,0};
        unsigned int stepNumber = 0;

        for(; stepNumber< numberOfStepsPerGeneration; stepNumber++)
        {
            primaryRobot.measure(robotsInCurrentRun, walls, floor);
            primaryRobot.network.run(primaryRobot.getSensorData());
            primaryRobot.run(stepDuration, primaryRobot.network.getOutput());

            if constexpr (IS_TOURNAMENT)
            {
                secondaryRobot.measure(robotsInCurrentRun, walls, floor);
                secondaryNetwork.run(secondaryRobot.getSensorData());
                secondaryRobot.run(stepDuration, secondaryNetwork.getOutput());
            }
            if (glm::length(primaryRobot.getPosition() - secondaryRobot.getPosition()) <= 2.0)
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
                erppm::win::run();
                if (!erppm::win::isAlive()) return;
            }
            if (stepNumber == 0)
            {
                if constexpr (PREDATOR_IS_PRIMARY)
                {
                    if (preyPositionAngle == glm::pi<float>() && primaryRobot.network.getOutput()[0] > primaryRobot.network.getOutput()[1])
                    {
                        preyPositionAngle = -preyPositionAngle;          
                    }
                }
                else
                {
                    if (preyPositionAngle == glm::pi<float>() && primaryRobot.network.getOutput()[0] < primaryRobot.network.getOutput()[1])
                    {
                        preyPositionAngle = -preyPositionAngle;          
                    } 
                }
            }
            if constexpr (TARGET_DISAPPEAR)
            {
                if (stepNumber == 1)
                {
                    lastOutput[0] = primaryRobot.network.getOutput()[0];
                    lastOutput[1] = primaryRobot.network.getOutput()[1];
                }
                else if (stepNumber == 2)
                {
                    robotsInCurrentRun = {&primaryRobot};
                }
            }
        }
        if constexpr (!OPEN_WINDOW)
        {
            if constexpr (PREDATOR_IS_PRIMARY)
            {
                double runError = 0;
                if constexpr (!TARGET_DISAPPEAR)
                {
                    runError += stepNumber;
                    runError += (preyCaught ? 0.0 : glm::length(primaryRobot.getPosition() - secondaryRobot.getPosition()) /*/ preyPositionRadius*/);
                    // runError += glm::pi<float>() - abs(glm::mod(abs(primaryRobot.getRotation()[2] - preyPositionAngle), glm::two_pi<float>()) - glm::pi<float>());
                    if ( !randomEnv ) runError += abs(primaryRobot.getRotation()[2] - preyPositionAngle);
                }
                else
                {
                    // runError += abs(lastOutput_predator[0] - primaryRobot.network.getOutput()[0]) + abs(lastOutput_predator[1] - primaryRobot.network.getOutput()[1]);
                    runError += glm::pi<float>() - abs(glm::mod(abs(primaryRobot.getRotation()[2] - preyPositionAngle), glm::two_pi<float>()) - glm::pi<float>());
                    // runError += abs(primaryRobot.getRotation()[2] - preyPositionAngle);
                }
                primaryRobot.network.score += runError;
            }
            else
            {
                double runError = 0;
                if constexpr (!TARGET_DISAPPEAR)
                {
                    if ( randomEnv ) runError += stepNumber;
                    if ( !randomEnv ) runError += (preyCaught ? -1.0 : glm::length(secondaryRobot.getPosition() - primaryRobot.getPosition()) /*/ preyPositionRadius*/);
                    // runError += glm::pi<float>() - abs(glm::mod(abs(primaryRobot.getRotation()[2] - preyPositionAngle_prey), glm::two_pi<float>()) - glm::pi<float>());
                    // runError += abs(primaryRobot.getRotation()[2] - preyPositionAngle_prey);
                }
                else
                {
                    runError += abs(lastOutput[0] - primaryRobot.network.getOutput()[0]) + abs(lastOutput[1] - primaryRobot.network.getOutput()[1]);
                    runError += abs(glm::mod(abs(primaryRobot.getRotation()[2] - preyPositionAngle), glm::two_pi<float>()) - glm::pi<float>());
                    // runError += abs(primaryRobot.getRotation()[2] - preyPositionAngle_prey);
                }
                primaryRobot.network.score += runError;
            }
        }
    }







    // float preyPositionAngle_predator = preyPositionAngle;
    // float preyPositionAngle_prey = preyPositionAngle;
    
    // const std::vector<erppm::RobotBase*>& predatorRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePredator);
    // const std::vector<erppm::RobotBase*>& preyRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePrey);
    // const std::vector<erppm::Wall>& walls = *erppm::env::get().currentWallSet;
    // const erppm::Floor& floor = erppm::env::get().floor;
    
    // static const unsigned int processor_count = std::thread::hardware_concurrency();
    // unsigned int begin; 
    // unsigned int end;
    // if constexpr (OPEN_WINDOW)
    // {
    //     begin  = 0; 
    //     end    = 1;
    // }
    // else
    // {
    //     begin  = (unsigned int)( predatorRobots.size() * ((float)threadId / (float)processor_count)); 
    //     end    = (unsigned int)( predatorRobots.size() * ((float)(threadId+1) / (float)processor_count));
    // }
    // // std::cout << threadId << " : " << begin << " - " << end << "\n";

    // for (unsigned int robotIndex = begin; robotIndex < end; robotIndex++)
    // {
    //     bool preyCaught = false;
    //     erppm::RobotBase& predatorRobot = *(predatorRobots[robotIndex]);
    //     erppm::RobotBase& preyRobot = *(preyRobots[robotIndex]);
    //     std::vector<erppm::RobotBase*> robotsInCurrentRun_predator = {&predatorRobot, &preyRobot};
    //     std::vector<erppm::RobotBase*> robotsInCurrentRun_prey = {&predatorRobot, &preyRobot};
    //     predatorRobot.network.clearValues();
    //     preyRobot.network.clearValues();

    //     double lastOutput_predator[2] = {0,0};
    //     double lastOutput_prey[2] = {0,0};
    //     unsigned int stepNumber = 0;
    //     for(; stepNumber< numberOfStepsPerGeneration; stepNumber++)
    //     {
    //         if constexpr (PREDATOR_LEARNING_MODE != 0)
    //         {
    //             predatorRobot.measure(robotsInCurrentRun_predator, walls, floor);
    //             predatorRobot.network.run(predatorRobot.getSensorData());
    //             predatorRobot.run(stepDuration, predatorRobot.network.getOutput());
    //         }
    //         if constexpr (PREY_LEARNING_MODE != 0)
    //         {
    //             preyRobot.measure(robotsInCurrentRun_prey, walls, floor);
    //             preyRobot.network.run(preyRobot.getSensorData());
    //             preyRobot.run(stepDuration, preyRobot.network.getOutput());
    //         }
    //         if (glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) < 2.0)
    //         {
    //             preyCaught = true;
    //             break;
    //         }
    //         for (std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun_predator.begin(); iter != robotsInCurrentRun_predator.end(); iter = std::next(iter))
    //         {
    //             (*iter)->collide(robotsInCurrentRun_predator, walls, floor);
    //             (*iter)->updateSensorsPosition();
    //         }
    //         for (std::vector<erppm::RobotBase*>::iterator iter = robotsInCurrentRun_prey.begin(); iter != robotsInCurrentRun_prey.end(); iter = std::next(iter))
    //         {
    //             (*iter)->collide(robotsInCurrentRun_prey, walls, floor);
    //             (*iter)->updateSensorsPosition();
    //         }
    //         if constexpr ( OPEN_WINDOW )
    //         {
    //             // std::cout << predatorRobot.network.getOutput().size() << " " << predatorRobot.network.getOutput()[0] << " " << predatorRobot.network.getOutput()[1] << "\r";
    //             erppm::win::run();
    //             if (!erppm::win::isAlive()) return;
    //         }
    //         if (stepNumber == 0)
    //         {
    //             if constexpr (PREDATOR_LEARNING_MODE != 0)
    //             {
    //                 if (preyPositionAngle_predator == glm::pi<float>() && predatorRobot.network.getOutput()[0] > predatorRobot.network.getOutput()[1])
    //                 {
    //                     preyPositionAngle_predator = -preyPositionAngle_predator;          
    //                 } 
    //             }
    //             if constexpr (PREY_LEARNING_MODE != 0)
    //             {
    //                 if (preyPositionAngle_prey == glm::pi<float>() && preyRobot.network.getOutput()[0] < preyRobot.network.getOutput()[1])
    //                 {
    //                     preyPositionAngle_prey = -preyPositionAngle_prey;          
    //                 } 
    //             }

    //         }
    //         if constexpr (TARGET_DISAPPEAR)
    //         {
    //             if (stepNumber == 1)
    //             {
    //                 lastOutput_predator[0] = predatorRobot.network.getOutput()[0];
    //                 lastOutput_predator[1] = predatorRobot.network.getOutput()[1];
    //                 lastOutput_prey[0] = preyRobot.network.getOutput()[0];
    //                 lastOutput_prey[1] = preyRobot.network.getOutput()[1];
    //             }
    //             else if (stepNumber == 2)
    //             {
    //                 robotsInCurrentRun_predator = {&predatorRobot};
    //                 robotsInCurrentRun_prey = {&preyRobot};
    //             }
    //         }
    //     }
    //     if constexpr (PREDATOR_LEARNING_MODE != 0 && !OPEN_WINDOW)
    //     {
    //         double runError = 0;
    //         if constexpr (!TARGET_DISAPPEAR)
    //         {
    //             runError += stepNumber;
    //             runError += (preyCaught ? 0.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) /*/ preyPositionRadius*/);
    //             // runError += glm::pi<float>() - abs(glm::mod(abs(predatorRobot.getRotation()[2] - preyPositionAngle_predator), glm::two_pi<float>()) - glm::pi<float>());
    //             runError += abs(predatorRobot.getRotation()[2] - preyPositionAngle_predator);
    //         }
    //         else
    //         {
    //             // runError += abs(lastOutput_predator[0] - predatorRobot.network.getOutput()[0]) + abs(lastOutput_predator[1] - predatorRobot.network.getOutput()[1]);
    //             runError += glm::pi<float>() - abs(glm::mod(abs(predatorRobot.getRotation()[2] - preyPositionAngle_predator), glm::two_pi<float>()) - glm::pi<float>());
    //             // runError += abs(predatorRobot.getRotation()[2] - preyPositionAngle_predator);
    //         }
    //         predatorRobot.network.score += runError;
    //     }
    //     if constexpr (PREY_LEARNING_MODE != 0 && !OPEN_WINDOW)
    //     {
    //         double runError = 0;
    //         if constexpr (!TARGET_DISAPPEAR)
    //         {
    //             // runError += stepNumber;
    //             runError += (preyCaught ? -1.0 : glm::length(predatorRobot.getPosition() - preyRobot.getPosition()) /*/ preyPositionRadius*/);
    //             // runError += glm::pi<float>() - abs(glm::mod(abs(preyRobot.getRotation()[2] - preyPositionAngle_prey), glm::two_pi<float>()) - glm::pi<float>());
    //             // runError += abs(preyRobot.getRotation()[2] - preyPositionAngle_prey);
    //         }
    //         else
    //         {
    //             runError += abs(lastOutput_prey[0] - preyRobot.network.getOutput()[0]) + abs(lastOutput_prey[1] - preyRobot.network.getOutput()[1]);
    //             runError += abs(glm::mod(abs(preyRobot.getRotation()[2] - preyPositionAngle_prey), glm::two_pi<float>()) - glm::pi<float>());
    //             // runError += abs(preyRobot.getRotation()[2] - preyPositionAngle_prey);
    //         }
    //         preyRobot.network.score += runError;
    //     }
    // }
}


template <
    bool OPEN_WINDOW,
    int PREDATOR_LEARNING_MODE,
    int PREY_LEARNING_MODE,
    bool PLACE_ON_LASER_SENORS,
    bool TARGET_DISAPPEAR
>
void erppm::sim::selectThreads(float preyPositionAngle)
{
    if constexpr ( OPEN_WINDOW )
    {
        erppm::sim::runRobot<
            OPEN_WINDOW,
            PREDATOR_LEARNING_MODE,
            PREY_LEARNING_MODE,
            PLACE_ON_LASER_SENORS,
            TARGET_DISAPPEAR
        >(0, preyPositionAngle);
    }
    else if constexpr (PREDATOR_LEARNING_MODE >= 2 && PREDATOR_LEARNING_MODE >= 2)
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
                    1,
                    PLACE_ON_LASER_SENORS,
                    TARGET_DISAPPEAR
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
        // futures.clear();
        std::vector<std::future<void>> futures2(processor_count);
        for( int threadId = 0; threadId < futures2.size(); threadId++ )
        {
            futures2[threadId] = std::async
            (
                std::launch::async,
                &erppm::sim::runRobot<
                    OPEN_WINDOW,
                    1,
                    PREY_LEARNING_MODE,
                    PLACE_ON_LASER_SENORS,
                    TARGET_DISAPPEAR
                >,
                this,
                threadId,
                preyPositionAngle
            );

        }
        for( auto& f : futures2 )
        {
            f.get();
        }
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
                    PLACE_ON_LASER_SENORS,
                    TARGET_DISAPPEAR
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



template <
    bool OPEN_WINDOW,
    int PREDATOR_LEARNING_MODE,
    int PREY_LEARNING_MODE,
    bool PLACE_ON_LASER_SENORS,
    bool TARGET_DISAPPEAR
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
        for (unsigned int repetition = 0; repetition < numberOfRepetitions; repetition++) // need to prepare a
        { 
            float preyPositionAngle = 0;
            if (randomEnv)
            {
                erppm::env::get().useScenario_random();
                erppm::env::get().setRobotsRandomConcentricSame(predatorRobots);
                erppm::env::get().setRobotsRandomConcentricSame(preyRobots);
            }
            else
            {
                if constexpr (OPEN_WINDOW){
                    if (!erppm::win::isAlive()) return;
                }
                // Prepare generation
                preyPositionAngle = 0;
                erppm::env::get().setRobotsCenterSame(predatorRobots);
                if constexpr (PLACE_ON_LASER_SENORS)
                {
                    preyPositionAngle = erppm::env::get().setRobotsOnLaserSame(preyRobots, predatorRobots[0], sensorsCount);
                }
                else
                {
                    erppm::env::get().setRobotsRandomConcentricSame(preyRobots);
                }
            }
            erppm::sim::selectThreads<
                OPEN_WINDOW,
                PREDATOR_LEARNING_MODE,
                PREY_LEARNING_MODE,
                PLACE_ON_LASER_SENORS,
                TARGET_DISAPPEAR
            >(preyPositionAngle);
            
        }
        if constexpr (!OPEN_WINDOW)
        {
            if constexpr (PREDATOR_LEARNING_MODE >= 2)
            {
                erppm::env::get().robots.sortBaseIncremental(erppm::EObjectTypePredator);
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
            else if constexpr (PREDATOR_LEARNING_MODE == 3)
            {
                unsigned int eliminated_index = erppm::cfg::numberOfSurvivingRobots;
                for(int i = 0; i < erppm::cfg::numberOfSurvivingRobots; i++)
                {
                    for(int j = i+1; j < erppm::cfg::numberOfSurvivingRobots; j++)
                    {
                        predatorRobots[eliminated_index]->evolveFrom(*(predatorRobots[i]),*(predatorRobots[j]));
                        eliminated_index++;
                    }
                }
            }

            if constexpr (PREY_LEARNING_MODE >= 2)
            {
                erppm::env::get().robots.sortBaseDecremental(erppm::EObjectTypePrey); // TODO reverse?
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
                unsigned int eliminated_index = erppm::cfg::numberOfSurvivingRobots;
                for(int i = 0; i < erppm::cfg::numberOfSurvivingRobots; i++)
                {
                    for(int j = i+1; j < erppm::cfg::numberOfSurvivingRobots; j++)
                    {
                        preyRobots[eliminated_index]->network.evolveFrom((preyRobots[i]->network),(preyRobots[j]->network));
                        eliminated_index++;
                    }
                }
            }
            else if(PREY_LEARNING_MODE == 3)
            {
                unsigned int eliminated_index = erppm::cfg::numberOfSurvivingRobots;
                for(int i = 0; i < erppm::cfg::numberOfSurvivingRobots; i++)
                {
                    for(int j = i+1; j < erppm::cfg::numberOfSurvivingRobots; j++)
                    {
                        preyRobots[eliminated_index]->evolveFrom(*(preyRobots[i]),*(preyRobots[j]));
                        eliminated_index++;
                    }
                }
            }
        }
    }
}

void erppm::sim::loopTemplateSwitching
(
    bool OPEN_WINDOW,
    int PREDATOR_LEARNING_MODE,
    int PREY_LEARNING_MODE,
    int PLACE_ON_LASER_SENORS,
    bool TARGET_DISAPPEAR
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
        else if(PREDATOR_LEARNING_MODE == 3) {SELECT_PREY_LM(__VA_ARGS__, 3)} \
        else { throw; }

    #define SELECT_PREY_LM(...) \
        if     (PREY_LEARNING_MODE == 0) {SELECT_PLACE_LASER(__VA_ARGS__, 0)} \
        else if(PREY_LEARNING_MODE == 1) {SELECT_PLACE_LASER(__VA_ARGS__, 1)} \
        else if(PREY_LEARNING_MODE == 2) {SELECT_PLACE_LASER(__VA_ARGS__, 2)} \
        else if(PREY_LEARNING_MODE == 3) {SELECT_PLACE_LASER(__VA_ARGS__, 3)} \
        else { throw; }

    #define SELECT_PLACE_LASER(...) \
        if     (PLACE_ON_LASER_SENORS ==-1) {SELECT_TARGET_DISAPPEAR(__VA_ARGS__, true)} \
        else if(PLACE_ON_LASER_SENORS == 0) {SELECT_TARGET_DISAPPEAR(__VA_ARGS__, false)} \
        else                                {SELECT_TARGET_DISAPPEAR(__VA_ARGS__, true)}

    #define SELECT_TARGET_DISAPPEAR(...) \
        if     (TARGET_DISAPPEAR) {EXEC(__VA_ARGS__, true)} \
        else                      {EXEC(__VA_ARGS__, false)} \

    #define EXEC(v1, v2, v3, v4, v5) \
        erppm::sim::loop<v1, v2, v3, v4, v5>(PLACE_ON_LASER_SENORS);

    SELECT_WINDOW();
    
    #undef GEN_SELECT
    #undef SELECT_WINDOW
    #undef SELECT_PREDATOR_LM
    #undef SELECT_PREY_LM
    #undef SELECT_PLACE_LASER
    #undef EXEC
}


 