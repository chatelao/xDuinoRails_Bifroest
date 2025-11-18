# Model Railway Turnout Control

This project is a control system for model railway turnouts, designed for the Nucleo-G474RE board. It supports both modern servo-driven turnouts and traditional dual-coil linear motors (e.g., Märklin M-Weiche).

## Implemented Features

- [x] **Dual Motor Support**: Unified control for both servo and dual-coil motors.
- [x] **Soft Switching for Servos**: Gradual, smooth movement for servo-driven turnouts.
- [x] **Soft Switching for Coils**: Simulated gradual movement for dual-coil motors using timed pulses.
- [x] **Precise Position Detection**: Uses end-position sensors (end-switches) to stop motors accurately.
- [x] **Timeout Protection**: A safety timeout prevents motor damage if a turnout gets stuck.
- [x] **Non-Blocking Operation**: The control logic is implemented as a state machine to allow concurrent movement of multiple turnouts without blocking the main loop.
- [x] **PlatformIO Configuration**: Includes a `platformio.ini` file for easy compilation and deployment on a Nucleo-G474RE.

## Hardware

- **Board**: Nucleo-G474RE
- **Motors**:
    - Standard hobby servos
    - Dual-coil linear motors
- **Sensors**: End-switches for position feedback.

## Project Structure

- `README.md`: This file.
- `platformio.ini`: PlatformIO project configuration.
- `src/main.cpp`: The main application source code.
- `.gitignore`: Git ignore file for PlatformIO build artifacts.
