#ifndef WEB_SERVER_H
#define WEB_SERVER_H

// Define the possible car states
enum CarCommand {
    CMD_STOP = 0,
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT
};

// Global variables to hold the current state from the web server
extern volatile CarCommand current_command;
extern volatile int current_gear;

// Function to start the server
void start_web_server();

#endif // WEB_SERVER_H