#pragma once

#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

#include "oglUtil/drawables/Mesh.hpp"
#include "Drawables/Wall.hpp"
#include "Drawables/Floor.hpp"

namespace erppm
{   
    class RobotBase;
    class SensorBase
    {
    public:
        friend class RobotBase;
        glm::vec3 positionAtRobot;
        glm::vec3 rotationAtRobot;
        std::vector<double> evolutionaryData;
    
        SensorBase(SensorBase&& other);
        SensorBase(const std::filesystem::path& modelPath, const glm::vec3& positionAtRobot, const glm::vec3& rotationAtRobot);
        virtual ~SensorBase();
        void updateLocation(glm::vec3 robotPosition, glm::vec3 robotRotation);
        virtual void measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor, std::vector<double>::iterator& measurementDataIterator) = 0;
        glm::vec3& getPosition();
        glm::vec3& getRotation();
        glm::vec3& getScale();
        glm::vec4& getPrimaryColor();
        virtual const size_t getMeasurementDataSize() const noexcept;
        virtual void reinitializeDrawables() = 0;
        virtual void updateDecorator() = 0;
        virtual void updateDecoratorColors(const glm::vec4& color) = 0;

    protected:
        oglu::Mesh body;
        RobotBase* parent;

    };
}
