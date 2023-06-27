#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <string>

#include "Robotics/RobotBase.hpp"
#include "Simulation/Cfg.hpp"
#include "Simulation/Env.hpp"
#include "Simulation/Win.hpp"
// TODO, move, collisions, measure, network control, evolution, template stage

namespace erppm
{
    enum EEvolutionMode
    {
        EEvolutionPass,
        EEvolutionRun,
        EEvolutionNetwork,
        EEvolutionFull
    };

    enum ETargetDisappearMode
    {
        ETargetDisappearPass,
        ETargetDisappearDetection,
        ETargetDisappearScore
    };

    class sim
    {
    public:
        static sim& get();
        std::string runName;
        std::string runStartTime;
        double stepDuration;
        unsigned int numberOfGenerations;
        unsigned int numberOfStepsPerGeneration;
        unsigned int numberOfRepetitions;
        bool recordRun;
        bool randomEnv;
        bool survival;
        
        template <
            bool OPEN_WINDOW,
            int PREDATOR_LEARNING_MODE,
            int PREY_LEARNING_MODE,
            bool PLACE_ON_LASER_SENORS,
            bool TARGET_DISAPPEAR
        >
        inline void runRobot
        (
            unsigned int threadId,
            float preyPositionAngle
        );

        template <
            bool OPEN_WINDOW,
            int PREDATOR_LEARNING_MODE,
            int PREY_LEARNING_MODE,
            bool PLACE_ON_LASER_SENORS,
            bool TARGET_DISAPPEAR
        >
        void selectThreads
        (
            float preyPositionAngle
        );

        template <
            bool OPEN_WINDOW,
            int PREDATOR_LEARNING_MODE,
            int PREY_LEARNING_MODE,
            bool PLACE_ON_LASER_SENORS,
            bool TARGET_DISAPPEAR
        >
        void loop
        (
            unsigned int sensorsCount
        );

        void loopTemplateSwitching
        (
            bool OPEN_WINDOW,
            int PREDATOR_LEARNING_MODE,
            int PREY_LEARNING_MODE,
            int PLACE_ON_LASER_SENORS,
            bool TARGET_DISAPPEAR
        );

    protected:
    private:
        sim() = default;
    };
}

#include "Simulation/Sim.inl"