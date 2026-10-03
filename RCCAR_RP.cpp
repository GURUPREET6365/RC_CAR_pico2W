#include "pico/stdlib.h"
#include <stdio.h>
#include "wifi_ap.h" 
#include "web_server.h"
#include "motor.h"       // <-- Include your new motor header


int main() {
    stdio_init_all();
    sleep_ms(2000); 

    printf("Starting RC Car...\n");

    // Initialize the motor driver pins
    motor_init();

    if (!start_access_point()) {
        printf("Halting due to Wi-Fi error.\n");
        while (true) sleep_ms(1000);
    }

    start_web_server();

    CarCommand last_command = CMD_STOP;
    int last_gear = 1;

    while (true) {
        // If the web server received a new command, apply it to the hardware
        if (current_command != last_command || current_gear != last_gear) {
            
            last_command = current_command;
            last_gear = current_gear;

            printf("Applying Hardware Command - Gear: %d\n", current_gear);

            // Execute the actual physical motor functions!
            switch (current_command) {
                case CMD_FORWARD:  motor_forward(current_gear); break;
                case CMD_BACKWARD: motor_backward(current_gear); break;
                case CMD_LEFT:     motor_left(current_gear); break;
                case CMD_RIGHT:    motor_right(current_gear); break;
                case CMD_STOP:     motor_stop(); break;
            }
        }
        
        sleep_ms(50);
    }
    return 0;
}