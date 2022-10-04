#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <list>

#include "oglUtil_OLD/Mesh.hpp"
#include "Robotics/SensorBase.hpp"
#include "Robotics/ObjectTypes.hpp"
#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"

namespace erppm
{
    class RobotBase
    {
    public:
        Mesh body;
        glm::dvec2 velocity;
        double angularVelocity;
        EObjectType type;
        RobotBase(const std::string& modePlath);
        virtual ~RobotBase();
        void draw(const glm::mat4& MVP, const glm::vec3& light) const;
        virtual void run(double time, const std::vector<double>& controlInput) = 0;
        void updateSensorsPosition();
        void collide(std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor);
        void measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor);
        const std::vector<double>& getSensorData() const;
        template <typename T>
        std::list<T>& getSensorList();
        template <typename T>
        void addSensor(T&& sensor);
        glm::vec4& getPosition();
        glm::vec4& getRotation();
        glm::vec4& getScale();
        glm::vec3& getPrimaryColor();
        virtual const size_t getControlInputSize() const noexcept;
        const size_t getSensorDataSize() const noexcept;
        void reinitializeDrawables();
        void clearVelocity();

    protected:
        std::vector<double> sensorData;
        std::vector<SensorBase*> sensors;
    };
}

#include "Robotics/RobotBase.inl"