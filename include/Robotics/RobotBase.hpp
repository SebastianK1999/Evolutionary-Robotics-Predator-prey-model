#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <filesystem>

#include "oglUtil/drawables/Mesh.hpp"
#include "Robotics/SensorBase.hpp"
#include "Robotics/ObjectTypes.hpp"
#include "Drawables/Floor.hpp"
#include "Drawables/Wall.hpp"
#include "JimmyNeuron/Jimmy.hpp"
#include "JimmyNeuron/Experimental/LameNetwork/LameNetwork.hpp"


namespace erppm
{
    class RobotBase
    {
    public:
        oglu::Mesh body;
        // Jimmy::LoopingNet network;
        Jimmy::E::LameNetwork network;
        glm::dvec2 velocity;
        double angularVelocity;
        EObjectType type;
        std::vector<SensorBase*> sensors;

        float scoreTime; 
        float scoreRotation; 
        float scoreDistance; 

        RobotBase(const std::filesystem::path& modePlath);
        virtual ~RobotBase();
        virtual void run(double time, const std::vector<double>& controlInput) = 0;
        void updateSensorsPosition();
        void collide(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor);
        void measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor);
        const std::vector<double>& getSensorData() const;
        template <typename T>
        std::list<T>& getSensorList();
        template <typename T>
        void addSensor(T&& sensor);
        template <typename T>
        void popSensors(int n = 1);
        glm::vec3& getPosition() const noexcept;
        glm::vec3& getRotation() const noexcept;
        glm::vec3& getScale() const noexcept;
        glm::vec4& getPrimaryColor() const noexcept;
        virtual const size_t getControlInputSize() const noexcept;
        const size_t getSensorDataSize() const noexcept;
        void reinitializeDrawables();
        void clearVelocity();
        void evolveFrom(const RobotBase& maleRobot, const RobotBase& femaleRobot);

    protected:
        std::vector<double> sensorData;
    };
}

#include "Robotics/RobotBase.inl"