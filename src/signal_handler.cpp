#include "signal_handler.hpp"
#include <iostream>

void setup_signal_handler() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
}

void signal_handler(int signal) {
    std::cout << "\nInterrupt signal (" << signal << ") received. Stopping processing..." << std::endl;
    GitModule::interrupt();
}

