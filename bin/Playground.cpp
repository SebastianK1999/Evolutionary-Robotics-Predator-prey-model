// A C++ main function that measures its execution time

#include <iostream>

#include "Simulation/Plt.hpp"


int main()
{
    // std::cout << GetDateTime() << "\n";
    erppm::plt::get().evolutionData.push_back({{0,1,2,3},{1,1,2,3}});
    erppm::plt::get().evolutionData.push_back({{2,1,2,3},{3,1,2,3}});
    erppm::plt::get().dumpEvolutionToFile("test");

    erppm::plt::get().runData.push_back({{0,1,2,3,4,5,{}},{1,1,2,3,4,5,{}}});
    erppm::plt::get().runData.push_back({{2,1,2,3,4,5,{}},{3,1,2,3,4,5,{}}});
    erppm::plt::get().dumpRunToFile("test", "scene_1", 12);
    return 0;
}