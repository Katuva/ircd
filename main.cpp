#include <iostream>
#include "logger.hpp"
#include "config.hpp"

int main() {
    try {
        // Load configuration at startup
        CONFIG.loadFromFile("config.yaml");

        // Set log level from configuration
        const auto log_level = CONFIG_GET_NESTED_DEFAULT("logging.level", std::string, "INFO");
        auto level = Logger::Level::INFO;
        if (log_level == "DEBUG") level = Logger::Level::DEBUG;
        else if (log_level == "WARNING") level = Logger::Level::WARNING;
        else if (log_level == "ERROR") level = Logger::Level::ERROR;
        else if (log_level == "FATAL") level = Logger::Level::FATAL;

        Logger::getInstance().setLogLevel(level);

        // Get server configuration
        auto server_name = CONFIG_GET_NESTED_DEFAULT("server.name", std::string, "DefaultIRCd");
        int port = CONFIG_GET_NESTED_DEFAULT("server.port", int, 6667);
        auto bind_address = CONFIG_GET_NESTED_DEFAULT("server.bind_address", std::string, "0.0.0.0");
        int max_connections = CONFIG_GET_NESTED_DEFAULT("server.max_connections", int, 100);

        LOG_INFO("ircd is starting up...");
        LOG_INFO("Server name: {}", server_name);
        LOG_INFO("Binding to {}:{}", bind_address, port);
        LOG_INFO("Max connections: {}", max_connections);

        LOG_INFO("ircd has started");
        LOG_DEBUG("ircd is running on port {}", port);

        // Example of checking optional configuration
        if (CONFIG.hasNestedKey("ssl.enabled") && CONFIG_GET_NESTED("ssl.enabled", bool)) {
            int ssl_port = CONFIG_GET_NESTED_DEFAULT("ssl.port", int, 6697);
            LOG_INFO("SSL enabled on port {}", ssl_port);
        }

        // Example of getting feature configuration
        int ping_timeout = CONFIG_GET_NESTED_DEFAULT("features.ping_timeout", int, 120);
        int max_channels = CONFIG_GET_NESTED_DEFAULT("features.max_channels_per_user", int, 10);
        LOG_DEBUG("Ping timeout: {}s, Max channels per user: {}", ping_timeout, max_channels);

    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}