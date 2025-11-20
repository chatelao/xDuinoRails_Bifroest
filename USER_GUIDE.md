# xDuinoRails_Turnouts User Guide

This guide provides detailed instructions for using the `xDuinoRails_Turnouts` library to build your own model railway turnout decoder.

## API Reference

### `xDuinoRails_Turnout`

This is the main class for controlling a standard two-way turnout.

#### Constructors

**For Servo or Coil Motors with End-Switches:**

```cpp
xDuinoRails_Turnout(int id, const char* name, MotorType motorType, int pin1, int pin2, int sensorPin1, int sensorPin2, int angleMin = 30, int angleMax = 150);
```

*   `id`: A unique integer to identify the turnout.
*   `name`: A descriptive name for the turnout (used in serial output).
*   `motorType`: The type of motor. Can be `MOTOR_SERVO` or `MOTOR_COIL`.
*   `pin1`, `pin2`: The motor control pins. For a servo, only `pin1` is used. For a coil, both are used.
*   `sensorPin1`, `sensorPin2`: The digital input pins for the end-switches.
*   `angleMin`, `angleMax`: (Optional) The minimum and maximum angles for a servo motor.

**For Coil Motors with BEMF Detection:**

```cpp
xDuinoRails_Turnout(int id, const char* name, MotorType motorType, int pwm_a, int pwm_b, int bemf_a, int bemf_b);
```

*   `motorType`: Must be `MOTOR_COIL_BEMF`.
*   `pwm_a`, `pwm_b`: The PWM output pins for the motor driver.
*   `bemf_a`, `bemf_b`: The analog input pins for BEMF sensing.

#### Methods

*   `void begin()`: Initializes the turnout. Call this in your `setup()` function.
*   `void update()`: Updates the turnout's state machine. Call this in your `loop()` function.
*   `void setPosition(int position)`: Sets the target position of the turnout (1 or 2).

### `xDuinoRails_ThreeWayTurnout`

This class controls a Märklin-style three-way turnout, which is composed of two standard coil turnouts.

#### Constructor

```cpp
xDuinoRails_ThreeWayTurnout(int id, const char* name, int coilPin1_A, int coilPin2_A, int sensorPin1_A, int sensorPin2_A, int coilPin1_B, int coilPin2_B, int sensorPin1_B, int sensorPin2_B);
```

*   The parameters are the same as for two standard coil turnouts, with `_A` and `_B` suffixes to distinguish between the two coil pairs.

#### Methods

*   `void begin()`: Initializes the turnout.
*   `void update()`: Updates the turnout's state machine.
*   `void setPosition(int position)`: Sets the target position (0 for straight, 1 for left, 2 for right).

## Wiring Diagrams

### Servo with End-Switches

*   **Servo:** Connect the servo's PWM pin to the pin specified in the constructor (`pin1`), and its power and ground to your board's 5V and GND.
*   **End-Switches:** Connect the two end-switches to the specified digital input pins (`sensorPin1`, `sensorPin2`) and ground. The library uses `INPUT_PULLUP`, so you don't need external pull-up resistors.

### Coil with End-Switches

*   **Motor Driver:** Connect the two coil control pins (`pin1`, `pin2`) to the inputs of a motor driver (e.g., an L298N or a Darlington array). The motor driver's outputs should be connected to the turnout's coils.
*   **End-Switches:** Same as for the servo.

### Coil with BEMF Detection

*   **Motor Driver:** Connect the two PWM pins (`pwm_a`, `pwm_b`) to the PWM inputs of a suitable motor driver.
*   **BEMF Sensing:** The BEMF sensing pins (`bemf_a`, `bemf_b`) should be connected to the motor's terminals, typically through a voltage divider to protect the analog inputs of your microcontroller.

## Code Example

Please see the `src/main.cpp` file for a complete, working example that demonstrates how to use all three turnout types.
