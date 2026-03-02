#pragma once
#include <string>

class ConfigManager {
public:
    static bool SaveConfig(const std::string& filepath = "sdcard/topconf.cfg");
    static bool LoadConfig(const std::string& filepath = "sdcard/topconf.cfg");
    
private:
    static void WriteFloatArray(std::ofstream& file, const std::string& name, float* values, int count);
    static void WriteFloat(std::ofstream& file, const std::string& name, float value);
    static bool ReadFloatArray(std::ifstream& file, const std::string& name, float* values, int count);
    static bool CreateDirectoryIfNotExists(const std::string& path);
    static std::string GetDirectoryFromPath(const std::string& filepath);
};