#include "Simulation/Sim.hpp"

erppm::sim& erppm::sim::get()
{
    static erppm::sim singleton;
    return singleton;
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