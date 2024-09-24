// license_module.hpp
#ifndef LICENSE_MODULE_HPP
#define LICENSE_MODULE_HPP

#include "statistics_module.hpp"
#include <string>
#include <unordered_map>
#include <regex>

class LicenseModule : public StatisticsModule {
private:
    std::string detected_license;
    std::unordered_map<std::string, std::regex> license_patterns = {
        {"MIT", std::regex("MIT License|Permission is hereby granted, free of charge,|The MIT License \\(MIT\\)")},
        {"GPL-3.0", std::regex("GNU GENERAL PUBLIC LICENSE|Version 3,")},
        {"Apache-2.0", std::regex("Apache License|Version 2.0,")},
        {"BSD-3-Clause", std::regex("BSD 3-Clause License|Redistribution and use in source and binary forms")},
        {"LGPL-3.0", std::regex("GNU LESSER GENERAL PUBLIC LICENSE|Version 3,")},
        {"MPL-2.0", std::regex("Mozilla Public License Version 2.0")},
        {"Unlicense", std::regex("This is free and unencumbered software released into the public domain")}
    };

public:
    void process_file(const fs::path& file_path) override;
    void print_stats() const override;

private:
    void detect_license(const std::string& content);
};

#endif // LICENSE_MODULE_HPP
