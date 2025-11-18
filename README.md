# Concept for Model Railway Turnout Control

This document outlines the concept for a model railway turnout control system to be implemented on a Nucleo-G474RE board. The system will control both servo-driven turnouts and traditional dual-coil linear motors (e.g., Märklin M-Weiche).

## Key Features

- **Soft Switching**: Gradual movement for both servo and linear motors.
- **Position Detection**: Use of end-position sensors (end-switches) to stop motors precisely.
- **Timeout Protection**: A safety timeout to prevent motor damage.
- **Generic Control**: A unified control interface for different motor types.

## Data Structures

A central `Turnout` struct will encapsulate all data for a single turnout. This allows for easy management of multiple turnouts.

```cpp
enum MotorType {
  MOTOR_SERVO,
  MOTOR_COIL
};

struct Turnout {
  // Common properties
  int id;
  const char* name;
  MotorType motorType;

  // Motor-specific configuration
  union {
    struct {
      int pin;
      int angleMin;
      int angleMax;
    } servo;
    struct {
      int pin1;
      int pin2;
    } coil;
  } motor;

  // Sensor pins
  int sensorPin1;
  int sensorPin2;

  // State machine
  enum State {
    STATE_IDLE,
    STATE_MOVING_TO_POS1,
    STATE_MOVING_TO_POS2
  } state;

  // Target position
  int targetPosition; // 0 or 1

  // Timing for non-blocking movement
  unsigned long lastMoveTime;
  unsigned long moveStartTime;
};
```

## State Machine

The control logic for each turnout will be implemented as a state machine within a main loop function (`updateTurnout`). This ensures non-blocking operation, allowing for smooth and concurrent movement of multiple turnouts.

The states are:

- **`STATE_IDLE`**: The turnout is at a stable position (either position 1 or 2). No motor is active.
- **`STATE_MOVING_TO_POS1`**: The turnout is moving towards position 1.
- **`STATE_MOVING_TO_POS2`**: The turnout is moving towards position 2.

Transitions between states are triggered by setting the `targetPosition` and are managed by the `updateTurnout` function.

## Motor Control Logic

### Servo Motors

- **Soft Switching**: The servo will move in small, incremental steps. The `lastMoveTime` will be used to control the speed of the movement.
- **Position Detection**: The servo moves to the predefined `angleMin` and `angleMax` values. End-switches are used to confirm the final position.
- **Timeout Protection**: A timeout (`moveStartTime`) will be used to detect if a servo gets stuck.

### Dual-Coil Linear Motors

- **Soft Switching**: This is the most innovative part of the concept for linear motors. Instead of a single, long pulse, the motor will be driven by a series of short pulses. This will create a "stuttering" motion that approximates a gradual movement.
- **Position Detection**: The end-switches are crucial here. The pulsing continues until the corresponding end-switch is triggered.
- **Timeout Protection**: If the end-switch is not reached within a specified time, the pulsing is stopped to prevent the coil from overheating.

## Main Loop

The main `loop()` function in Arduino will iterate through an array of `Turnout` objects and call `updateTurnout()` for each one.

```cpp
void loop() {
  for (int i = 0; i < numTurnouts; ++i) {
    updateTurnout(&turnouts[i]);
  }
  // Other non-blocking tasks can run here
}
```

## `updateTurnout` Function

This function will contain the core logic of the state machine.

```cpp
void updateTurnout(Turnout* turnout) {
  // Read sensor values
  bool sensor1_active = digitalRead(turnout->sensorPin1) == LOW;
  bool sensor2_active = digitalRead(turnout->sensorPin2) == LOW;

  switch (turnout->state) {
    case STATE_IDLE:
      // Check if a new target position is set
      if (turnout->targetPosition == 1 && !sensor1_active) {
        turnout->state = STATE_MOVING_TO_POS1;
        turnout->moveStartTime = millis();
        // Start moving
      } else if (turnout->targetPosition == 2 && !sensor2_active) {
        turnout->state = STATE_MOVING_TO_POS2;
        turnout->moveStartTime = millis();
        // Start moving
      }
      break;

    case STATE_MOVING_TO_POS1:
      // Check for timeout
      if (millis() - turnout->moveStartTime > TIMEOUT_MS) {
        turnout->state = STATE_IDLE;
        // Stop motor
        return;
      }

      // Check if position is reached
      if (sensor1_active) {
        turnout->state = STATE_IDLE;
        // Stop motor
      } else {
        // Continue moving (pulsing for coil, stepping for servo)
      }
      break;

    case STATE_MOVING_TO_POS2:
      // ... similar logic for moving to position 2
      break;
  }
}
```
