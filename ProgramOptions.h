#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <numeric>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// Defined subcommand and executor
#define COMMAND_DEF_SEQ                                                                            \
    COMMAND_DEF(Example, ExampleExecutor)

std::string GetSubcommand(int argc, char** argv)
{
    // Supported commands
    std::string supported = "Supported commands:  ";
#define COMMAND_DEF(command, executor) supported += (#command); supported += "  ";
    COMMAND_DEF_SEQ
#undef COMMAND_DEF

    // Positional options for subcommand
    po::options_description visiable("Supported Commands");
    visiable.add_options()("help", "Help message");
    visiable.add_options()("command", po::value<std::string>(), supported.c_str());
    po::options_description hide("Hide");
    hide.add_options()("placeholder", po::value<std::vector<std::string>>(), "Placeholder.");
    po::options_description all("All");
    all.add(visiable).add(hide);

    po::positional_options_description position;
    position.add("command", 1);
    position.add("placeholder", -1);
    po::variables_map vm;

    std::string subcmd;
    try
    {
        po::parsed_options parsed = po::command_line_parser(argc, argv)
            .options(all)
            .positional(position)
            .allow_unregistered()
            .run();

        po::store(parsed, vm);

        if (vm["command"].empty())
        {
            std::cout << visiable << std::endl;
            exit(0);
        }
        subcmd = vm["command"].as<std::string>();
    }
    catch (boost::program_options::error& e)
    {
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl << std::endl;
        std::cout << visiable << std::endl;
        exit(-1);
    }

#define COMMAND_DEF(type, command)                                                                 \
    if (subcmd == #type)                                                                           \
    {                                                                                              \
        return subcmd;                                                                             \
    }
    COMMAND_DEF_SEQ
#undef COMMAND_DEF
    std::cerr << std::endl << "[ERROR] Unsupported Command: " << subcmd << std::endl << std::endl;
    std::cout << visiable << std::endl;
    exit(-1);

    // To disalbe warning C4715: not all control paths return a value
    return "";
}


class IExecutor
{
public:
    // Add boost::program_options here
    virtual void AddOptions(int argc, char** argv) = 0;

    // Read parsed options from variables_map to privaite variables
    virtual void ReadOptions() = 0;

    // Actual bussiness logci
    virtual void Execute() = 0;

protected:
    void CreateOptionDesc(const std::string desc)
    {
        m_desc = std::make_unique<po::options_description>(desc);
    }

    po::options_description& GetOptionsDesc()
    {
        return *m_desc;
    }

    po::variables_map& GetVariablesMap()
    {
        return m_vm;
    }

protected:
    std::unique_ptr<po::options_description> m_desc;
    po::variables_map m_vm;
};


#define HELP_AND_VALIDATE                                                                          \
    try                                                                                            \
    {                                                                                              \
        po::store(po::parse_command_line(argc, argv, *m_desc.get()), m_vm);                        \
        if (m_vm.count("help"))                                                                    \
        {                                                                                          \
            std::cout << *m_desc.get() << std::endl;                                               \
            exit(0);                                                                               \
        }                                                                                          \
        po::notify(m_vm);                                                                          \
    }                                                                                              \
    catch (boost::program_options::error & e)                                                      \
    {                                                                                              \
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl << std::endl;                 \
        std::cout << *m_desc.get() << std::endl;                                                   \
        exit(-1);                                                                                  \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////                      Implement Command Details Below                                        ///
/////////////////////////////////////////////////////////////////////////////////////////////////////



class ExampleExecutor :public IExecutor
{
public:
    void AddOptions(int argc, char** argv) override
    {
        CreateOptionDesc("Valid options for *Example*");
        GetOptionsDesc().add_options()
            ("help", "Help message")
            ("RequiredInt", po::value<std::uint32_t>()->required(), "Example of RequiredInt")
            ("RequiredIntWithDefault", po::value<std::uint32_t>()->default_value(1), "Example of RequiredIntWithDefault")
            ("Multiple", po::value<std::vector<std::string>>()->multitoken()->composing()->required(), "Example of Multiple inputs")
            ("example", po::value<std::string>()->required(), "Example description.");

        // Help and Validation
        HELP_AND_VALIDATE;
    }

    void ReadOptions() override
    {
        const auto & map = GetVariablesMap();
        m_example = map["example"].as<std::string>();
        m_int = map["RequiredInt"].as<std::uint32_t>();
        m_multiples = map["Multiple"].as<std::vector<std::string>>();
    }

    void Execute() override
    {
        std::cout << "[INFO] " << m_example << std::endl;
    }

private:
    std::string m_example;
    std::uint32_t m_int;
    std::vector<std::string> m_multiples;
};

#undef HELP_AND_VALIDATE