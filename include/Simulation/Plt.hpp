#pragma once

#include <vector>
#include <filesystem>

namespace  erppm
{
    struct RunPlotInfo
    {
        float position_x;
        float position_y;
        float rotation;
        float distance;
        double motor_0;
        double motor_1;
        std::vector<double> sensors;
    };

    struct EvolutionPlotInfo
    {
        float steps;
        float rotation;
        float distance;
        double normalizedScore;
    };

    template <typename T>
    struct PopulationsPlotInfo
    {
        T predatorInfo;
        T preyInfo;
    };

    class plt
    {
    public:
        static plt& get();
        void dumpEvolutionToFile(const std::string& runName);
        void dumpRunToFile(const std::string& runName, const std::string& mapName, unsigned int repetitionNumber);

        std::vector<erppm::PopulationsPlotInfo<erppm::EvolutionPlotInfo>> evolutionData;
        std::vector<erppm::PopulationsPlotInfo<erppm::RunPlotInfo>> runData;
        

    protected:
    private:
        plt() = default;
    };
} // namespace  erppm
