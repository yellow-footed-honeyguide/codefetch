module;  
#include <string>             
#include <regex>   
#include <fstream>  
#include <sstream>  
#include <iostream> 
#include <unordered_map> 
#include <filesystem>        
export module License_module;
import Output_formatter;
import Statistics_module;

export class LicenseModule : public CodeFetchModule {
private:
    std::string detected_license;
    std::unordered_map<std::string, std::regex> license_patterns = {
        {"MIT", std::regex("MIT License|Permission is hereby granted, free of charge,|The MIT License \\(MIT\\)")},
        {"GPL-3.0", std::regex("GNU GENERAL PUBLIC LICENSE|Version 3,")},
        {"Apache-2.0", std::regex("Apache License|Version 2.0,")},
        {"BSD-3-Clause", std::regex("BSD 3-Clause License|Redistribution and use in source and binary forms")},
        {"LGPL-3.0", std::regex("GNU LESSER GENERAL PUBLIC LICENSE|Version 3,")},
        {"MPL-2.0", std::regex("Mozilla Public License Version 2.0")},
        {"Unlicense", std::regex("This is free and unencumbered software released into the public domain")},
        {"LGPL-2.1", std::regex("GNU Lesser General Public License|Version 2.1,")},
        {"GPL-2.0", std::regex("GNU GENERAL PUBLIC LICENSE|Version 2,")},
        {"BSD-2-Clause", std::regex("BSD 2-Clause License|Redistribution and use in source and binary forms")},
        {"AGPL-3.0", std::regex("GNU AFFERO GENERAL PUBLIC LICENSE|Version 3,")},
        {"EPL-2.0", std::regex("Eclipse Public License - v 2.0")},
        {"CDDL-1.0", std::regex("COMMON DEVELOPMENT AND DISTRIBUTION LICENSE \\(CDDL\\) Version 1.0")},
        {"ISC", std::regex("ISC License|Permission to use, copy, modify, and/or distribute this software")},
        {"Artistic-2.0", std::regex("The Artistic License 2.0|Copyright \\(c\\) 2000-2006, The Perl Foundation")},
        {"Zlib", std::regex("zlib License|This software is provided 'as-is', without any express or implied warranty")}
    };

public:
    LicenseModule() = default;
    
    virtual ~LicenseModule(){ } // to fix this: undefined reference to 'vtable...
    
    void process_file(const std::filesystem::path& file_path) override{
        std::string filename = file_path.filename().string();
        if (filename == "LICENSE" || filename == "LICENSE.txt" || 
            filename == "LICENSE.md" || filename == "COPYING" || 
            filename == "README.md" || filename == "README") {
            std::ifstream file(file_path);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
                detect_license(content);
            }
        }
    }

    void print_stats() const override{
        if (!detected_license.empty()) {
            std::vector<std::pair<std::string, std::string>> items = {
                {"License", detected_license}
            };
            OutputFormatter::print_section("License", "§", items);
        }
    }

private:
    void detect_license(const std::string& content) {
        for (const auto& [license, pattern] : license_patterns) {
            if (std::regex_search(content, pattern)) {
                detected_license = license;
                return;
            }
        }
    }
};
