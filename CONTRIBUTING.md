# Contributing to xDuinoRails_Turnouts

First off, thank you for considering contributing to this project! Your help is greatly appreciated.

## Getting Started

### Development Environment

This project is built using PlatformIO. To get started, you'll need to install the [PlatformIO CLI](https://platformio.org/install/cli). Once you have it installed, you can build the project by running:

```bash
pio run
```

To clean the build artifacts, you can use:

```bash
pio run -t clean
```

### Hardware

The primary development target for this project is the `Nucleo-G474RE` board. While the code is written to be as portable as possible, all pull requests should be tested against this target.

## Coding Conventions

*   **Language:** The core library code is written in C++/Arduino.
*   **Identifiers:** All variable and function names should be in English and use `camelCase` for variables and `snake_case` for functions. Class names should use `PascalCase`.
*   **Comments:** Comments and serial output messages should be in German. This is a project convention that we would like to maintain.
*   **Formatting:** Please follow the formatting of the existing code. An automated code formatter may be introduced in the future.

## Submitting a Pull Request

1.  Fork the repository and create a new branch for your feature or bug fix.
2.  Make your changes, adhering to the coding conventions.
3.  If you add a new feature, please update the `README.md` and any relevant documentation.
4.  Ensure that the project builds successfully with `pio run`.
5.  Submit a pull request with a clear description of your changes.

Thank you for your contribution!
