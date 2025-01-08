#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <map>


class TextFileHandler {
    std::map<std::string, std::string> data;

public:

	void load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            size_t eq_pos = line.find('=');
            if (eq_pos == std::string::npos) continue;

            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(eq_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            data[key] = value;
        }
        file.close();
    }

    void save(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        for (const auto& kv : data) {
            file << kv.first << "=" << kv.second << "\n";
        }
        file.close();
    }

    std::string get(const std::string& key, const std::string& default_value = "") const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return default_value;
    }

    void set(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    bool keyExists(const std::string& key) const
	{
        return data.contains(key);
    }
};
