#include <sstream>
#include <iterator>
#include <filesystem>

#include "oglUtil/drawableSystem/DrawableRegistry.hpp"

#include "Simulation/Win.hpp"
#include "Simulation/Env.hpp"
#include "Simulation/Sim.hpp"

#include "Simulation/Cmd.hpp"

bool erppm::cmd::isRunningFlag = true;

void erppm::cmd::cmdLoop()
{
    while(erppm::cmd::isRunningFlag)
    {
        const erppm::Command command = erppm::cmd::getCommand();
        erppm::cmd::executeCommand(command);
    }
}

const bool erppm::cmd::isRunning()
{
    return isRunningFlag;
}

const erppm::Command erppm::cmd::getCommand()
{
    std::string commandLine;
    std::cout << " <<: ";
    std::getline(std::cin, commandLine);
    return lineToCommand(commandLine);
}

void erppm::cmd::executeCommand(const erppm::Command& command)
{
    if (command.size() == 0)
    {
        return;
    }
    else if (command[0] == "ping")
    {
        ping_cmd(command);
    }
    else if (command[0] == "echo")
    {
        echo_cmd(command);
    }
    else if (command[0] == "run" || command[0] == "r")
    {
        run_cmd(command);
    }
    else if (command[0] == "exe" || command[0] == "e")
    {
        exe_cmd(command);
    }
    else if (command[0] == "stage" || command[0] == "s")
    {
        stage_cmd(command);
    }
    else if (command[0] == "view" || command[0] == "v")
    {
        view_cmd(command);
    }
    else if (command[0] == "save")
    {
        saveModel_cmd(command);
    }
    else if (command[0] == "load")
    {
        loadModel_cmd(command);
    }
    else if (command[0] == "reset")
    {
        resetModel_cmd(command);
    }
    else if (command[0] == "quit" || command[0] == "q" || command[0] == "exit")
    {
        exit_cmd(command);
    }
    else if (command[0] == "set")
    {
        setVar_cmd(command);
    }
    else if (command[0] == "help")
    {
        help_cmd(command);
    }
    else
    {
        undefined_cmd(command);
    }
}

void erppm::cmd::ping_cmd(const erppm::Command& command)
{
    std::cout << "pong!\n";
}

void erppm::cmd::echo_cmd(const erppm::Command& command)
{
    for(unsigned int index = 1; index < command.size(); index++)
    {
        std::cout << command[index] << " ";
    }
    std::cout << "\n";
}


void erppm::cmd::run_cmd(const erppm::Command& command)
{
    if (command.size() != 9){
        wrongParms_cmd(command);
        return;
    }
    bool openWindow = (atoi(command[1].c_str()) == 1);
    if(openWindow)
    {
        erppm::win::open();
    }
    erppm::sim::get().numberOfGenerations = atoi(command[2].c_str());
    if (command[3][command[3].size()-1] == 's') 
    {
        std::string amount = command[3].substr(0, command[3][command[3].size()-1]);
        float seconds = atof(amount.c_str());
        erppm::sim::get().numberOfStepsPerGeneration = (unsigned int) seconds/erppm::sim::get().stepDuration;
    }
    else
    {
        erppm::sim::get().numberOfStepsPerGeneration = atoi(command[3].c_str());
    }
    if (erppm::sim::get().stepDuration == 0)
    {
        erppm::sim::get().stepDuration = 0.01;
    }
    erppm::env::get().useScenario_empty();
    oglu::DrawableRegistry::reinitialize();
    erppm::sim::get().numberOfRepetitions = 25;
    erppm::sim::get().randomEnv = (atoi(command[8].c_str()) != 0); 
    erppm::sim::get().loopTemplateSwitching
    (
        openWindow,
        atoi(command[4].c_str()),
        atoi(command[5].c_str()),
        atoi(command[6].c_str()),
        command[7] == "1"
    );
    erppm::win::destroy();
}

void erppm::cmd::exe_cmd(const erppm::Command& command)
{
    if (command.size() != 2)
    {
        wrongParms_cmd(command);
        return;
    }
    std::filesystem::path filePath = std::filesystem::path()/"res"/"exe"/command[1];
    std::ifstream file(filePath);
    if (file.is_open()){
        std::string line;
        while (std::getline(file, line))
        {
            executeCommand(lineToCommand(line));
        }
    }
    else
    {
        std::cout << "Can't open the \"" << filePath << "\" file.\n";
    }
    file.close();
}

void erppm::cmd::stage_cmd(const erppm::Command& command)
{
    if (command.size() != 4){
        wrongParms_cmd(command);
        return;
    }
    // PREDATOR 
    if (command[1] == "0")
    {
        run_cmd({"run", "0", command[2], command[3], "2", "0", "-1", "1", "0"});
    }
    else if (command[1] == "1")
    {
        run_cmd({"run", "0", command[2], command[3], "2", "0", "-1", "0", "0"});
    }
    else if (command[1] == "2")
    {
        run_cmd({"run", "0", command[2], command[3], "2", "0", "0", "0", "0"});
    }
    // PREY
    else if (command[1] == "3")
    {
        run_cmd({"run", "0", command[2], command[3], "0", "2", "-1", "1", "0"});
    }
    else if (command[1] == "4")
    {
        run_cmd({"run", "0", command[2], command[3], "0", "2", "-1", "0", "0"});
    }
    else if (command[1] == "5")
    {
        run_cmd({"run", "0", command[2], command[3], "0", "2", "0", "0", "0"});
    }
    else if (command[1] == "6")
    {
        run_cmd({"run", "0", command[2], command[3], "1", "2", "0", "0", "0"});
    }
    // ALL
    else if (command[1] == "7")
    {
        run_cmd({"run", "0", command[2], command[3], "2", "2", "0", "0", "0"});
    }
    else if (command[1] == "8")
    {
        run_cmd({"run", "0", command[2], command[3], "2", "2", "0", "0", "1"});
    }

}

void erppm::cmd::view_cmd(const erppm::Command& command)
{
    if (command.size() != 3){
        wrongParms_cmd(command);
        return;
    }
    // PREDATOR 
    if (command[1] == "0")
    {
        run_cmd({"run", "1", "1", command[2], "2", "0", "-1", "1", "0"});
    }
    else if (command[1] == "1")
    {
        run_cmd({"run", "1", "1", command[2], "2", "0", "-1", "0", "0"});
    }
    else if (command[1] == "2")
    {
        run_cmd({"run", "1", "1", command[2], "2", "0", "0", "0", "0"});
    }
    // PREY
    else if (command[1] == "3")
    {
        run_cmd({"run", "1", "1", command[2], "0", "2", "-1", "1", "0"});
    }
    else if (command[1] == "4")
    {
        run_cmd({"run", "1", "1", command[2], "0", "2", "-1", "0", "0"});
    }
    else if (command[1] == "5")
    {
        run_cmd({"run", "1", "1", command[2], "0", "2", "0", "0", "0"});
    }
    else if (command[1] == "6")
    {
        run_cmd({"run", "1", "1", command[2], "1", "2", "0", "0", "0"});
    }
    // ALL
    else if (command[1] == "7")
    {
        run_cmd({"run", "1", "1", command[2], "2", "2", "0", "0", "0"});
    }
    else if (command[1] == "8")
    {
        run_cmd({"run", "1", "1", command[2], "2", "2", "0", "0", "1"});
    }
}

void erppm::cmd::saveModel_cmd(const erppm::Command& command)
{
    bool yes = (command.size() == 3 && command[2] == "yes");
    if (command.size() != 2 && !yes)
    {
        wrongParms_cmd(command);
        return;
    }
    std::string fileName = command[1];
    std::filesystem::path fileDirectory = std::filesystem::path()/"saves"/fileName/"";
    if(! std::filesystem::exists(fileDirectory))
    {
        std::cout << fileDirectory << "\n";
        std::filesystem::create_directories(fileDirectory);
    }
    else if (!yes)
    {    
        erppm::Command confirm = {};
        while(confirm.size() != 1 || !(confirm[0] == "y" || confirm[0] == "yes"))
        {
            std::cout << "You're about to override existing files. Are you sure, you want to continue? (y/n):\n";
            confirm = erppm::cmd::getCommand();
            if(confirm.size() == 1 && (confirm[0] == "n" || confirm[0] == "no"))
            {
                std::cout << "Save canceled\n";
                return;
            }
        }
        std::cout << "Saving!\n";
    }    
    const std::vector<erppm::RobotBase*>& predatorRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePredator);
    const std::vector<erppm::RobotBase*>& preyRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePrey);  
    for(int i = 0; i < predatorRobots.size(); i++)
    {
        predatorRobots[i]->network.saveToFile(fileDirectory/("predator_"+std::to_string(i)+".jn"));
    } 
    for(int i = 0; i < preyRobots.size(); i++)
    {
        preyRobots[i]->network.saveToFile(fileDirectory/("prey_"+std::to_string(i)+".jn"));
    } 
}

void erppm::cmd::loadModel_cmd(const erppm::Command& command)
{
    if (command.size() != 2)
    {
        wrongParms_cmd(command);
        return;
    }
    std::string fileName = command[1];
    std::filesystem::path fileDirectory = std::filesystem::path()/"saves"/fileName/"";
    if(! std::filesystem::exists("saves/" + fileName + "/"))
    {
        std::cout << "Not found!\n";
        return;
    }
    const std::vector<erppm::RobotBase*>& predatorRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePredator);
    const std::vector<erppm::RobotBase*>& preyRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePrey);
    for(int i = 0; i < predatorRobots.size(); i++)
    {
        predatorRobots[i]->network.loadFromFile(fileDirectory/("predator_"+std::to_string(i)+".jn"));
    } 
    for(int i = 0; i < preyRobots.size(); i++)
    {
        preyRobots[i]->network.loadFromFile(fileDirectory/("prey_"+std::to_string(i)+".jn"));
    } 
}

void erppm::cmd::resetModel_cmd(const erppm::Command& command)
{
    if (command.size() != 1)
    {
        wrongParms_cmd(command);
        return;
    }
    const std::vector<erppm::RobotBase*>& predatorRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePredator);
    const std::vector<erppm::RobotBase*>& preyRobots = erppm::env::get().robots.getBase(erppm::EObjectTypePrey);
    for(int i = 0; i < predatorRobots.size(); i++)
    {
        predatorRobots[i]->network.randomize(-0.01, 0.01);
    } 
    for(int i = 0; i < preyRobots.size(); i++)
    {
        preyRobots[i]->network.randomize(-0.01, 0.01);
    }
    std::cout << "Weights reset \n";
}

void erppm::cmd::exit_cmd(const erppm::Command& command)
{
    erppm::cmd::isRunningFlag = false;
}

void erppm::cmd::setVar_cmd(const erppm::Command& command)
{
    if (command.size() != 3)
    {
        wrongParms_cmd(command);
        return;
    }
    if (command[1] == "susc")
    {
        for (auto& robot : erppm::env::get().robots.getRound(erppm::EObjectTypePredator)) robot->network.susceptibilityToMutations = atof(command[2].c_str());
        for (auto& robot : erppm::env::get().robots.getRound(erppm::EObjectTypePrey)) robot->network.susceptibilityToMutations = atof(command[2].c_str());
    }
    else if (command[1] == "sdur")
    {
        erppm::sim::get().stepDuration = atof(command[2].c_str());
        std::cout << "sdur=" << erppm::sim::get().stepDuration <<  "\n";
    }
    else
    {
        std::cout << "Variable \"" << command[1] <<  "\" not found.\n";
        return;
    }
}

void erppm::cmd::help_cmd(const erppm::Command& command)
{
    std::cout <<
        "Available commands:\n" << 
        "\t" << "run [scenario] - Opens window with simulation\n" <<
        "\t" << "stage [n] - Trains neural network in the background\n" <<
        // "\t" << "train [stage] - Trains neural network in the background\n" <<
        "\t" << "exit/quit/ - Terminates program\n" <<
    "";
}

void erppm::cmd::undefined_cmd(const erppm::Command& command)
{
    std::cout << "Undefined command \"" << command[0] << "\"\n";
}

void erppm::cmd::wrongParms_cmd(const erppm::Command& command)
{
    std::cout << "Wrong \"" << command[0] <<  "\" command parameters : ";
    echo_cmd(command);
}

erppm::Command erppm::cmd::lineToCommand(const std::string& commandLine)
{
    erppm::Command command;
    std::istringstream iss(commandLine);
    std::string word;
    while(iss >> word)
    {
        if (word == "#")
        {
            return command;
        }
        command.push_back(word);
    }
    return command;
}