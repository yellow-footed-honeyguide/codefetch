#pragma once
#include <string>
#include <unordered_map>


struct LanguageInfo {
    std::string name;        
    std::string description; 
    std::string category;
};

extern const std::unordered_map<std::string, LanguageInfo> extension_to_language;


