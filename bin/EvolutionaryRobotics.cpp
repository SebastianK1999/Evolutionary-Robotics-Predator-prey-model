#include <iostream>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "oglUtil_OLD/Misc.hpp"

#include "oglUtil/ogluTools.hpp"

#include "JimmyNeuron/Misc/Misc.hpp"

#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "Robotics/RoundRobot.hpp"
#include "Robotics/LaserSensor.hpp"
#include "Simulation/Env.hpp"
#include "Simulation/Cmd.hpp"
#include "Simulation/Cfg.hpp"
#include "Simulation/Win.hpp"

int main(int argc, char *argv[])
{
    oglu::setupOglu();
    oglu::setupGlfw();
    // oglu::DrawableBase::limitDrawables = 100;
    if constexpr (ERPPM_CFG_ALLOW_VISUALS)
    {
        oglu::Window wInitGlew(800, 800, "Init GLEW");
        erppm::env::get().useScenario_empty();
    }
    if(argc == 2)
    {
        erppm::cmd::executeCommand({"e", argv[1]});
    }
    erppm::cmd::cmdLoop();
    _exit(0);
    return 0;
}