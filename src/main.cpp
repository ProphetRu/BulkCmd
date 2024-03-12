#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using StringList = std::vector<std::string>;

/**
 * @brief Process commands read from standard input and handle them in blocks.
 * 
 * @param block_size The size of the block for processing commands.
 */
void ProcessCommands(unsigned block_size) noexcept
{
    /**
     * @brief Get the current timestamp as a string.
     * 
     * @return The current timestamp as a string.
     */
    auto get_current_time_stamp = []() noexcept
    {
        return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    };

    /**
     * @brief Log the commands to a file with the given timestamp.
     * 
     * @param time_stamp The timestamp for the log file name.
     * @param cmds The list of commands to log.
     * @return True if logging was successful, false otherwise.
     */
    auto log = [](const std::string& time_stamp, const StringList& cmds) noexcept
    {
        if (!time_stamp.empty() && !cmds.empty())
        {
            if (std::ofstream log_file("bulk" + time_stamp + ".log"); log_file.is_open())
            {
                std::cout << "bulk: ";
                log_file  << "bulk: ";

                for (const auto& cmd : cmds)
                {
                    std::cout << cmd << ", ";
                    log_file  << cmd << ", ";
                }

                std::cout << std::endl;
                log_file  << std::endl;

                log_file.close();
                return true;
            }
        }

        return false;
    };

    StringList commands;
    int deep{};
    std::string line;
    std::string block_time;

    while (std::getline(std::cin, line)) 
    {
        if (line == "{") 
        {
            // Process previous static block
            ++deep;
            if (deep == 1)
            {
                if (log(block_time, commands))
                {
                    commands.clear();
                }
            }
        }
        else if (line == "}") 
        {
            // Process dynamic block
            --deep;
            if (deep == 0)
            {
                if (log(block_time, commands))
                {
                    commands.clear();
                }
            }
        }
        else if (line == "EOF") 
        {
            break;
        }
        else 
        {
            if (deep == 0)
            {
                if (commands.empty()) 
                {
                    block_time = get_current_time_stamp();
                }

                commands.emplace_back(line);
                if (commands.size() == block_size) 
                {
                    // Process static block
                    if (log(block_time, commands))
                    {
                        commands.clear();
                    }
                }
            }
            else 
            {
                // Process dynamic block
                commands.emplace_back(line);
            }
        }
    }

    // Process remaining static commands
    if (!commands.empty() && deep == 0)
    {
        if (log(block_time, commands))
        {
            commands.clear();
        }
    }
}

/**
 * @brief Main function to process command line arguments and start command processing.
 * 
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return Exit status of the program.
 */
int main(int argc, char* argv[]) noexcept
{
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <block size>" << std::endl;
        return -1;
    }

    int block_size{};

    try
    {
        block_size = std::stoi(argv[1]);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Invalid block size: " << ex.what() << std::endl;
    }

    ProcessCommands(block_size);
    
    return 0;
}
