#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "oglUtil_OLD/Mesh.hpp"
#include "Drawables/Wall.hpp"
#include "Drawables/Floor.hpp"

namespace erppm
{   
    class RobotBase;
    class SensorBase
    {
    public:
        friend class RobotBase;
        glm::vec4 positionAtRobot;
        glm::vec4 rotationAtRobot;
        std::vector<double> evolutionaryData;
        
        SensorBase(SensorBase&& other);
        SensorBase(const std::string& modelPath, const glm::vec4& positionAtRobot, const glm::vec4& rotationAtRobot);
        virtual ~SensorBase();
        virtual void draw(const glm::mat4& MVP, const glm::vec3& light);
        void updateLocation(glm::vec4 robotPosition, glm::vec4 robotRotation);
        virtual void measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor, std::vector<double>::iterator& measurementDataIterator) = 0;
        glm::vec4& getPosition();
        glm::vec4& getRotation();
        glm::vec4& getScale();
        glm::vec3& getPrimaryColor();
        virtual const size_t getMeasurementDataSize() const noexcept;
        virtual void reinitializeDrawables() = 0;

    protected:
        Mesh body;
        RobotBase* parent;
    };
}
