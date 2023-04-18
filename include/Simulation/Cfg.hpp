#pragma once

#include <stddef.h>

#define ERPPM_CFG_ALLOW_VISUALS 1

namespace erppm
{
    namespace cfg
    {
        // TODO to functions and static members nonconst
        constexpr size_t numberOfSurvivingRobots = 15;
        constexpr size_t numberOfEliminatedRobots =  (numberOfSurvivingRobots)*(numberOfSurvivingRobots-1)/2;
        constexpr size_t numberOfRobots = numberOfSurvivingRobots + numberOfEliminatedRobots;
        constexpr size_t numberOfSensorsPerSide = 4;
        constexpr size_t numberOfSensors = numberOfSensorsPerSide * 2 + 1;
    }
}
