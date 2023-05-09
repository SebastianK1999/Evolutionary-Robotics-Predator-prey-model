#pragma once 

#include <vector>
#include <string>

namespace erppm
{
    typedef std::vector<std::string> Command;

    class cmd
    {
    public:
        static void cmdLoop();
        static const bool isRunning();
        static const erppm::Command getCommand();
        static void executeCommand(const erppm::Command& command);

    private:
        static bool isRunningFlag;
        static void ping_cmd(const erppm::Command& command);
        static void echo_cmd(const erppm::Command& command);
        static void run_cmd(const erppm::Command& command);
        static void exe_cmd(const erppm::Command& command);
        static void view_cmd(const erppm::Command& command);
        static void stage_cmd(const erppm::Command& command);
        static void saveModel_cmd(const erppm::Command& command);
        static void loadModel_cmd(const erppm::Command& command);
        static void resetModel_cmd(const erppm::Command& command);
        static void exit_cmd(const erppm::Command& command);
        static void setVar_cmd(const erppm::Command& command);
        static void help_cmd(const erppm::Command& command);
        static void undefined_cmd(const erppm::Command& command);
        static void wrongParms_cmd(const erppm::Command& command);

        static erppm::Command lineToCommand(const std::string& commandLine);

        cmd() = delete;
    };
}