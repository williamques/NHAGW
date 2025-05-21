#include "CLI.h"
#include <iostream>

CLI::CLI(Model* model) : model(model) {
    std::cout << "[CLI] CLI object created on thread: " << std::this_thread::get_id() << std::endl;
}

CLI::~CLI() {
    stop();
    std::cout << "[CLI] CLI object destroyed on thread: " << std::this_thread::get_id() << std::endl;
}

void CLI::start() {
    std::cout << "[CLI] Starting IO thread from thread: " << std::this_thread::get_id() << std::endl;
    // Start IO thread
    ioThread = std::thread(&CLI::processInput, this);
    std::cout << "[CLI] IO thread started with ID: " << ioThread.get_id() << std::endl;

    // Display initial help
    displayHelp();
}

void CLI::stop() {
    std::cout << "[CLI] Stopping IO thread from thread: " << std::this_thread::get_id() << std::endl;
    running = false;
    
    if (ioThread.joinable()) {
        ioThread.join();
        std::cout << "[CLI] IO thread with ID: " << ioThread.get_id() << " has exited." << std::endl;
    }
}

void CLI::processInput() {
    std::cout << "[CLI] IO thread running with ID: " << std::this_thread::get_id() << std::endl;
    while (running) {
        std::string input;
        std::cin >> input;
        handleCommand(input);
    }
    std::cout << "[CLI] IO thread exiting with ID: " << std::this_thread::get_id() << std::endl;
}

void CLI::handleCommand(const std::string& command) {
    std::string cmd, rmd;
    int end = command.find(' ');
    cmd = command.substr(0, end);
    rmd = command.substr(end + 1);
    std::cout << "[CLI] Command Handled by Thread with ID: " << std::this_thread::get_id() << std::endl;
    if (cmd == "help") {
        displayHelp();
    }
    else if (cmd == "step") {
        model->stepOnce();
        model->wake();
    }
    else if (cmd == "play") {
        model->setPlaying(true);
        model->wake();
        std::cout << "Simulation started" << std::endl;
    }
    else if (cmd == "pause") {
        model->setPlaying(false);
        std::cout << "Simulation paused" << std::endl;
    }
    else if (cmd == "display") {
        displayGrid();
    }
    else if (cmd == "metrics") {
        model->collectMetrics();
    }
    else if (cmd == "quit") {
        model->setRunning(false);
        running = false;
    }
    else {
        std:: cout << "Unknown command. Type 'help' for available commands." << std::endl;
    }
}

void CLI::displayGrid() const {
    std::cout << "\nCurrent Grid State (Step: " << model->getStepCount() << "):\n";
    for (int i = 0; i < model->getHeight(); ++i) {
        for (int j = 0; j < model->getWidth(); ++j) {
            const Cell* cell = model->getCell(i, j);
            if (cell->hasType("Tree")) {
                std::cout << "T ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void CLI::displayHelp() const {
    std::cout << "\nAvailable Commands:\n"
       << "  help     - Display this help message\n"
       << "  step     - Perform one simulation step\n"
       << "  play     - Start continuous simulation\n"
       << "  pause    - Pause continuous simulation\n"
       << "  speed X  - Set simulation speed to X (e.g., 0.5, 1, 2)\n"
       << "  display  - Show current grid state\n"
       << "  quit     - Exit the program\n"
       << std::endl;
} 