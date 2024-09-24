#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <csignal>
#include "git_module.hpp"

void setup_signal_handler();
void signal_handler(int signal);

#endif // SIGNAL_HANDLER_HPP

