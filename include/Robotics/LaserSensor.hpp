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
        static const std::filesystem::path modelPath;
        static constexpr size_t SIZE_OF_EVOLUTIONARY_DATA = 2;
        static void draw(const glm::mat4& MVP, const glm::vec3& light);
        LaserBeam laserBeam;
        LaserSensor(const glm::vec3& positionAtRobot, const glm::vec3& rotationAtRobot, const std::array<double, SIZE_OF_EVOLUTIONARY_DATA>& evolutionaryData);
        LaserSensor(LaserSensor&&other);
        virtual ~LaserSensor();
        void measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor, std::vector<double>::iterator& measurementDataIterator) override;
        const size_t getMeasurementDataSize() const noexcept override;
        void reinitializeDrawables() override;
        void updateDecorator() override;
        void updateDecoratorColors(const glm::vec4& color) override;
    };   
}