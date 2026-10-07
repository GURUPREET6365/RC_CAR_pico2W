#include "motor.h"
#include "pico/stdlib.h"

// --- WIRING DEFINITIONS ---
const uint LEFT_IN1_PIN = 20;    
const uint LEFT_IN2_PIN = 21;  
const uint RIGHT_IN1_PIN = 10; 
const uint RIGHT_IN2_PIN = 5; 

// --- INITIALIZATION ---
void motor_init() {
    gpio_init(LEFT_IN1_PIN);
    gpio_init(LEFT_IN2_PIN);
    gpio_init(RIGHT_IN1_PIN);
    gpio_init(RIGHT_IN2_PIN);

    // 3. Now safely set as output
    gpio_set_dir(LEFT_IN1_PIN, GPIO_OUT);
    gpio_set_dir(LEFT_IN2_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_IN1_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_IN2_PIN, GPIO_OUT);
    
    motor_stop();
}


void motor_forward(int gear) {
    gpio_put(LEFT_IN1_PIN, 0); 
    gpio_put(LEFT_IN2_PIN, 1);

    gpio_put(RIGHT_IN2_PIN, 0);
    gpio_put(RIGHT_IN1_PIN, 1);
}

void motor_backward(int gear) {
    gpio_put(LEFT_IN1_PIN, 1); 
    gpio_put(LEFT_IN2_PIN, 0);

    gpio_put(RIGHT_IN2_PIN, 1);
    gpio_put(RIGHT_IN1_PIN, 0);
}

void motor_left(int gear) {
    gpio_put(RIGHT_IN2_PIN, 0);
    gpio_put(RIGHT_IN1_PIN, 1);

    gpio_put(LEFT_IN1_PIN, 1); 
    gpio_put(LEFT_IN2_PIN, 0);
}

void motor_right(int gear) {
    gpio_put(LEFT_IN1_PIN, 0); 
    gpio_put(LEFT_IN2_PIN, 1);

    gpio_put(RIGHT_IN2_PIN, 1);
    gpio_put(RIGHT_IN1_PIN, 0);
}

void motor_stop() {
    gpio_put(LEFT_IN1_PIN, 0);
    gpio_put(LEFT_IN2_PIN, 0);
    gpio_put(RIGHT_IN1_PIN, 0);
    gpio_put(RIGHT_IN2_PIN, 0);
}