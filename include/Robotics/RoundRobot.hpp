#pragma once

#include "Robotics/RobotBase.hpp"

namespace erppm
{
    class RoundRobot : public virtual RobotBase
    {
    public:
        static const std::filesystem::path modelPath;
        static void draw(const glm::mat4& MVP, const glm::vec3& light);

        RoundRobot();
        virtual ~RoundRobot();
        void run(double time, const std::vector<double>& controlInput) override;
        const size_t getControlInputSize() const noexcept override;
    private:   
        static std::shared_ptr<oglu::RegisteredDrawable> meshRegistry;
        
    };   
}