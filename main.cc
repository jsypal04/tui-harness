#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <cjson/json.h>
#include <thread>
#include <unistd.h>

#define CONFIG_FILE ".config/jev-harness/config.json"

Json* read_config() {
    const char* home = std::getenv("HOME");
    if (home == nullptr) {
        std::cerr << "Failed to load HOME env var.\n";
        return nullptr;
    }

    std::string config_path = std::string(home) + "/" + CONFIG_FILE;

    std::ifstream config_stream(config_path);
    std::stringstream buffer;
    buffer << config_stream.rdbuf();

    std::string json_source = buffer.str();
    
    try {
        Json* config = new Json(json_source);
        return config;
    } catch (std::exception& e) {
        return nullptr;
    }
}


void application_listener_thread(std::string socket_path) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "Failed to get socket\n";
        return;
    }

    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cout << "Failed to connect to " << socket_path << "\n";
        std::cout << "Error number: " << errno << "\n";
        close(fd);
        return;
    }

    std::cout << "Connected to " << socket_path << "\n";

    size_t n = 2048;
    char buffer[n];
    ssize_t size;

    do {
        size = read(fd, buffer, n);
        bzero(buffer + size, n - size);
        std::cout << buffer << "\n";
    } while (size > 0);
}

int main() {
    Json* config = read_config();

    auto threads = std::map<std::string, std::thread>();

    for (auto& [app, properties] : *config) {
        auto app_properties = properties.get<std::map<std::string, Value>>();
        auto socket_path = app_properties.at("socket_path").get<std::string>();

        std::thread thread(application_listener_thread, socket_path);
        
        threads.emplace(socket_path, std::move(thread));
    }

    for (auto& [socket_path, thread] : threads) {
        thread.join();
    }


    delete config;
}
