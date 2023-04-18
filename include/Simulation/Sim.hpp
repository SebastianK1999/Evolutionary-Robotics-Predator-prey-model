#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <future>

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
        double stepDuration;
        unsigned int numberOfGenerations;
        unsigned int numberOfStepsPerGeneration;
        
        template <
            bool OPEN_WINDOW,
            int PREDATOR_LEARNING_MODE,
            int PREY_LEARNING_MODE,
            bool PLACE_ON_LASER_SENORS
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
            bool PLACE_ON_LASER_SENORS
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
            int PLACE_ON_LASER_SENORS
        );

    protected:
    private:
        sim() = default;
    };
}

#include "Simulation/Sim.inl"