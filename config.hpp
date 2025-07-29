#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fkYAML/node.hpp>
#include <string>
#include <stdexcept>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>

class Config {
private:
    static std::unique_ptr<Config> instance;
    fkyaml::node config_data;

    Config() = default;

public:
    static Config& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<Config>(new Config());
        }
        return *instance;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void loadFromFile(const std::string& filename) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open config file: " + filename);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();

            config_data = fkyaml::node::deserialize(buffer.str());
        } catch (const fkyaml::exception& e) {
            throw std::runtime_error("Failed to load config file '" + filename + "': " + e.what());
        }
    }

    template<typename T>
    T get(const std::string& key) const {
        try {
            if (!config_data.contains(key)) {
                throw std::runtime_error("Configuration key '" + key + "' not found");
            }
            return config_data[key].get_value<T>();
        } catch (const fkyaml::exception& e) {
            throw std::runtime_error("Failed to get config value for key '" + key + "': " + e.what());
        }
    }

    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        try {
            if (!config_data.contains(key)) {
                return default_value;
            }
            return config_data[key].get_value<T>();
        } catch (const fkyaml::exception&) {
            return default_value;
        }
    }

    template<typename T>
    T getNested(const std::string& path) const {
        try {
            fkyaml::node current = config_data;

            for (const std::vector<std::string> keys = splitPath(path); const auto& key : keys) {
                if (!current.contains(key)) {
                    throw std::runtime_error("Configuration path '" + path + "' not found");
                }
                current = current[key];
            }

            return current.get_value<T>();
        } catch (const fkyaml::exception& e) {
            throw std::runtime_error("Failed to get nested config value for path '" + path + "': " + e.what());
        }
    }

    template<typename T>
    T getNested(const std::string& path, const T& default_value) const {
        try {
            fkyaml::node current = config_data;

            for (const std::vector<std::string> keys = splitPath(path); const auto& key : keys) {
                if (!current.contains(key)) {
                    return default_value;
                }
                current = current[key];
            }

            return current.get_value<T>();
        } catch (const fkyaml::exception&) {
            return default_value;
        }
    }

    bool hasKey(const std::string& key) const {
        return config_data.contains(key);
    }

    bool hasNestedKey(const std::string& path) const {
        try {
            fkyaml::node current = config_data;

            for (const std::vector<std::string> keys = splitPath(path); const auto& key : keys) {
                if (!current.contains(key)) {
                    return false;
                }
                current = current[key];
            }

            return true;
        } catch (const fkyaml::exception&) {
            return false;
        }
    }

private:
    static std::vector<std::string> splitPath(const std::string& path) {
        std::vector<std::string> keys;
        std::stringstream ss(path);
        std::string key;

        while (std::getline(ss, key, '.')) {
            if (!key.empty()) {
                keys.push_back(key);
            }
        }

        return keys;
    }
};

std::unique_ptr<Config> Config::instance = nullptr;

#define CONFIG Config::getInstance()
#define CONFIG_GET(key, type) Config::getInstance().get<type>(key)
#define CONFIG_GET_DEFAULT(key, type, default_val) Config::getInstance().get<type>(key, default_val)
#define CONFIG_GET_NESTED(path, type) Config::getInstance().getNested<type>(path)
#define CONFIG_GET_NESTED_DEFAULT(path, type, default_val) Config::getInstance().getNested<type>(path, default_val)

#endif