#include "ProgramOptions.h"
int main(int argc, char** argv)
{
    // Get subcommand
    const std::string  subcmd = GetSubcommand(argc, argv);

    // Dispatch to subcommand executor
#define COMMAND_DEF(command, executor)                                                             \
    if (subcmd == #command)                                                                        \
    {                                                                                              \
        executor run;                                                                              \
        run.AddOptions(argc, argv);                                                                \
        run.ReadOptions();                                                                         \
        run.Execute();                                                                             \
        return 0;                                                                                  \
    }
    COMMAND_DEF_SEQ
#undef COMMAND_DEF

}