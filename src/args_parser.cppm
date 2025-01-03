module; 
#include <print>
#include <iostream>
#include <string>      
#include <vector>       
#include <map>  
#include <stdexcept>
export module args_parser;

export class ArgsParser {
private:
    std::string program_name; // Program name storage
    std::string directory; // Directory path storage
    std::map<std::string, bool*> flags; // Flag pointers map
    std::string version; // Version string

public:
    ArgsParser(const std::string& name, const std::string& ver) : // Constructor with program info
        program_name(name), version(ver) {}

    void add_flag(const std::string& name, bool* value) { // Add flag to parser
        flags[name] = value;
    }

    void parse(int argc, char* argv[]) { // Parse command line arguments
        std::vector<std::string> args(argv + 1, argv + argc); // Convert to vector
        
        for (size_t i = 0; i < args.size(); ++i) { // Process all arguments
            const auto& arg = args[i];
            
            if (arg == "-v" || arg == "--version") { // Handle version flag
                //std::cout << program_name << " version " << version << std::endl;
                println("{} version {}", program_name, version);
                std::exit(0);
            }

            if (arg == "-h" || arg == "--help") { // Handle help flag
                std::println("Usage: {} [OPTION] [DIRECTORY]", program_name);
                std::println("Fast tool for source code analysis.\n");
                std::println("Options:");
                std::println("-v, --version            Show version information");
                std::println("-c, --line_counter       Show line counter statistics");
                std::println("-l, --languages          Show language statistics");
                std::println("-g, --git-statistics     Show git statistics information");
                std::println("-m, --metabuild_system   Show metabuild system information");
                std::println("-i, --license            Show license information");
                std::println("-m, --metabuild_system   Show metabuild system information");
                std::exit(0);
            }
            
            if (arg.substr(0, 2) == "--") { // Handle long flags
                std::string flag = arg.substr(2);
                auto it = flags.find(flag);
                if (it != flags.end()) {
                    *it->second = true;
                    continue;
                }
            }
            
            if (arg[0] == '-') { // Handle short flags
                std::string flag = arg.substr(1);
                auto it = flags.find(flag);
                if (it != flags.end()) {
                    *it->second = true;
                    continue;
                }
            }
            
            if (directory.empty()) { // Handle directory argument
                directory = arg;
            }
        }
        
        if (directory.empty()) { // Check required directory
            throw std::runtime_error("Directory path is required");
        }
    }

    const std::string& get_directory() const { // Directory getter
        return directory;
    }
};
