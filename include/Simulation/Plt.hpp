#pragma once

#include <vector>

namespace  erppm
{
    struct PopulationPlotInfo
    {
        float steps;
        float rotation;
    };

    struct PlotInfo
    {
        erppm::PopulationPlotInfo preyInfo;
        erppm::PopulationPlotInfo predatorInfo;
    };

    class plt
    {
    public:
        static plt& get() { static erppm::plt singleton; return singleton; };
        std::vector<erppm::PlotInfo> data;

    protected:
    private:
        plt() = default;
    };
} // namespace  erppm
