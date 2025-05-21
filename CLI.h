#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Model.h"

class CLI {
private:
    Model* model;
    std::thread ioThread;
    std::atomic<bool> running{true};

    void processInput();
    void handleCommand(const std::string& command);

public:
    CLI(Model* model);
    ~CLI();

    void start();
    void stop();
    void displayGrid() const;
    void displayHelp() const;
}; 