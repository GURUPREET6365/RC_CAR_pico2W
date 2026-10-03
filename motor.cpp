#include "motor.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

const uint ENA_PIN = 6; // Connects to L298N ENA (Left Motors)
const uint ENB_PIN = 7; // Connects to L298N ENB (Right Motors)

// Define your GPIO pins connected to the Motor Driver
const uint LEFT_FORWARD_PIN = 2;   // e.g., IN1
const uint LEFT_BACKWARD_PIN = 3;  // e.g., IN2
const uint RIGHT_FORWARD_PIN = 4;  // e.g., IN3
const uint RIGHT_BACKWARD_PIN = 5; // e.g., IN4

void motor_init() {
    // ... [Initialize IN1-IN4 like before] ...
        // Initialize all 4 pins
    gpio_init(LEFT_FORWARD_PIN);
    gpio_init(LEFT_BACKWARD_PIN);
    gpio_init(RIGHT_FORWARD_PIN);
    gpio_init(RIGHT_BACKWARD_PIN);

    // Set them all as outputs
    gpio_set_dir(LEFT_FORWARD_PIN, GPIO_OUT);
    gpio_set_dir(LEFT_BACKWARD_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_FORWARD_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_BACKWARD_PIN, GPIO_OUT);
    
    // Ensure motors are off at startup
    // 1. Tell the Pico that these pins will use Hardware PWM
    gpio_set_function(ENA_PIN, GPIO_FUNC_PWM);
    gpio_set_function(ENB_PIN, GPIO_FUNC_PWM);
    
    // 2. Find out which hardware "slice" owns these pins
    uint slice_left = pwm_gpio_to_slice_num(ENA_PIN);
    uint slice_right = pwm_gpio_to_slice_num(ENB_PIN);
    
    // 3. Set the resolution (65535 is the max 16-bit value)
    pwm_set_wrap(slice_left, 10000);
    pwm_set_wrap(slice_right, 10000);
    
    // 4. Turn the PWM generators on
    pwm_set_enabled(slice_left, true);
    pwm_set_enabled(slice_right, true);
    motor_stop();
}

void set_motor_speed(int gear) {
    uint slice_left = pwm_gpio_to_slice_num(ENA_PIN);
    uint slice_right = pwm_gpio_to_slice_num(ENB_PIN);

    uint16_t speed_value = 0;

    if (gear == 1) speed_value = 6000; // ~33% speed (65535 / 3)
    if (gear == 2) speed_value = 8000; // ~66% speed (65535 * 0.66)
    if (gear == 3) speed_value = 10001; // 100% speed

    // Apply the speed to the ENA and ENB pins
    pwm_set_chan_level(slice_left, pwm_gpio_to_channel(ENA_PIN), speed_value);
    pwm_set_chan_level(slice_right, pwm_gpio_to_channel(ENB_PIN), speed_value);
}


void motor_forward(int gear) {
    set_motor_speed(gear);
    // To go forward, both wheels spin forward
    gpio_put(LEFT_FORWARD_PIN, 1);
    gpio_put(LEFT_BACKWARD_PIN, 0);
    
    gpio_put(RIGHT_FORWARD_PIN, 1);
    gpio_put(RIGHT_BACKWARD_PIN, 0);
    
    // (Later, you can use the 'gear' variable to set PWM speed here instead of just '1')
}

void motor_backward(int gear) {
    set_motor_speed(gear);
    // To go backward, both wheels spin in reverse
    gpio_put(LEFT_FORWARD_PIN, 0);
    gpio_put(LEFT_BACKWARD_PIN, 1);
    
    gpio_put(RIGHT_FORWARD_PIN, 0);
    gpio_put(RIGHT_BACKWARD_PIN, 1);
}

void motor_left(int gear) {
    set_motor_speed(gear);
    // To turn left: Left wheel reverses, Right wheel goes forward (Tank turn)
    gpio_put(LEFT_FORWARD_PIN, 0);
    gpio_put(LEFT_BACKWARD_PIN, 1);
    
    gpio_put(RIGHT_FORWARD_PIN, 1);
    gpio_put(RIGHT_BACKWARD_PIN, 0);
}

void motor_right(int gear) {
    set_motor_speed(gear);
    // To turn right: Left wheel goes forward, Right wheel reverses
    gpio_put(LEFT_FORWARD_PIN, 1);
    gpio_put(LEFT_BACKWARD_PIN, 0);
    
    gpio_put(RIGHT_FORWARD_PIN, 0);
    gpio_put(RIGHT_BACKWARD_PIN, 1);
}

void motor_stop() {
    // Cut power to all pins
    gpio_put(LEFT_FORWARD_PIN, 0);
    gpio_put(LEFT_BACKWARD_PIN, 0);
    gpio_put(RIGHT_FORWARD_PIN, 0);
    gpio_put(RIGHT_BACKWARD_PIN, 0);
}
