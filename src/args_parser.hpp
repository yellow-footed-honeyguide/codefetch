#ifndef ARGS_PARSER_HPP
#define ARGS_PARSER_HPP

#include <string>         // [C++98]
#include <vector>         // [C++98]
#include <map>            // [C++98]
#include <stdexcept>      // [C++98]

class ArgsParser {
private:
    std::string program_name;            // [C++11] Program name storage
    std::string directory;               // [C++11] Directory path storage
    std::map<std::string, bool*> flags;  // [C++11] Flag pointers map
    std::string version;                 // [C++11] Version string

public:
    ArgsParser(const std::string& name, const std::string& ver) : // [C++11] Constructor with program info
        program_name(name), version(ver) {}

    void add_flag(const std::string& name, bool* value) {         // [C++11] Add flag to parser
        flags[name] = value;
    }

    void parse(int argc, char* argv[]) {                          // [C++11] Parse command line arguments
        std::vector<std::string> args(argv + 1, argv + argc);     // [C++11] Convert to vector
        
        for (size_t i = 0; i < args.size(); ++i) {   // Process all arguments
            const auto& arg = args[i];
            
            if (arg == "-v" || arg == "--version") { // Handle version flag
                std::cout << program_name << " version " << version << std::endl;
                std::exit(0);
            }
            
            if (arg.substr(0, 2) == "--") {          // [C++11] Handle long flags
                std::string flag = arg.substr(2);
                auto it = flags.find(flag);
                if (it != flags.end()) {
                    *it->second = true;
                    continue;
                }
            }
            
            if (arg[0] == '-') {                     // Handle short flags
                std::string flag = arg.substr(1);
                auto it = flags.find(flag);
                if (it != flags.end()) {
                    *it->second = true;
                    continue;
                }
            }
            
            if (directory.empty()) {                 // Handle directory argument
                directory = arg;
            }
        }
        
        if (directory.empty()) {                     // [C++11] Check required directory
            throw std::runtime_error("Directory path is required");
        }
    }

    const std::string& get_directory() const {       // [C++11] Directory getter
        return directory;
    }
};

#endif // ARGS_PARSER_HPP
