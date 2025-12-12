# xDuinoRails_Turnouts User Guide

This guide provides detailed instructions for using the `xDuinoRails_Turnouts` library to build your own model railway turnout decoder.

## API Reference

### `xDuinoRails_Turnout`

This is the main class for controlling a standard two-way turnout.

#### Configuration Structs

**`BEMF_Config`**

Used for configuring turnouts with BEMF-based position detection.

```cpp
struct BEMF_Config {
    int pwm_a_pin;          // PWM output pin for coil A
    int pwm_b_pin;          // PWM output pin for coil B
    int bemf_a_pin;         // Analog input pin for measuring BEMF on coil A
    int bemf_b_pin;         // Analog input pin for measuring BEMF on coil B
    int bemf_threshold = 10; // (Optional) Threshold for BEMF detection. Default: 10
    int bemf_stall_count = 5; // (Optional) Number of consecutive detections required to confirm stall. Default: 5
};
```

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
xDuinoRails_Turnout(int id, const char* name, const BEMF_Config& bemf_config);
```

*   `id`: A unique integer to identify the turnout.
*   `name`: A descriptive name for the turnout.
*   `bemf_config`: A `BEMF_Config` struct containing pin assignments and detection parameters.

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

### DCC Input

*   **Optocoupler:** Connect the DCC signal from the track to the defined DCC pin (default: D0) using an optocoupler circuit (e.g., 6N137). The optocoupler protects the microcontroller from the high track voltage and isolates the signal.
    *   Track Signal A -> 1k Resistor -> Optocoupler Anode
    *   Track Signal B -> Optocoupler Cathode
    *   Optocoupler VCC -> 3.3V/5V
    *   Optocoupler GND -> GND
    *   Optocoupler Output (Collector) -> DCC Pin (with pull-up enabled in software)

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
