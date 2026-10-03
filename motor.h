#ifndef MOTOR_H
#define MOTOR_H

// Initialize the GPIO pins for the motor driver
void motor_init();

// Directional functions (passing the gear for speed control)
void motor_forward(int gear);
void motor_backward(int gear);
void motor_left(int gear);
void motor_right(int gear);
void motor_stop();

#endif // MOTOR_H