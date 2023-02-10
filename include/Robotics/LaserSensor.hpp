#pragma once

#include <array>

#include "Robotics/SensorBase.hpp"
#include "Robotics/RobotBase.hpp"
#include "Drawables/LaserBeam.hpp"
#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"

namespace erppm
{
    class LaserSensor : public virtual SensorBase
    {
    public:
        constexpr static size_t SIZE_OF_EVOLUTIONARY_DATA = 2;
        LaserBeam laserBeam;
        LaserSensor(const glm::vec4& positionAtRobot, const glm::vec4& rotationAtRobot, const std::array<double, SIZE_OF_EVOLUTIONARY_DATA>& evolutionaryData);
        LaserSensor(LaserSensor&&other);
        virtual ~LaserSensor();
        void measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor, std::vector<double>::iterator& measurementDataIterator) override;
        void draw(const glm::mat4& MVP, const glm::vec3& light) override;
        const size_t getMeasurementDataSize() const noexcept override;
        void reinitializeDrawables() override;
    };   
}