// license_module.hpp
#ifndef LICENSE_MODULE_HPP
#define LICENSE_MODULE_HPP

#include "statistics_module.hpp"
#include <string>
#include <unordered_map>
#include <regex>

class LicenseModule : public CodeFetchModule {
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
    void process_file(const fs::path& file_path) override;
    void print_stats() const override;

private:
    void detect_license(const std::string& content);
};

#endif // LICENSE_MODULE_HPP
