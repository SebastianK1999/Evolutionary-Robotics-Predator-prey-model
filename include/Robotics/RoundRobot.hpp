#pragma once

#include "Robotics/RobotBase.hpp"

namespace erppm
{
    class RoundRobot : public virtual RobotBase
    {
    public:
        RoundRobot();
        virtual ~RoundRobot();
        void run(double time, const std::vector<double>& controlInput) override;
        const size_t getControlInputSize() const noexcept override;
    };   
}