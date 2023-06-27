#include <iostream>
#include <fstream>

#include "Simulation/Plt.hpp"
#include "Simulation/Sim.hpp"
#include "JimmyNeuron/Misc/Misc.hpp"

erppm::plt& erppm::plt::get()
{
    static erppm::plt singleton;
    return singleton;
};


void erppm::plt::dumpEvolutionToFile(const std::string& runName)
{
    std::filesystem::path fileDirectory = std::filesystem::path()/"plots"/"evolution"/runName/"";
    if(! std::filesystem::exists(fileDirectory))
    {
        std::filesystem::create_directories(fileDirectory);
    }
    std::cout << fileDirectory << "\n";

    std::string dateDime = Jimmy::Misc::GetDateTime(); 
    {
        std::ofstream ofs(fileDirectory/(dateDime+"_predator.txt"));
        for (const auto& evolutionInfo : evolutionData)
        {
            ofs << evolutionInfo.predatorInfo.steps
                << " " << evolutionInfo.predatorInfo.rotation
                << " " << evolutionInfo.predatorInfo.distance
                << " " << evolutionInfo.predatorInfo.normalizedScore
                << "\n"; 
        }
        ofs.close();
    }
    {
        std::ofstream ofs(fileDirectory/(dateDime+"_prey.txt"));
        for (const auto& evolutionInfo : evolutionData)
        {
            ofs << evolutionInfo.preyInfo.steps
                << " " << evolutionInfo.preyInfo.rotation
                << " " << evolutionInfo.preyInfo.distance
                << " " << evolutionInfo.preyInfo.normalizedScore
                << "\n"; 
        }
        ofs.close();
    }
    evolutionData.clear();
}

void erppm::plt::dumpRunToFile(const std::string& runName, const std::string& mapName, unsigned int repetitionNumber)
{
    std::filesystem::path fileDirectory = std::filesystem::path()/"plots"/"run"/runName/erppm::sim::get().runStartTime/"";
    if(! std::filesystem::exists(fileDirectory))
    {
        std::filesystem::create_directories(fileDirectory);
    }
    std::cout << fileDirectory << "\n";

    {
        std::ofstream ofs(fileDirectory/(std::to_string(repetitionNumber)+"_predator_("+ mapName +").txt"));
        for (const auto& runInfo : runData)
        {
            ofs << runInfo.predatorInfo.position_x 
                << " " << runInfo.predatorInfo.position_y
                << " " << runInfo.predatorInfo.rotation
                << " " << runInfo.predatorInfo.distance
                << " " << runInfo.predatorInfo.motor_0
                << " " << runInfo.predatorInfo.motor_1;
            for (auto sensor : runInfo.predatorInfo.sensors)
            {
                ofs << " " << sensor;
            }
            ofs << "\n"; 
        }
        ofs.close();
    }
    {
        std::ofstream ofs(fileDirectory/(std::to_string(repetitionNumber)+"_prey_("+ mapName +").txt"));
        for (const auto& runInfo : runData)
        {
            ofs << runInfo.preyInfo.position_x 
                << " " << runInfo.preyInfo.position_y
                << " " << runInfo.preyInfo.rotation
                << " " << runInfo.preyInfo.distance
                << " " << runInfo.preyInfo.motor_0
                << " " << runInfo.preyInfo.motor_1;
            for (auto sensor : runInfo.preyInfo.sensors)
            {
                ofs << " " << sensor;
            }
            ofs << "\n"; 
        }
        ofs.close();
    }
    runData.clear();
}
